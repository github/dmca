package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.StoryStickerResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Path;
import retrofit2.http.QueryMap;
import retrofit2.http.Url;

public interface StoriesRepository {
    @GET("/api/v1/media/{mediaId}/info/")
    Call<String> fetch(@Path("mediaId") final long mediaId);
    // this one is the same as MediaRepository.fetch BUT you need to make sure it's a story

    @GET("/api/v1/feed/reels_tray/")
    Call<String> getFeedStories();

    @GET("/api/v1/highlights/{uid}/highlights_tray/")
    Call<String> fetchHighlights(@Path("uid") final long uid);

    @GET("/api/v1/archive/reel/day_shells/")
    Call<String> fetchArchive(@QueryMap Map<String, String> queryParams);

    @GET
    Call<String> getUserStory(@Url String url);

    @FormUrlEncoded
    @POST("/api/v1/media/{storyId}/{stickerId}/{action}/")
    Call<StoryStickerResponse> respondToSticker(@Path("storyId") String storyId,
                                                @Path("stickerId") String stickerId,
                                                @Path("action") String action,
                                                // story_poll_vote, story_question_response, story_slider_vote, story_quiz_answer
                                                @FieldMap Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v2/media/seen/")
    Call<String> seen(@QueryMap Map<String, String> queryParams, @FieldMap Map<String, String> form);
}
