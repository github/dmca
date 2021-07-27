package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.AymlResponse;
import awais.instagrabber.repositories.responses.NewsInboxResponse;
import awais.instagrabber.repositories.responses.UserSearchResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.POST;
import retrofit2.http.Query;

public interface NewsRepository {
    @GET("/api/v1/news/inbox/")
    Call<NewsInboxResponse> appInbox(@Query(value = "mark_as_seen", encoded = true) boolean markAsSeen,
                                     @Header(value = "x-ig-app-id") String xIgAppId);

    @FormUrlEncoded
    @POST("/api/v1/discover/ayml/")
    Call<AymlResponse> getAyml(@FieldMap final Map<String, String> form);

    @GET("/api/v1/discover/chaining/")
    Call<UserSearchResponse> getChaining(@Query(value = "target_id") long targetId);
}
