package awais.instagrabber.webservices;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableMap;

import awais.instagrabber.repositories.LocationRepository;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.LocationFeedResponse;
import awais.instagrabber.repositories.responses.Place;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.utils.TextUtils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class LocationService extends BaseService {
    private static final String TAG = "LocationService";

    private final LocationRepository repository;

    private static LocationService instance;

    private LocationService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(LocationRepository.class);
    }

    public static LocationService getInstance() {
        if (instance == null) {
            instance = new LocationService();
        }
        return instance;
    }

    public void fetchPosts(final long locationId,
                           final String maxId,
                           final ServiceCallback<PostsFetchResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        if (!TextUtils.isEmpty(maxId)) {
            builder.put("max_id", maxId);
        }
        final Call<LocationFeedResponse> request = repository.fetchPosts(locationId, builder.build());
        request.enqueue(new Callback<LocationFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<LocationFeedResponse> call, @NonNull final Response<LocationFeedResponse> response) {
                if (callback == null) return;
                final LocationFeedResponse body = response.body();
                if (body == null) {
                    callback.onSuccess(null);
                    return;
                }
                final PostsFetchResponse postsFetchResponse = new PostsFetchResponse(
                        body.getItems(),
                        body.isMoreAvailable(),
                        body.getNextMaxId()
                );
                callback.onSuccess(postsFetchResponse);

            }

            @Override
            public void onFailure(@NonNull final Call<LocationFeedResponse> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }

    public void fetch(@NonNull final long locationId,
                      final ServiceCallback<Location> callback) {
        final Call<Place> request = repository.fetch(locationId);
        request.enqueue(new Callback<Place>() {
            @Override
            public void onResponse(@NonNull final Call<Place> call, @NonNull final Response<Place> response) {
                if (callback == null) {
                    return;
                }
                callback.onSuccess(response.body() == null ? null : response.body().getLocation());
            }

            @Override
            public void onFailure(@NonNull final Call<Place> call, @NonNull final Throwable t) {
                if (callback != null) {
                    callback.onFailure(t);
                }
            }
        });
    }
}
