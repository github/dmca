package awais.instagrabber.repositories;

import awais.instagrabber.repositories.responses.giphy.GiphyGifResponse;
import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.Query;

public interface GifRepository {

    @GET("/api/v1/creatives/story_media_search_keyed_format/")
    Call<GiphyGifResponse> searchGiphyGifs(@Query("request_surface") final String requestSurface,
                                           @Query("q") final String query,
                                           @Query("media_types") final String mediaTypes);
}
