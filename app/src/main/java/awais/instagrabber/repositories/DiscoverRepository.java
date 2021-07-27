package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.discover.TopicalExploreFeedResponse;
import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.QueryMap;

public interface DiscoverRepository {
    @GET("/api/v1/discover/topical_explore/")
    Call<TopicalExploreFeedResponse> topicalExplore(@QueryMap Map<String, String> queryParams);
}
