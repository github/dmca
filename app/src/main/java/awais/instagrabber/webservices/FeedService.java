package awais.instagrabber.webservices;

import android.util.Log;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import awais.instagrabber.repositories.FeedRepository;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.repositories.responses.feed.EndOfFeedDemarcator;
import awais.instagrabber.repositories.responses.feed.EndOfFeedGroup;
import awais.instagrabber.repositories.responses.feed.EndOfFeedGroupSet;
import awais.instagrabber.repositories.responses.feed.FeedFetchResponse;
import awais.instagrabber.utils.TextUtils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class FeedService extends BaseService {
    private static final String TAG = "FeedService";

    private final FeedRepository repository;

    private static FeedService instance;

    private FeedService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(FeedRepository.class);
    }

    public static FeedService getInstance() {
        if (instance == null) {
            instance = new FeedService();
        }
        return instance;
    }

    public void fetch(final String csrfToken,
                      final String deviceUuid,
                      final String cursor,
                      final ServiceCallback<PostsFetchResponse> callback) {
        final Map<String, String> form = new HashMap<>();
        form.put("_uuid", deviceUuid);
        form.put("_csrftoken", csrfToken);
        form.put("phone_id", UUID.randomUUID().toString());
        form.put("device_id", UUID.randomUUID().toString());
        form.put("client_session_id", UUID.randomUUID().toString());
        form.put("is_prefetch", "0");
        if (!TextUtils.isEmpty(cursor)) {
            form.put("max_id", cursor);
            form.put("reason", "pagination");
        } else {
            form.put("is_pull_to_refresh", "1");
            form.put("reason", "pull_to_refresh");
        }
        final Call<FeedFetchResponse> request = repository.fetch(form);
        request.enqueue(new Callback<FeedFetchResponse>() {
            @Override
            public void onResponse(@NonNull final Call<FeedFetchResponse> call, @NonNull final Response<FeedFetchResponse> response) {
                try {
                    // Log.d(TAG, "onResponse: body: " + response.body());
                    final PostsFetchResponse postsFetchResponse = parseResponse(response);
                    if (callback != null) {
                        callback.onSuccess(postsFetchResponse);
                    }
                } catch (Exception e) {
                    Log.e(TAG, "onResponse", e);
                    if (callback != null) {
                        callback.onFailure(e);
                    }
                }
            }

            @Override
            public void onFailure(@NonNull final Call<FeedFetchResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });

    }

    @NonNull
    private PostsFetchResponse parseResponse(@NonNull final Response<FeedFetchResponse> response) {
        final FeedFetchResponse feedFetchResponse = response.body();
        if (feedFetchResponse == null) {
            Log.e(TAG, "parseResponse: feed response body is empty with status code: " + response.code());
            return new PostsFetchResponse(Collections.emptyList(), false, null);
        }
        return parseResponseBody(feedFetchResponse);
    }

    @NonNull
    private PostsFetchResponse parseResponseBody(@NonNull final FeedFetchResponse feedFetchResponse) {
        final boolean moreAvailable = feedFetchResponse.isMoreAvailable();
        String nextMaxId = feedFetchResponse.getNextMaxId();
        final boolean needNewMaxId = nextMaxId.equals("feed_recs_head_load");
        final List<Media> allPosts = new ArrayList<>();
        final List<Media> items = feedFetchResponse.getItems();
        for (final Media media : items) {
            if (needNewMaxId && media.getEndOfFeedDemarcator() != null) {
                final EndOfFeedDemarcator endOfFeedDemarcator = media.getEndOfFeedDemarcator();
                final EndOfFeedGroupSet groupSet = endOfFeedDemarcator.getGroupSet();
                if (groupSet == null) continue;
                final List<EndOfFeedGroup> groups = groupSet.getGroups();
                if (groups == null) continue;
                for (final EndOfFeedGroup group : groups) {
                    final String id = group.getId();
                    if (id == null || !id.equals("past_posts")) continue;
                    nextMaxId = group.getNextMaxId();
                    final List<Media> feedItems = group.getFeedItems();
                    for (final Media feedItem : feedItems) {
                        if (feedItem == null || feedItem.isInjected() || feedItem.getMediaType() == null) continue;
                        allPosts.add(feedItem);
                    }
                }
                continue;
            }
            if (media == null || media.isInjected() || media.getMediaType() == null) continue;
            allPosts.add(media);
        }
        return new PostsFetchResponse(allPosts, moreAvailable, nextMaxId);
    }
}