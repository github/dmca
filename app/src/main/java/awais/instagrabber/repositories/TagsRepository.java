package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.Hashtag;
import awais.instagrabber.repositories.responses.TagFeedResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.POST;
import retrofit2.http.Path;
import retrofit2.http.QueryMap;

public interface TagsRepository {
    @GET("/api/v1/tags/{tag}/info/")
    Call<Hashtag> fetch(@Path("tag") final String tag);

    @FormUrlEncoded
    @POST("/api/v1/tags/{action}/{tag}/")
    Call<String> changeFollow(@FieldMap final Map<String, String> signedForm,
                              @Path("action") String action,
                              @Path("tag") String tag);

    @GET("/api/v1/feed/tag/{tag}/")
    Call<TagFeedResponse> fetchPosts(@Path("tag") final String tag,
                                     @QueryMap Map<String, String> queryParams);
}
