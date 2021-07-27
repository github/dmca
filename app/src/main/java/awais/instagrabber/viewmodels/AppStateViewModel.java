package awais.instagrabber.viewmodels;

import android.app.Application;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.webservices.ServiceCallback;
import awais.instagrabber.webservices.UserService;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class AppStateViewModel extends AndroidViewModel {
    private static final String TAG = AppStateViewModel.class.getSimpleName();

    private final String cookie;
    private final MutableLiveData<User> currentUser = new MutableLiveData<>();

    private UserService userService;

    public AppStateViewModel(@NonNull final Application application) {
        super(application);
        // Log.d(TAG, "AppStateViewModel: constructor");
        cookie = settingsHelper.getString(Constants.COOKIE);
        final boolean isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) != 0;
        if (!isLoggedIn) return;
        userService = UserService.getInstance();
        // final AccountRepository accountRepository = AccountRepository.getInstance(AccountDataSource.getInstance(application));
        fetchProfileDetails();
    }

    @Nullable
    public User getCurrentUser() {
        return currentUser.getValue();
    }

    public LiveData<User> getCurrentUserLiveData() {
        return currentUser;
    }

    private void fetchProfileDetails() {
        final long uid = CookieUtils.getUserIdFromCookie(cookie);
        if (userService == null) return;
        userService.getUserInfo(uid, new ServiceCallback<User>() {
            @Override
            public void onSuccess(final User user) {
                currentUser.postValue(user);
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
            }
        });
    }
}
