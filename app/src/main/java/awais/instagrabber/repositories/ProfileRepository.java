package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.WrappedFeedResponse;
import awais.instagrabber.repositories.responses.saved.CollectionsListResponse;
import awais.instagrabber.repositories.responses.UserFeedResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.Path;
import retrofit2.http.POST;
import retrofit2.http.QueryMap;

public interface ProfileRepository {

    @GET("/api/v1/feed/user/{uid}/")
    Call<UserFeedResponse> fetch(@Path("uid") final long uid, @QueryMap Map<String, String> queryParams);

    @GET("/api/v1/feed/saved/")
    Call<WrappedFeedResponse> fetchSaved(@QueryMap Map<String, String> queryParams);

    @GET("/api/v1/feed/collection/{collectionId}/")
    Call<WrappedFeedResponse> fetchSavedCollection(@Path("collectionId") final String collectionId,
                                                   @QueryMap Map<String, String> queryParams);

    @GET("/api/v1/feed/liked/")
    Call<UserFeedResponse> fetchLiked(@QueryMap Map<String, String> queryParams);

    @GET("/api/v1/usertags/{profileId}/feed/")
    Call<UserFeedResponse> fetchTagged(@Path("profileId") final long profileId, @QueryMap Map<String, String> queryParams);

    @GET("/api/v1/collections/list/")
    Call<CollectionsListResponse> fetchCollections(@QueryMap Map<String, String> queryParams);

    @FormUrlEncoded
    @POST("/api/v1/collections/create/")
    Call<String> createCollection(@FieldMap Map<String, String> signedForm);
}
