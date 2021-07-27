package awais.instagrabber.webservices;

import android.util.Log;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableMap;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import awais.instagrabber.models.enums.FollowingType;
import awais.instagrabber.repositories.GraphQLRepository;
import awais.instagrabber.repositories.responses.FriendshipStatus;
import awais.instagrabber.repositories.responses.GraphQLUserListFetchResponse;
import awais.instagrabber.repositories.responses.Hashtag;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.TextUtils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class GraphQLService extends BaseService {
    private static final String TAG = "GraphQLService";

    private final GraphQLRepository repository;

    private static GraphQLService instance;

    private GraphQLService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofitWeb()
                                    .create(GraphQLRepository.class);
    }

    public static GraphQLService getInstance() {
        if (instance == null) {
            instance = new GraphQLService();
        }
        return instance;
    }

    private void fetch(final String queryHash,
                       final String variables,
                       final String arg1,
                       final String arg2,
                       final User backup,
                       final ServiceCallback<PostsFetchResponse> callback) {
        final Map<String, String> queryMap = new HashMap<>();
        queryMap.put("query_hash", queryHash);
        queryMap.put("variables", variables);
        final Call<String> request = repository.fetch(queryMap);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                try {
                    // Log.d(TAG, "onResponse: body: " + response.body());
                    final PostsFetchResponse postsFetchResponse = parsePostResponse(response, arg1, arg2, backup);
                    if (callback != null) {
                        callback.onSuccess(postsFetchResponse);
                    }
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse", e);
                    if (callback != null) {
                        callback.onFailure(e);
                    }
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchLocationPosts(final long locationId,
                                   final String maxId,
                                   final ServiceCallback<PostsFetchResponse> callback) {
        fetch("36bd0f2bf5911908de389b8ceaa3be6d",
              "{\"id\":\"" + locationId + "\"," +
                      "\"first\":25," +
                      "\"after\":\"" + (maxId == null ? "" : maxId) + "\"}",
              Constants.EXTRAS_LOCATION,
              "edge_location_to_media",
              null,
              callback);
    }

    public void fetchHashtagPosts(@NonNull final String tag,
                                  final String maxId,
                                  final ServiceCallback<PostsFetchResponse> callback) {
        fetch("9b498c08113f1e09617a1703c22b2f32",
              "{\"tag_name\":\"" + tag + "\"," +
                      "\"first\":25," +
                      "\"after\":\"" + (maxId == null ? "" : maxId) + "\"}",
              Constants.EXTRAS_HASHTAG,
              "edge_hashtag_to_media",
              null,
              callback);
    }

    public void fetchProfilePosts(final long profileId,
                                  final int postsPerPage,
                                  final String maxId,
                                  final User backup,
                                  final ServiceCallback<PostsFetchResponse> callback) {
        fetch("18a7b935ab438c4514b1f742d8fa07a7",
              "{\"id\":\"" + profileId + "\"," +
                      "\"first\":" + postsPerPage + "," +
                      "\"after\":\"" + (maxId == null ? "" : maxId) + "\"}",
              Constants.EXTRAS_USER,
              "edge_owner_to_timeline_media",
              backup,
              callback);
    }

    public void fetchTaggedPosts(final long profileId,
                                 final int postsPerPage,
                                 final String maxId,
                                 final ServiceCallback<PostsFetchResponse> callback) {
        fetch("31fe64d9463cbbe58319dced405c6206",
              "{\"id\":\"" + profileId + "\"," +
                      "\"first\":" + postsPerPage + "," +
                      "\"after\":\"" + (maxId == null ? "" : maxId) + "\"}",
              Constants.EXTRAS_USER,
              "edge_user_to_photos_of_you",
              null,
              callback);
    }

    @NonNull
    private PostsFetchResponse parsePostResponse(@NonNull final Response<String> response,
                                                 @NonNull final String arg1,
                                                 @NonNull final String arg2,
                                                 final User backup)
            throws JSONException {
        if (TextUtils.isEmpty(response.body())) {
            Log.e(TAG, "parseResponse: feed response body is empty with status code: " + response.code());
            return new PostsFetchResponse(Collections.emptyList(), false, null);
        }
        return parseResponseBody(response.body(), arg1, arg2, backup);
    }

    @NonNull
    private PostsFetchResponse parseResponseBody(@NonNull final String body,
                                                 @NonNull final String arg1,
                                                 @NonNull final String arg2,
                                                 final User backup)
            throws JSONException {
        final List<Media> items = new ArrayList<>();
        final JSONObject timelineFeed = new JSONObject(body)
                .getJSONObject("data")
                .getJSONObject(arg1)
                .getJSONObject(arg2);
        final String endCursor;
        final boolean hasNextPage;

        final JSONObject pageInfo = timelineFeed.getJSONObject("page_info");
        if (pageInfo.has("has_next_page")) {
            hasNextPage = pageInfo.getBoolean("has_next_page");
            endCursor = hasNextPage ? pageInfo.getString("end_cursor") : null;
        } else {
            hasNextPage = false;
            endCursor = null;
        }

        final JSONArray feedItems = timelineFeed.getJSONArray("edges");

        for (int i = 0; i < feedItems.length(); ++i) {
            final JSONObject itemJson = feedItems.optJSONObject(i);
            if (itemJson == null) {
                continue;
            }
            final Media media = ResponseBodyUtils.parseGraphQLItem(itemJson, backup);
            if (media != null) {
                items.add(media);
            }
        }
        return new PostsFetchResponse(items, hasNextPage, endCursor);
    }

    public void fetchCommentLikers(final String commentId,
                                   final String endCursor,
                                   final ServiceCallback<GraphQLUserListFetchResponse> callback) {
        final Map<String, String> queryMap = new HashMap<>();
        queryMap.put("query_hash", "5f0b1f6281e72053cbc07909c8d154ae");
        queryMap.put("variables", "{\"comment_id\":\"" + commentId + "\"," +
                "\"first\":30," +
                "\"after\":\"" + (endCursor == null ? "" : endCursor) + "\"}");
        final Call<String> request = repository.fetch(queryMap);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String rawBody = response.body();
                if (rawBody == null) {
                    Log.e(TAG, "Error occurred while fetching gql comment likes of " + commentId);
                    callback.onSuccess(null);
                    return;
                }
                try {
                    final JSONObject body = new JSONObject(rawBody);
                    final String status = body.getString("status");
                    final JSONObject data = body.getJSONObject("data").getJSONObject("comment").getJSONObject("edge_liked_by");
                    final JSONObject pageInfo = data.getJSONObject("page_info");
                    final String endCursor = pageInfo.getBoolean("has_next_page") ? pageInfo.getString("end_cursor") : null;
                    final JSONArray users = data.getJSONArray("edges");
                    final int usersLen = users.length();
                    final List<User> userModels = new ArrayList<>();
                    for (int j = 0; j < usersLen; ++j) {
                        final JSONObject userObject = users.getJSONObject(j).getJSONObject("node");
                        userModels.add(new User(
                                userObject.getLong("id"),
                                userObject.getString("username"),
                                userObject.optString("full_name"),
                                userObject.optBoolean("is_private"),
                                userObject.getString("profile_pic_url"),
                                userObject.optBoolean("is_verified")
                        ));
                        // userModels.add(new ProfileModel(userObject.optBoolean("is_private"),
                        //                                 false,
                        //                                 userObject.optBoolean("is_verified"),
                        //                                 userObject.getString("id"),
                        //                                 userObject.getString("username"),
                        //                                 userObject.optString("full_name"),
                        //                                 null, null,
                        //                                 userObject.getString("profile_pic_url"),
                        //                                 null, 0, 0, 0, false, false, false, false, false));
                    }
                    callback.onSuccess(new GraphQLUserListFetchResponse(endCursor, status, userModels));
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse", e);
                    if (callback != null) {
                        callback.onFailure(e);
                    }
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public Call<String> fetchComments(final String shortCodeOrCommentId,
                                      final boolean root,
                                      final String cursor) {
        final Map<String, String> queryMap = new HashMap<>();
        queryMap.put("query_hash", root ? "bc3296d1ce80a24b1b6e40b1e72903f5" : "51fdd02b67508306ad4484ff574a0b62");
        final Map<String, Object> variables = ImmutableMap.of(
                root ? "shortcode" : "comment_id", shortCodeOrCommentId,
                "first", 50,
                "after", cursor == null ? "" : cursor
        );
        queryMap.put("variables", new JSONObject(variables).toString());
        return repository.fetch(queryMap);
    }

    public void fetchUser(final String username,
                          final ServiceCallback<User> callback) {
        final Call<String> request = repository.getUser(username);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String rawBody = response.body();
                if (rawBody == null) {
                    Log.e(TAG, "Error occurred while fetching gql user of " + username);
                    callback.onSuccess(null);
                    return;
                }
                try {
                    final JSONObject body = new JSONObject(rawBody);
                    final JSONObject userJson = body.getJSONObject("graphql")
                                                    .getJSONObject(Constants.EXTRAS_USER);

                    boolean isPrivate = userJson.getBoolean("is_private");
                    final long id = userJson.optLong(Constants.EXTRAS_ID, 0);
                    final JSONObject timelineMedia = userJson.getJSONObject("edge_owner_to_timeline_media");
                    // if (timelineMedia.has("edges")) {
                    //     final JSONArray edges = timelineMedia.getJSONArray("edges");
                    // }

                    String url = userJson.optString("external_url");
                    if (TextUtils.isEmpty(url)) url = null;

                    callback.onSuccess(new User(
                            id,
                            username,
                            userJson.getString("full_name"),
                            isPrivate,
                            userJson.getString("profile_pic_url_hd"),
                            null,
                            new FriendshipStatus(
                                    userJson.optBoolean("followed_by_viewer"),
                                    userJson.optBoolean("follows_viewer"),
                                    userJson.optBoolean("blocked_by_viewer"),
                                    false,
                                    isPrivate,
                                    userJson.optBoolean("has_requested_viewer"),
                                    userJson.optBoolean("requested_by_viewer"),
                                    false,
                                    userJson.optBoolean("restricted_by_viewer"),
                                    false
                            ),
                            userJson.getBoolean("is_verified"),
                            false,
                            false,
                            false,
                            false,
                            false,
                            null,
                            null,
                            timelineMedia.getLong("count"),
                            userJson.getJSONObject("edge_followed_by").getLong("count"),
                            userJson.getJSONObject("edge_follow").getLong("count"),
                            0,
                            userJson.getString("biography"),
                            url,
                            0,
                            null,
                            null,
                            null,
                            null,
                            null,
                            null));
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse", e);
                    if (callback != null) {
                        callback.onFailure(e);
                    }
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchPost(final String shortcode,
                          final ServiceCallback<Media> callback) {
        final Call<String> request = repository.getPost(shortcode);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String rawBody = response.body();
                if (rawBody == null) {
                    Log.e(TAG, "Error occurred while fetching gql post of " + shortcode);
                    callback.onSuccess(null);
                    return;
                }
                try {
                    final JSONObject body = new JSONObject(rawBody);
                    final JSONObject media = body.getJSONObject("graphql")
                            .getJSONObject("shortcode_media");
                    callback.onSuccess(ResponseBodyUtils.parseGraphQLItem(media, null));
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse", e);
                    if (callback != null) {
                        callback.onFailure(e);
                    }
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchTag(final String tag,
                         final ServiceCallback<Hashtag> callback) {
        final Call<String> request = repository.getTag(tag);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String rawBody = response.body();
                if (rawBody == null) {
                    Log.e(TAG, "Error occurred while fetching gql tag of " + tag);
                    callback.onSuccess(null);
                    return;
                }
                try {
                    final JSONObject body = new JSONObject(rawBody)
                            .getJSONObject("graphql")
                            .getJSONObject(Constants.EXTRAS_HASHTAG);
                    final JSONObject timelineMedia = body.getJSONObject("edge_hashtag_to_media");
                    callback.onSuccess(new Hashtag(
                            body.getString(Constants.EXTRAS_ID),
                            body.getString("name"),
                            timelineMedia.getLong("count"),
                            body.optBoolean("is_following") ? FollowingType.FOLLOWING : FollowingType.NOT_FOLLOWING,
                            null));
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse", e);
                    if (callback != null) {
                        callback.onFailure(e);
                    }
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetchLocation(final long locationId,
                              final ServiceCallback<Location> callback) {
        final Call<String> request = repository.getLocation(locationId);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String rawBody = response.body();
                if (rawBody == null) {
                    Log.e(TAG, "Error occurred while fetching gql location of " + locationId);
                    callback.onSuccess(null);
                    return;
                }
                try {
                    final JSONObject body = new JSONObject(rawBody)
                            .getJSONObject("graphql")
                            .getJSONObject(Constants.EXTRAS_LOCATION);
                    final JSONObject timelineMedia = body.getJSONObject("edge_location_to_media");
                    final JSONObject address = new JSONObject(body.getString("address_json"));
                    callback.onSuccess(new Location(
                            body.getLong(Constants.EXTRAS_ID),
                            body.getString("slug"),
                            body.getString("name"),
                            address.optString("street_address"),
                            address.optString("city_name"),
                            body.optDouble("lng", 0d),
                            body.optDouble("lat", 0d)
                    ));
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse", e);
                    if (callback != null) {
                        callback.onFailure(e);
                    }
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }
}
