package awais.instagrabber.webservices;

import android.util.Log;

import androidx.annotation.NonNull;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import awais.instagrabber.models.FollowModel;
import awais.instagrabber.repositories.FriendshipRepository;
import awais.instagrabber.repositories.responses.FriendshipChangeResponse;
import awais.instagrabber.repositories.responses.FriendshipListFetchResponse;
import awais.instagrabber.repositories.responses.FriendshipRestrictResponse;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class FriendshipService extends BaseService {
    private static final String TAG = "FriendshipService";

    private final FriendshipRepository repository;
    private final String deviceUuid, csrfToken;
    private final long userId;

    private static FriendshipService instance;

    private FriendshipService(final String deviceUuid,
                              final String csrfToken,
                              final long userId) {
        this.deviceUuid = deviceUuid;
        this.csrfToken = csrfToken;
        this.userId = userId;
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(FriendshipRepository.class);
    }

    public String getCsrfToken() {
        return csrfToken;
    }

    public String getDeviceUuid() {
        return deviceUuid;
    }

    public long getUserId() {
        return userId;
    }

    public static FriendshipService getInstance(final String deviceUuid, final String csrfToken, final long userId) {
        if (instance == null
                || !Objects.equals(instance.getCsrfToken(), csrfToken)
                || !Objects.equals(instance.getDeviceUuid(), deviceUuid)
                || !Objects.equals(instance.getUserId(), userId)) {
            instance = new FriendshipService(deviceUuid, csrfToken, userId);
        }
        return instance;
    }

    public void follow(final long targetUserId,
                       final ServiceCallback<FriendshipChangeResponse> callback) {
        change("create", targetUserId, callback);
    }

    public void unfollow(final long targetUserId,
                         final ServiceCallback<FriendshipChangeResponse> callback) {
        change("destroy", targetUserId, callback);
    }

    public void changeBlock(final boolean unblock,
                            final long targetUserId,
                            final ServiceCallback<FriendshipChangeResponse> callback) {
        change(unblock ? "unblock" : "block", targetUserId, callback);
    }

    public void toggleRestrict(final long targetUserId,
                               final boolean restrict,
                               final ServiceCallback<FriendshipRestrictResponse> callback) {
        final Map<String, String> form = new HashMap<>(3);
        form.put("_csrftoken", csrfToken);
        form.put("_uuid", deviceUuid);
        form.put("target_user_id", String.valueOf(targetUserId));
        final String action = restrict ? "restrict" : "unrestrict";
        final Call<FriendshipRestrictResponse> request = repository.toggleRestrict(action, form);
        request.enqueue(new Callback<FriendshipRestrictResponse>() {
            @Override
            public void onResponse(@NonNull final Call<FriendshipRestrictResponse> call,
                                   @NonNull final Response<FriendshipRestrictResponse> response) {
                if (callback != null) {
                    callback.onSuccess(response.body());
                }
            }

            @Override
            public void onFailure(@NonNull final Call<FriendshipRestrictResponse> call,
                                  @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void approve(final long targetUserId,
                        final ServiceCallback<FriendshipChangeResponse> callback) {
        change("approve", targetUserId, callback);
    }

    public void ignore(final long targetUserId,
                       final ServiceCallback<FriendshipChangeResponse> callback) {
        change("ignore", targetUserId, callback);
    }

    public void removeFollower(final long targetUserId,
                               final ServiceCallback<FriendshipChangeResponse> callback) {
        change("remove_follower", targetUserId, callback);
    }

    private void change(final String action,
                        final long targetUserId,
                        final ServiceCallback<FriendshipChangeResponse> callback) {
        final Map<String, Object> form = new HashMap<>(5);
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("_uuid", deviceUuid);
        form.put("radio_type", "wifi-none");
        form.put("user_id", targetUserId);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<FriendshipChangeResponse> request = repository.change(action, targetUserId, signedForm);
        request.enqueue(new Callback<FriendshipChangeResponse>() {
            @Override
            public void onResponse(@NonNull final Call<FriendshipChangeResponse> call,
                                   @NonNull final Response<FriendshipChangeResponse> response) {
                if (callback != null) {
                    callback.onSuccess(response.body());
                }
            }

            @Override
            public void onFailure(@NonNull final Call<FriendshipChangeResponse> call,
                                  @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void changeMute(final boolean unmute,
                           final long targetUserId,
                           final boolean story, // true for story, false for posts
                           final ServiceCallback<FriendshipChangeResponse> callback) {
        final Map<String, String> form = new HashMap<>(4);
        form.put("_csrftoken", csrfToken);
        form.put("_uid", String.valueOf(userId));
        form.put("_uuid", deviceUuid);
        form.put(story ? "target_reel_author_id" : "target_posts_author_id", String.valueOf(targetUserId));
        final Call<FriendshipChangeResponse> request = repository.changeMute(unmute ?
                                                                             "unmute_posts_or_story_from_follow" :
                                                                             "mute_posts_or_story_from_follow",
                                                                             form);
        request.enqueue(new Callback<FriendshipChangeResponse>() {
            @Override
            public void onResponse(@NonNull final Call<FriendshipChangeResponse> call,
                                   @NonNull final Response<FriendshipChangeResponse> response) {
                if (callback != null) {
                    callback.onSuccess(response.body());
                }
            }

            @Override
            public void onFailure(@NonNull final Call<FriendshipChangeResponse> call,
                                  @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void getList(final boolean follower,
                        final long targetUserId,
                        final String maxId,
                        final ServiceCallback<FriendshipListFetchResponse> callback) {
        final Map<String, String> queryMap = new HashMap<>();
        if (maxId != null) queryMap.put("max_id", maxId);
        final Call<String> request = repository.getList(
                targetUserId,
                follower ? "followers" : "following",
                queryMap);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                try {
                    if (callback == null) {
                        return;
                    }
                    final String body = response.body();
                    if (TextUtils.isEmpty(body)) {
                        callback.onSuccess(null);
                        return;
                    }
                    final FriendshipListFetchResponse friendshipListFetchResponse = parseListResponse(body);
                    callback.onSuccess(friendshipListFetchResponse);
                } catch (JSONException e) {
                    Log.e(TAG, "onResponse", e);
                    callback.onFailure(e);
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

    private FriendshipListFetchResponse parseListResponse(@NonNull final String body) throws JSONException {
        final JSONObject root = new JSONObject(body);
        final String nextMaxId = root.optString("next_max_id");
        final String status = root.optString("status");
        final JSONArray itemsJson = root.optJSONArray("users");
        final List<FollowModel> items = parseItems(itemsJson);
        return new FriendshipListFetchResponse(
                nextMaxId,
                status,
                items
        );
    }

    private List<FollowModel> parseItems(final JSONArray items) throws JSONException {
        if (items == null) {
            return Collections.emptyList();
        }
        final List<FollowModel> followModels = new ArrayList<>();
        for (int i = 0; i < items.length(); i++) {
            final JSONObject itemJson = items.optJSONObject(i);
            if (itemJson == null) {
                continue;
            }
            final FollowModel followModel = new FollowModel(itemJson.getString("pk"),
                                                            itemJson.getString("username"),
                                                            itemJson.optString("full_name"),
                                                            itemJson.getString("profile_pic_url"));
            if (followModel != null) {
                followModels.add(followModel);
            }
        }
        return followModels;
    }
}
