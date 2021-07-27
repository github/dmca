package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.UserFeedResponse;
import awais.instagrabber.repositories.responses.WrappedFeedResponse;
import awais.instagrabber.repositories.responses.saved.CollectionsListResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Path;
import retrofit2.http.QueryMap;

public interface CollectionRepository {

    @FormUrlEncoded
    @POST("/api/v1/collections/{id}/{action}/")
    Call<String> changeCollection(@Path("id") String id,
                                  @Path("action") String action,
                                  @FieldMap Map<String, String> signedForm);
}
