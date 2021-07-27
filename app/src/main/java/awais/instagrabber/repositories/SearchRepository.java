package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.search.SearchResponse;
import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.QueryMap;
import retrofit2.http.Url;

public interface SearchRepository {
    @GET
    Call<SearchResponse> search(@Url String url, @QueryMap(encoded = true) Map<String, String> queryParams);
}
