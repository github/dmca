package awais.instagrabber.webservices;

import android.text.TextUtils;

import androidx.annotation.NonNull;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.repositories.CollectionRepository;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.utils.Utils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class CollectionService extends BaseService {
    private static final String TAG = "ProfileService";

    private final CollectionRepository repository;
    private final String deviceUuid, csrfToken;
    private final long userId;

    private static CollectionService instance;

    private CollectionService(final String deviceUuid,
                              final String csrfToken,
                              final long userId) {
        this.deviceUuid = deviceUuid;
        this.csrfToken = csrfToken;
        this.userId = userId;
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(CollectionRepository.class);
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

    public static CollectionService getInstance(final String deviceUuid, final String csrfToken, final long userId) {
        if (instance == null
                || !Objects.equals(instance.getCsrfToken(), csrfToken)
                || !Objects.equals(instance.getDeviceUuid(), deviceUuid)
                || !Objects.equals(instance.getUserId(), userId)) {
            instance = new CollectionService(deviceUuid, csrfToken, userId);
        }
        return instance;
    }

    public void addPostsToCollection(final String collectionId,
                                     final List<Media> posts,
                                     final ServiceCallback<String> callback) {
        final Map<String, Object> form = new HashMap<>(2);
        form.put("module_name", "feed_saved_add_to_collection");
        final List<String> ids;
        ids = posts.stream()
                   .map(Media::getPk)
                   .filter(Objects::nonNull)
                   .collect(Collectors.toList());
        form.put("added_media_ids", "[" + TextUtils.join(",", ids) + "]");
        changeCollection(collectionId, "edit", form, callback);
    }

    public void editCollectionName(final String collectionId,
                                   final String name,
                                   final ServiceCallback<String> callback) {
        final Map<String, Object> form = new HashMap<>(1);
        form.put("name", name);
        changeCollection(collectionId, "edit", form, callback);
    }

    public void deleteCollection(final String collectionId,
                                 final ServiceCallback<String> callback) {
        changeCollection(collectionId, "delete", null, callback);
    }

    public void changeCollection(final String collectionId,
                                 final String action,
                                 final Map<String, Object> options,
                                 final ServiceCallback<String> callback) {
        final Map<String, Object> form = new HashMap<>();
        form.put("_csrftoken", csrfToken);
        form.put("_uuid", deviceUuid);
        form.put("_uid", userId);
        if (options != null) form.putAll(options);
        final Map<String, String> signedForm = Utils.sign(form);
        final Call<String> request = repository.changeCollection(collectionId, action, signedForm);
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
}
