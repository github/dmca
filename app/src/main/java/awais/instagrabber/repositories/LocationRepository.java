package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.LocationFeedResponse;
import awais.instagrabber.repositories.responses.Place;
import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.Path;
import retrofit2.http.QueryMap;

public interface LocationRepository {
    @GET("/api/v1/locations/{location}/info/")
    Call<Place> fetch(@Path("location") final long locationId);

    @GET("/api/v1/feed/location/{location}/")
    Call<LocationFeedResponse> fetchPosts(@Path("location") final long locationId,
                                          @QueryMap Map<String, String> queryParams);
}
