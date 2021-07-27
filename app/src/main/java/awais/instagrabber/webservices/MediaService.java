package awais.instagrabber.webservices;

import android.util.Log;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableMap;
import com.google.gson.Gson;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;

import awais.instagrabber.models.Comment;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.MediaRepository;
import awais.instagrabber.repositories.requests.UploadFinishOptions;
import awais.instagrabber.repositories.responses.LikersResponse;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.MediaInfoResponse;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.DateUtils;
import awais.instagrabber.utils.MediaUploadHelper;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class MediaService extends BaseService {
    private static final String TAG = "MediaService";
    private static final List<MediaItemType> DELETABLE_ITEMS_TYPES = ImmutableList.of(MediaItemType.MEDIA_TYPE_IMAGE,
                                                                                      MediaItemType.MEDIA_TYPE_VIDEO,
                                                                                      MediaItemType.MEDIA_TYPE_SLIDER);

    private final MediaRepository repository;
    private final String deviceUuid, csrfToken;
    private final long userId;

    private static MediaService instance;

    private MediaService(final String deviceUuid,
                         final String csrfToken,
                         final long userId) {
        this.deviceUuid = deviceUuid;
        this.csrfToken = csrfToken;
        this.userId = userId;
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(MediaRepository.class);
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

    public static MediaService getInstance(final String deviceUuid, final String csrfToken, final long userId) {
        if (instance == null
                || !Objects.equals(instance.getCsrfToken(), csrfToken)
                || !Objects.equals(instance.getDeviceUuid(), deviceUuid)
                || !Objects.equals(instance.getUserId(), userId)) {
            instance = new MediaService(deviceUuid, csrfToken, userId);
        }
        return instance;
    }

    public void fetch(final long mediaId,
                      final ServiceCallback<Media> callback) {
        final Call<MediaInfoResponse> request = repository.fetch(mediaId);
        request.enqueue(new Callback<MediaInfoResponse>() {
            @Override
            public void onResponse(@NonNull final Call<MediaInfoResponse> call,
                                   @NonNull final Response<MediaInfoResponse> response) {
                if (callback == null) return;
                final MediaInfoResponse mediaInfoResponse = response.body();
                if (mediaInfoResponse == null || mediaInfoResponse.getItems() == null || mediaInfoResponse.getItems().isEmpty()) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(mediaInfoResponse.getItems().get(0));
            }

            @Override
            public void onFailure(@NonNull final Call<MediaInfoResponse> call,
                                  @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void like(final String mediaId,
                     final ServiceCallback<Boolean> callback) {
        action(mediaId, "like", null, callback);
    }

    public void unlike(final String mediaId,
                       final ServiceCallback<Boolean> callback) {
        action(mediaId, "unlike", null, callback);
    }

    public void save(final String mediaId,
                     final String collection,
                     final ServiceCallback<Boolean> callback) {
        action(mediaId, "save", collection, callback);
    }

    public void unsave(final String mediaId,
                       final ServiceCallback<Boolean> callback) {
        action(mediaId, "unsave", null, callback);
    }

    private void action(final String mediaId,
                        final String action,
                        final String collection,
                        final ServiceCallback<Boolean> callback) {
        final Map<String, Object> form = new HashMap<>();
        form.put("media_id", mediaId);
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("_uuid", deviceUuid);
        // form.put("radio_type", "wifi-none");
        if (action.equals("save") && !TextUtils.isEmpty(collection)) form.put("added_collection_ids", "[" + collection + "]");
        // there also exists "removed_collection_ids" which can be used with "save" and "unsave"
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> request = repository.action(action, mediaId, signedForm);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call,
                                   @NonNull final Response<String> response) {
                if (callback == null) return;
                final String body = response.body();
                if (body == null) {
                    callback.onFailure(new RuntimeException("Returned body is null"));
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    final String status = jsonObject.optString("status");
                    callback.onSuccess(status.equals("ok"));
                } catch (JSONException e) {
                    callback.onFailure(e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call,
                                  @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void comment(@NonNull final String mediaId,
                        @NonNull final String comment,
                        final String replyToCommentId,
                        @NonNull final ServiceCallback<Comment> callback) {
        final String module = "self_comments_v2";
        final Map<String, Object> form = new HashMap<>();
        // form.put("user_breadcrumb", userBreadcrumb(comment.length()));
        form.put("idempotence_token", UUID.randomUUID().toString());
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("_uuid", deviceUuid);
        form.put("comment_text", comment);
        form.put("containermodule", module);
        if (!TextUtils.isEmpty(replyToCommentId)) {
            form.put("replied_to_comment_id", replyToCommentId);
        }
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> commentRequest = repository.comment(mediaId, signedForm);
        commentRequest.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String body = response.body();
                if (body == null) {
                    Log.e(TAG, "Error occurred while creating comment");
                    callback.onSuccess(null);
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    // final String status = jsonObject.optString("status");
                    final JSONObject commentJsonObject = jsonObject.optJSONObject("comment");
                    Comment comment = null;
                    if (commentJsonObject != null) {
                        final JSONObject userJsonObject = commentJsonObject.optJSONObject("user");
                        if (userJsonObject != null) {
                            final Gson gson = new Gson();
                            final User user = gson.fromJson(userJsonObject.toString(), User.class);
                            comment = new Comment(
                                    commentJsonObject.optString("pk"),
                                    commentJsonObject.optString("text"),
                                    commentJsonObject.optLong("created_at"),
                                    0,
                                    false,
                                    user,
                                    0,
                                    !TextUtils.isEmpty(replyToCommentId)
                            );
                        }
                    }
                    callback.onSuccess(comment);
                } catch (Exception e) {
                    callback.onFailure(e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                callback.onFailure(t);
            }
        });
    }

    public void deleteComment(final String mediaId,
                              final String commentId,
                              @NonNull final ServiceCallback<Boolean> callback) {
        deleteComments(mediaId, Collections.singletonList(commentId), callback);
    }

    public void deleteComments(final String mediaId,
                               final List<String> commentIds,
                               @NonNull final ServiceCallback<Boolean> callback) {
        final Map<String, Object> form = new HashMap<>();
        form.put("comment_ids_to_delete", android.text.TextUtils.join(",", commentIds));
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("_uuid", deviceUuid);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> bulkDeleteRequest = repository.commentsBulkDelete(mediaId, signedForm);
        bulkDeleteRequest.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String body = response.body();
                if (body == null) {
                    Log.e(TAG, "Error occurred while deleting comments");
                    callback.onSuccess(false);
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    final String status = jsonObject.optString("status");
                    callback.onSuccess(status.equals("ok"));
                } catch (JSONException e) {
                    // Log.e(TAG, "Error parsing body", e);
                    callback.onFailure(e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                // Log.e(TAG, "Error deleting comments", t);
                callback.onFailure(t);
            }
        });
    }

    public void commentLike(@NonNull final String commentId,
                            @NonNull final ServiceCallback<Boolean> callback) {
        final Map<String, Object> form = new HashMap<>();
        form.put("_csrftoken", csrfToken);
        // form.put("_uid", userId);
        // form.put("_uuid", deviceUuid);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> commentLikeRequest = repository.commentLike(commentId, signedForm);
        commentLikeRequest.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String body = response.body();
                if (body == null) {
                    Log.e(TAG, "Error occurred while liking comment");
                    callback.onSuccess(false);
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    final String status = jsonObject.optString("status");
                    callback.onSuccess(status.equals("ok"));
                } catch (JSONException e) {
                    // Log.e(TAG, "Error parsing body", e);
                    callback.onFailure(e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "Error liking comment", t);
                callback.onFailure(t);
            }
        });
    }

    public void commentUnlike(final String commentId,
                              @NonNull final ServiceCallback<Boolean> callback) {
        final Map<String, Object> form = new HashMap<>();
        form.put("_csrftoken", csrfToken);
        // form.put("_uid", userId);
        // form.put("_uuid", deviceUuid);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> commentUnlikeRequest = repository.commentUnlike(commentId, signedForm);
        commentUnlikeRequest.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String body = response.body();
                if (body == null) {
                    Log.e(TAG, "Error occurred while unliking comment");
                    callback.onSuccess(false);
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    final String status = jsonObject.optString("status");
                    callback.onSuccess(status.equals("ok"));
                } catch (JSONException e) {
                    // Log.e(TAG, "Error parsing body", e);
                    callback.onFailure(e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "Error unliking comment", t);
                callback.onFailure(t);
            }
        });
    }

    public void editCaption(final String postId,
                            final String newCaption,
                            @NonNull final ServiceCallback<Boolean> callback) {
        final Map<String, Object> form = new HashMap<>();
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("_uuid", deviceUuid);
        form.put("igtv_feed_preview", "false");
        form.put("media_id", postId);
        form.put("caption_text", newCaption);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> request = repository.editCaption(postId, signedForm);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String body = response.body();
                if (body == null) {
                    Log.e(TAG, "Error occurred while editing caption");
                    callback.onSuccess(false);
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    final String status = jsonObject.optString("status");
                    callback.onSuccess(status.equals("ok"));
                } catch (JSONException e) {
                    // Log.e(TAG, "Error parsing body", e);
                    callback.onFailure(e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "Error editing caption", t);
                callback.onFailure(t);
            }
        });
    }

    public void fetchLikes(final String mediaId,
                           final boolean isComment,
                           @NonNull final ServiceCallback<List<User>> callback) {
        final Call<LikersResponse> likesRequest = repository.fetchLikes(mediaId, isComment ? "comment_likers" : "likers");
        likesRequest.enqueue(new Callback<LikersResponse>() {
            @Override
            public void onResponse(@NonNull final Call<LikersResponse> call, @NonNull final Response<LikersResponse> response) {
                final LikersResponse likersResponse = response.body();
                if (likersResponse == null) {
                    Log.e(TAG, "Error occurred while fetching likes of " + mediaId);
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(likersResponse.getUsers());
            }

            @Override
            public void onFailure(@NonNull final Call<LikersResponse> call, @NonNull final Throwable t) {
                Log.e(TAG, "Error getting likes", t);
                callback.onFailure(t);
            }
        });
    }

    public void translate(final String id,
                          final String type, // 1 caption 2 comment 3 bio
                          @NonNull final ServiceCallback<String> callback) {
        final Map<String, String> form = new HashMap<>();
        form.put("id", String.valueOf(id));
        form.put("type", type);
        final Call<String> request = repository.translate(form);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                final String body = response.body();
                if (body == null) {
                    Log.e(TAG, "Error occurred while translating");
                    callback.onSuccess(null);
                    return;
                }
                try {
                    final JSONObject jsonObject = new JSONObject(body);
                    final String translation = jsonObject.optString("translation");
                    callback.onSuccess(translation);
                } catch (JSONException e) {
                    // Log.e(TAG, "Error parsing body", e);
                    callback.onFailure(e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "Error translating", t);
                callback.onFailure(t);
            }
        });
    }

    public Call<String> uploadFinish(@NonNull final UploadFinishOptions options) {
        if (options.getVideoOptions() != null) {
            final UploadFinishOptions.VideoOptions videoOptions = options.getVideoOptions();
            if (videoOptions.getClips() == null) {
                videoOptions.setClips(Collections.singletonList(
                        new UploadFinishOptions.Clip()
                                .setLength(videoOptions.getLength())
                                .setSourceType(options.getSourceType())
                ));
            }
        }
        final String timezoneOffset = String.valueOf(DateUtils.getTimezoneOffset());
        final ImmutableMap.Builder<String, Object> formBuilder = ImmutableMap.<String, Object>builder()
                .put("timezone_offset", timezoneOffset)
                .put("_csrftoken", csrfToken)
                .put("source_type", options.getSourceType())
                .put("_uid", String.valueOf(userId))
                .put("_uuid", deviceUuid)
                .put("upload_id", options.getUploadId());
        if (options.getVideoOptions() != null) {
            formBuilder.putAll(options.getVideoOptions().getMap());
        }
        final Map<String, String> queryMap = options.getVideoOptions() != null ? ImmutableMap.of("video", "1") : Collections.emptyMap();
        final Map<String, String> signedForm = Utils.sign(formBuilder.build());
        return repository.uploadFinish(MediaUploadHelper.getRetryContextString(), queryMap, signedForm);
    }

    public Call<String> delete(@NonNull final String postId,
                               @NonNull final MediaItemType type) {
        if (!DELETABLE_ITEMS_TYPES.contains(type)) return null;
        final Map<String, Object> form = new HashMap<>();
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("_uuid", deviceUuid);
        form.put("igtv_feed_preview", "false");
        form.put("media_id", postId);
        final Map<String, String> signedForm = Utils.sign(form);
        final String mediaType;
        switch (type) {
            case MEDIA_TYPE_IMAGE:
                mediaType = "PHOTO";
                break;
            case MEDIA_TYPE_VIDEO:
                mediaType = "VIDEO";
                break;
            case MEDIA_TYPE_SLIDER:
                mediaType = "CAROUSEL";
                break;
            default:
                return null;
        }
        return repository.delete(postId, mediaType, signedForm);
    }
}
