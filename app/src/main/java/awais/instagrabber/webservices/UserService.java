package awais.instagrabber.webservices;

import androidx.annotation.NonNull;

import java.util.TimeZone;

import awais.instagrabber.repositories.UserRepository;
import awais.instagrabber.repositories.responses.FriendshipStatus;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.UserSearchResponse;
import awais.instagrabber.repositories.responses.WrappedUser;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class UserService extends BaseService {
    private static final String TAG = UserService.class.getSimpleName();

    private final UserRepository repository;

    private static UserService instance;

    private UserService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(UserRepository.class);
    }

    public static UserService getInstance() {
        if (instance == null) {
            instance = new UserService();
        }
        return instance;
    }

    public void getUserInfo(final long uid, final ServiceCallback<User> callback) {
        final Call<WrappedUser> request = repository.getUserInfo(uid);
        request.enqueue(new Callback<WrappedUser>() {
            @Override
            public void onResponse(@NonNull final Call<WrappedUser> call, @NonNull final Response<WrappedUser> response) {
                final WrappedUser user = response.body();
                if (user == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(user.getUser());
            }

            @Override
            public void onFailure(@NonNull final Call<WrappedUser> call, @NonNull final Throwable t) {
                callback.onFailure(t);
            }
        });
    }

    public void getUsernameInfo(final String username, final ServiceCallback<User> callback) {
        final Call<WrappedUser> request = repository.getUsernameInfo(username);
        request.enqueue(new Callback<WrappedUser>() {
            @Override
            public void onResponse(@NonNull final Call<WrappedUser> call, @NonNull final Response<WrappedUser> response) {
                final WrappedUser user = response.body();
                if (user == null) {
                    callback.onFailure(null);
                    return;
                }
                callback.onSuccess(user.getUser());
            }

            @Override
            public void onFailure(@NonNull final Call<WrappedUser> call, @NonNull final Throwable t) {
                callback.onFailure(t);
            }
        });
    }

    public void getUserFriendship(final long uid, final ServiceCallback<FriendshipStatus> callback) {
        final Call<FriendshipStatus> request = repository.getUserFriendship(uid);
        request.enqueue(new Callback<FriendshipStatus>() {
            @Override
            public void onResponse(@NonNull final Call<FriendshipStatus> call, @NonNull final Response<FriendshipStatus> response) {
                final FriendshipStatus status = response.body();
                if (status == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(status);
            }

            @Override
            public void onFailure(@NonNull final Call<FriendshipStatus> call, @NonNull final Throwable t) {
                callback.onFailure(t);
            }
        });
    }


    public Call<UserSearchResponse> search(final String query) {
        final float timezoneOffset = (float) TimeZone.getDefault().getRawOffset() / 1000;
        return repository.search(timezoneOffset, query);
    }
}
