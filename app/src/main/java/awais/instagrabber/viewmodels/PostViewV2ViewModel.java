package awais.instagrabber.viewmodels;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.google.common.collect.ImmutableList;

import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.models.Resource;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Caption;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.webservices.MediaService;
import awais.instagrabber.webservices.ServiceCallback;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class PostViewV2ViewModel extends ViewModel {
    private static final String TAG = PostViewV2ViewModel.class.getSimpleName();

    private final MutableLiveData<User> user = new MutableLiveData<>();
    private final MutableLiveData<Caption> caption = new MutableLiveData<>();
    private final MutableLiveData<Location> location = new MutableLiveData<>();
    private final MutableLiveData<String> date = new MutableLiveData<>();
    private final MutableLiveData<Long> likeCount = new MutableLiveData<>(0L);
    private final MutableLiveData<Long> commentCount = new MutableLiveData<>(0L);
    private final MutableLiveData<Long> viewCount = new MutableLiveData<>(0L);
    private final MutableLiveData<MediaItemType> type = new MutableLiveData<>();
    private final MutableLiveData<Boolean> liked = new MutableLiveData<>(false);
    private final MutableLiveData<Boolean> saved = new MutableLiveData<>(false);
    private final MutableLiveData<List<Integer>> options = new MutableLiveData<>(new ArrayList<>());
    private final MediaService mediaService;
    private final long viewerId;
    private final boolean isLoggedIn;

    private Media media;

    public PostViewV2ViewModel() {
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        final String deviceUuid = settingsHelper.getString(Constants.DEVICE_UUID);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        viewerId = CookieUtils.getUserIdFromCookie(cookie);
        mediaService = MediaService.getInstance(deviceUuid, csrfToken, viewerId);
        isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) > 0;
    }

    public void setMedia(final Media media) {
        this.media = media;
        user.postValue(media.getUser());
        caption.postValue(media.getCaption());
        location.postValue(media.getLocation());
        date.postValue(media.getDate());
        likeCount.postValue(media.getLikeCount());
        commentCount.postValue(media.getCommentCount());
        viewCount.postValue(media.getMediaType() == MediaItemType.MEDIA_TYPE_VIDEO ? media.getViewCount() : null);
        type.postValue(media.getMediaType());
        liked.postValue(media.hasLiked());
        saved.postValue(media.hasViewerSaved());
        initOptions();
    }

    private void initOptions() {
        final ImmutableList.Builder<Integer> builder = ImmutableList.builder();
        if (isLoggedIn && media.getUser() != null && media.getUser().getPk() == viewerId) {
            builder.add(R.id.edit_caption);
            builder.add(R.id.delete);
        }
        options.postValue(builder.build());
    }

    public Media getMedia() {
        return media;
    }

    public boolean isLoggedIn() {
        return isLoggedIn;
    }

    public LiveData<User> getUser() {
        return user;
    }

    public LiveData<Caption> getCaption() {
        return caption;
    }

    public LiveData<Location> getLocation() {
        return location;
    }

    public LiveData<String> getDate() {
        return date;
    }

    public LiveData<Long> getLikeCount() {
        return likeCount;
    }

    public LiveData<Long> getCommentCount() {
        return commentCount;
    }

    public LiveData<Long> getViewCount() {
        return viewCount;
    }

    public LiveData<MediaItemType> getType() {
        return type;
    }

    public LiveData<Boolean> getLiked() {
        return liked;
    }

    public LiveData<Boolean> getSaved() {
        return saved;
    }

    public LiveData<List<Integer>> getOptions() {
        return options;
    }

    @NonNull
    public LiveData<Resource<Object>> toggleLike() {
        if (media.hasLiked()) {
            return unlike();
        }
        return like();
    }

    public LiveData<Resource<Object>> like() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        mediaService.like(media.getPk(), getLikeUnlikeCallback(data));
        return data;
    }

    public LiveData<Resource<Object>> unlike() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        mediaService.unlike(media.getPk(), getLikeUnlikeCallback(data));
        return data;
    }

    @NonNull
    private ServiceCallback<Boolean> getLikeUnlikeCallback(final MutableLiveData<Resource<Object>> data) {
        return new ServiceCallback<Boolean>() {
            @Override
            public void onSuccess(final Boolean result) {
                if (!result) {
                    data.postValue(Resource.error("", null));
                    return;
                }
                data.postValue(Resource.success(true));
                final long currentLikesCount = media.getLikeCount();
                final long updatedCount;
                if (!media.hasLiked()) {
                    updatedCount = currentLikesCount + 1;
                    media.setHasLiked(true);
                } else {
                    updatedCount = currentLikesCount - 1;
                    media.setHasLiked(false);
                }
                media.setLikeCount(updatedCount);
                likeCount.postValue(updatedCount);
                liked.postValue(media.hasLiked());
            }

            @Override
            public void onFailure(final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), null));
                Log.e(TAG, "Error during like/unlike", t);
            }
        };
    }

    @NonNull
    public LiveData<Resource<Object>> toggleSave() {
        if (!media.hasViewerSaved()) {
            return save(null, false);
        }
        return unsave();
    }

    @NonNull
    public LiveData<Resource<Object>> toggleSave(final String collection, final boolean ignoreSaveState) {
        return save(collection, ignoreSaveState);
    }

    public LiveData<Resource<Object>> save(final String collection, final boolean ignoreSaveState) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        mediaService.save(media.getPk(), collection, getSaveUnsaveCallback(data, ignoreSaveState));
        return data;
    }

    public LiveData<Resource<Object>> unsave() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        mediaService.unsave(media.getPk(), getSaveUnsaveCallback(data, false));
        return data;
    }

    @NonNull
    private ServiceCallback<Boolean> getSaveUnsaveCallback(final MutableLiveData<Resource<Object>> data,
                                                           final boolean ignoreSaveState) {
        return new ServiceCallback<Boolean>() {
            @Override
            public void onSuccess(final Boolean result) {
                if (!result) {
                    data.postValue(Resource.error("", null));
                    return;
                }
                data.postValue(Resource.success(true));
                if (!ignoreSaveState) media.setHasViewerSaved(!media.hasViewerSaved());
                saved.postValue(media.hasViewerSaved());
            }

            @Override
            public void onFailure(final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), null));
                Log.e(TAG, "Error during save/unsave", t);
            }
        };
    }

    public LiveData<Resource<Object>> updateCaption(final String caption) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        mediaService.editCaption(media.getPk(), caption, new ServiceCallback<Boolean>() {
            @Override
            public void onSuccess(final Boolean result) {
                if (result) {
                    data.postValue(Resource.success(""));
                    media.setPostCaption(caption);
                    PostViewV2ViewModel.this.caption.postValue(media.getCaption());
                    return;
                }
                data.postValue(Resource.error("", null));
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "Error editing caption", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<String>> translateCaption() {
        final MutableLiveData<Resource<String>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Caption value = caption.getValue();
        if (value == null) return data;
        mediaService.translate(String.valueOf(value.getPk()), "1", new ServiceCallback<String>() {
            @Override
            public void onSuccess(final String result) {
                if (TextUtils.isEmpty(result)) {
                    data.postValue(Resource.error("", null));
                    return;
                }
                data.postValue(Resource.success(result));
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "Error translating comment", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public boolean hasPk() {
        return media.getPk() != null;
    }

    public void setViewCount(final Long viewCount) {
        this.viewCount.postValue(viewCount);
    }

    public LiveData<Resource<Object>> delete() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Call<String> request = mediaService.delete(media.getId(), media.getMediaType());
        if (request == null) {
            data.postValue(Resource.success(new Object()));
            return data;
        }
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    data.postValue(Resource.error(R.string.generic_null_response, null));
                    return;
                }
                final String body = response.body();
                if (body == null) {
                    data.postValue(Resource.error(R.string.generic_null_response, null));
                    return;
                }
                data.postValue(Resource.success(new Object()));
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }
}
