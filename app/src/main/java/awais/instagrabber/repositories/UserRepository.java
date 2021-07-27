package awais.instagrabber.repositories;

import awais.instagrabber.repositories.responses.FriendshipStatus;
import awais.instagrabber.repositories.responses.UserSearchResponse;
import awais.instagrabber.repositories.responses.WrappedUser;
import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.Path;
import retrofit2.http.Query;

public interface UserRepository {

    @GET("/api/v1/users/{uid}/info/")
    Call<WrappedUser> getUserInfo(@Path("uid") final long uid);

    @GET("/api/v1/users/{username}/usernameinfo/")
    Call<WrappedUser> getUsernameInfo(@Path("username") final String username);

    @GET("/api/v1/friendships/show/{uid}/")
    Call<FriendshipStatus> getUserFriendship(@Path("uid") final long uid);

    @GET("/api/v1/users/search/")
    Call<UserSearchResponse> search(@Query("timezone_offset") float timezoneOffset,
                                    @Query("q") String query);
}
