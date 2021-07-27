package awais.instagrabber.webservices;

import android.util.Log;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableMap;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

import awais.instagrabber.repositories.TagsRepository;
import awais.instagrabber.repositories.responses.Hashtag;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.repositories.responses.TagFeedResponse;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class TagsService extends BaseService {

    private static final String TAG = "TagsService";

    private static TagsService instance;

    private final TagsRepository repository;

    private TagsService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(TagsRepository.class);
    }

    public static TagsService getInstance() {
        if (instance == null) {
            instance = new TagsService();
        }
        return instance;
    }

    public void fetch(@NonNull final String tag,
                      final ServiceCallback<Hashtag> callback) {
        final Call<Hashtag> request = repository.fetch(tag);
        request.enqueue(new Callback<Hashtag>() {
            @Override
            public void onResponse(@NonNull final Call<Hashtag> call, @NonNull final Response<Hashtag> response) {
                if (callback == null) {
                    return;
                }
                callback.onSuccess(response.body());
            }

            @Override
            public void onFailure(@NonNull final Call<Hashtag> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void changeFollow(@NonNull final String action,
                             @NonNull final String tag,
                             @NonNull final String csrfToken,
                             @NonNull final long userId,
                             @NonNull final String deviceUuid,
                             final ServiceCallback<Boolean> callback) {
        final Map<String, Object> form = new HashMap<>(3);
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("_uuid", deviceUuid);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> request = repository.changeFollow(signedForm, action, tag);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String body = response.body();
                if (body == null) {
                    callback.onFailure(new RuntimeException("body is null"));
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    final String status = jsonObject.optString("status");
                    callback.onSuccess(status.equals("ok"));
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse: ", e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                // Log.e(TAG, "onFailure: ", t);
                callback.onFailure(t);
            }
        });
    }

    public void fetchPosts(@NonNull final String tag,
                           final String maxId,
                           final ServiceCallback<PostsFetchResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        if (!TextUtils.isEmpty(maxId)) {
            builder.put("max_id", maxId);
        }
        final Call<TagFeedResponse> request = repository.fetchPosts(tag, builder.build());
        request.enqueue(new Callback<TagFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<TagFeedResponse> call, @NonNull final Response<TagFeedResponse> response) {
                if (callback == null) {
                    return;
                }
                final TagFeedResponse body = response.body();
                if (body == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(new PostsFetchResponse(
                        body.getItems(),
                        body.isMoreAvailable(),
                        body.getNextMaxId()
                ));
            }

            @Override
            public void onFailure(@NonNull final Call<TagFeedResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }
}
