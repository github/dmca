package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.FriendshipChangeResponse;
import awais.instagrabber.repositories.responses.FriendshipRestrictResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Path;
import retrofit2.http.QueryMap;

public interface FriendshipRepository {

    @FormUrlEncoded
    @POST("/api/v1/friendships/{action}/{id}/")
    Call<FriendshipChangeResponse> change(@Path("action") String action,
                                          @Path("id") long id,
                                          @FieldMap Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/restrict_action/{action}/")
    Call<FriendshipRestrictResponse> toggleRestrict(@Path("action") String action,
                                                    @FieldMap Map<String, String> form);

    @GET("/api/v1/friendships/{userId}/{type}/")
    Call<String> getList(@Path("userId") long userId,
                         @Path("type") String type, // following or followers
                         @QueryMap(encoded = true) Map<String, String> queryParams);

    @FormUrlEncoded
    @POST("/api/v1/friendships/{action}/")
    Call<FriendshipChangeResponse> changeMute(@Path("action") String action,
                                              @FieldMap Map<String, String> form);
}
