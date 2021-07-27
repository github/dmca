package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.feed.FeedFetchResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.POST;

public interface FeedRepository {
    @FormUrlEncoded
    @POST("/api/v1/feed/timeline/")
    Call<FeedFetchResponse> fetch(@FieldMap final Map<String, String> signedForm);
}
