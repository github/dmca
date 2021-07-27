package awais.instagrabber.db.repositories;

import java.util.List;

import awais.instagrabber.db.datasources.AccountDataSource;
import awais.instagrabber.db.entities.Account;
import awais.instagrabber.utils.AppExecutors;

public class AccountRepository {
    private static final String TAG = AccountRepository.class.getSimpleName();

    private static AccountRepository instance;

    private final AppExecutors appExecutors;
    private final AccountDataSource accountDataSource;

    // private List<Account> cachedAccounts;

    private AccountRepository(final AppExecutors appExecutors, final AccountDataSource accountDataSource) {
        this.appExecutors = appExecutors;
        this.accountDataSource = accountDataSource;
    }

    public static AccountRepository getInstance(final AccountDataSource accountDataSource) {
        if (instance == null) {
            instance = new AccountRepository(AppExecutors.getInstance(), accountDataSource);
        }
        return instance;
    }

    public void getAccount(final long uid,
                           final RepositoryCallback<Account> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final Account account = accountDataSource.getAccount(String.valueOf(uid));
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (account == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                callback.onSuccess(account);
            });
        });
    }

    public void getAllAccounts(final RepositoryCallback<List<Account>> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final List<Account> accounts = accountDataSource.getAllAccounts();
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (accounts == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                // cachedAccounts = accounts;
                callback.onSuccess(accounts);
            });
        });
    }

    public void insertOrUpdateAccounts(final List<Account> accounts,
                                       final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            for (final Account account : accounts) {
                accountDataSource.insertOrUpdateAccount(account.getUid(),
                                                        account.getUsername(),
                                                        account.getCookie(),
                                                        account.getFullName(),
                                                        account.getProfilePic());
            }
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

    public void insertOrUpdateAccount(final long uid,
                                      final String username,
                                      final String cookie,
                                      final String fullName,
                                      final String profilePicUrl,
                                      final RepositoryCallback<Account> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            accountDataSource.insertOrUpdateAccount(String.valueOf(uid), username, cookie, fullName, profilePicUrl);
            final Account updated = accountDataSource.getAccount(String.valueOf(uid));
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (updated == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                callback.onSuccess(updated);
            });
        });
    }

    public void deleteAccount(final Account account,
                              final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            accountDataSource.deleteAccount(account);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

    public void deleteAllAccounts(final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            accountDataSource.deleteAllAccounts();
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

}
