package awais.instagrabber.webservices;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableMap;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.repositories.ProfileRepository;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.repositories.responses.UserFeedResponse;
import awais.instagrabber.repositories.responses.WrappedFeedResponse;
import awais.instagrabber.repositories.responses.WrappedMedia;
import awais.instagrabber.repositories.responses.saved.CollectionsListResponse;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ProfileService extends BaseService {
    private static final String TAG = "ProfileService";

    private final ProfileRepository repository;

    private static ProfileService instance;

    private ProfileService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(ProfileRepository.class);
    }

    public static ProfileService getInstance() {
        if (instance == null) {
            instance = new ProfileService();
        }
        return instance;
    }

    public void fetchPosts(final long userId,
                           final String maxId,
                           final ServiceCallback<PostsFetchResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        if (!TextUtils.isEmpty(maxId)) {
            builder.put("max_id", maxId);
        }
        final Call<UserFeedResponse> request = repository.fetch(userId, builder.build());
        request.enqueue(new Callback<UserFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<UserFeedResponse> call, @NonNull final Response<UserFeedResponse> response) {
                if (callback == null) return;
                final UserFeedResponse body = response.body();
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
            public void onFailure(@NonNull final Call<UserFeedResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchSaved(final String maxId,
                           final String collectionId,
                           final ServiceCallback<PostsFetchResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        Call<WrappedFeedResponse> request = null;
        if (!TextUtils.isEmpty(maxId)) {
            builder.put("max_id", maxId);
        }
        if (TextUtils.isEmpty(collectionId) || collectionId.equals("ALL_MEDIA_AUTO_COLLECTION")) request = repository.fetchSaved(builder.build());
        else request = repository.fetchSavedCollection(collectionId, builder.build());
        request.enqueue(new Callback<WrappedFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<WrappedFeedResponse> call, @NonNull final Response<WrappedFeedResponse> response) {
                if (callback == null) return;
                final WrappedFeedResponse userFeedResponse = response.body();
                if (userFeedResponse == null) {
                    callback.onSuccess(null);
                    return;
                }
                final List<WrappedMedia> items = userFeedResponse.getItems();
                final List<Media> posts;
                if (items == null) {
                    posts = Collections.emptyList();
                } else {
                    posts = items.stream()
                                 .map(WrappedMedia::getMedia)
                                 .filter(Objects::nonNull)
                                 .collect(Collectors.toList());
                }
                callback.onSuccess(new PostsFetchResponse(
                        posts,
                        userFeedResponse.isMoreAvailable(),
                        userFeedResponse.getNextMaxId()
                ));
            }

            @Override
            public void onFailure(@NonNull final Call<WrappedFeedResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchCollections(final String maxId,
                                 final ServiceCallback<CollectionsListResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        if (!TextUtils.isEmpty(maxId)) {
            builder.put("max_id", maxId);
        }
        builder.put("collection_types", "[\"ALL_MEDIA_AUTO_COLLECTION\",\"MEDIA\",\"PRODUCT_AUTO_COLLECTION\"]");
        final Call<CollectionsListResponse> request = repository.fetchCollections(builder.build());
        request.enqueue(new Callback<CollectionsListResponse>() {
            @Override
            public void onResponse(@NonNull final Call<CollectionsListResponse> call, @NonNull final Response<CollectionsListResponse> response) {
                if (callback == null) return;
                final CollectionsListResponse collectionsListResponse = response.body();
                if (collectionsListResponse == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(collectionsListResponse);
            }

            @Override
            public void onFailure(@NonNull final Call<CollectionsListResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void createCollection(final String name,
                                 final String deviceUuid,
                                 final long userId,
                                 final String csrfToken,
                                 final ServiceCallback<String> callback) {
        final Map<String, Object> form = new HashMap<>(6);
        form.put("_csrftoken", csrfToken);
        form.put("_uuid", deviceUuid);
        form.put("_uid", userId);
        form.put("collection_visibility", "0"); // 1 for public, planned for future but currently inexistant
        form.put("module_name", "collection_create");
        form.put("name", name);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> request = repository.createCollection(signedForm);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (callback == null) return;
                final String collectionsListResponse = response.body();
                if (collectionsListResponse == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(collectionsListResponse);
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchLiked(final String maxId,
                           final ServiceCallback<PostsFetchResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        if (!TextUtils.isEmpty(maxId)) {
            builder.put("max_id", maxId);
        }
        final Call<UserFeedResponse> request = repository.fetchLiked(builder.build());
        request.enqueue(new Callback<UserFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<UserFeedResponse> call, @NonNull final Response<UserFeedResponse> response) {
                if (callback == null) return;
                final UserFeedResponse userFeedResponse = response.body();
                if (userFeedResponse == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(new PostsFetchResponse(
                        userFeedResponse.getItems(),
                        userFeedResponse.isMoreAvailable(),
                        userFeedResponse.getNextMaxId()
                ));
            }

            @Override
            public void onFailure(@NonNull final Call<UserFeedResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchTagged(final long profileId,
                            final String maxId,
                            final ServiceCallback<PostsFetchResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        if (!TextUtils.isEmpty(maxId)) {
            builder.put("max_id", maxId);
        }
        final Call<UserFeedResponse> request = repository.fetchTagged(profileId, builder.build());
        request.enqueue(new Callback<UserFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<UserFeedResponse> call, @NonNull final Response<UserFeedResponse> response) {
                if (callback == null) return;
                final UserFeedResponse userFeedResponse = response.body();
                if (userFeedResponse == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(new PostsFetchResponse(
                        userFeedResponse.getItems(),
                        userFeedResponse.isMoreAvailable(),
                        userFeedResponse.getNextMaxId()
                ));
            }

            @Override
            public void onFailure(@NonNull final Call<UserFeedResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    // private PostsFetchResponse parseProfilePostsResponse(final String body) throws JSONException {
    //     final JSONObject root = new JSONObject(body);
    //     final boolean moreAvailable = root.optBoolean("more_available");
    //     final String nextMaxId = root.optString("next_max_id");
    //     final JSONArray itemsJson = root.optJSONArray("items");
    //     final List<FeedModel> items = parseItems(itemsJson, false);
    //     return new PostsFetchResponse(
    //             items,
    //             moreAvailable,
    //             nextMaxId
    //     );
    // }

    // private PostsFetchResponse parseSavedPostsResponse(final String body, final boolean isInMedia) throws JSONException {
    //     final JSONObject root = new JSONObject(body);
    //     final boolean moreAvailable = root.optBoolean("more_available");
    //     final String nextMaxId = root.optString("next_max_id");
    //     final int numResults = root.optInt("num_results");
    //     final String status = root.optString("status");
    //     final JSONArray itemsJson = root.optJSONArray("items");
    //     final List<FeedModel> items = parseItems(itemsJson, isInMedia);
    //     return new PostsFetchResponse(
    //             items,
    //             moreAvailable,
    //             nextMaxId
    //     );
    // }

    // private List<FeedModel> parseItems(final JSONArray items, final boolean isInMedia) throws JSONException {
    //     if (items == null) {
    //         return Collections.emptyList();
    //     }
    //     final List<FeedModel> feedModels = new ArrayList<>();
    //     for (int i = 0; i < items.length(); i++) {
    //         final JSONObject itemJson = items.optJSONObject(i);
    //         if (itemJson == null) {
    //             continue;
    //         }
    //         final FeedModel feedModel = ResponseBodyUtils.parseItem(isInMedia ? itemJson.optJSONObject("media") : itemJson);
    //         if (feedModel != null) {
    //             feedModels.add(feedModel);
    //         }
    //     }
    //     return feedModels;
    // }
}
