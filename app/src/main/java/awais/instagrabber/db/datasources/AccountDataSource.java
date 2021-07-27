package awais.instagrabber.db.datasources;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.List;

import awais.instagrabber.db.AppDatabase;
import awais.instagrabber.db.dao.AccountDao;
import awais.instagrabber.db.entities.Account;

public class AccountDataSource {
    private static final String TAG = AccountDataSource.class.getSimpleName();

    private static AccountDataSource INSTANCE;

    private final AccountDao accountDao;

    private AccountDataSource(final AccountDao accountDao) {
        this.accountDao = accountDao;
    }

    public static AccountDataSource getInstance(@NonNull Context context) {
        if (INSTANCE == null) {
            synchronized (AccountDataSource.class) {
                if (INSTANCE == null) {
                    final AppDatabase database = AppDatabase.getDatabase(context);
                    INSTANCE = new AccountDataSource(database.accountDao());
                }
            }
        }
        return INSTANCE;
    }

    @Nullable
    public final Account getAccount(final String uid) {
        return accountDao.findAccountByUid(uid);
    }

    @NonNull
    public final List<Account> getAllAccounts() {
        return accountDao.getAllAccounts();
    }

    public final void insertOrUpdateAccount(final String uid,
                                            final String username,
                                            final String cookie,
                                            final String fullName,
                                            final String profilePicUrl) {
        final Account account = getAccount(uid);
        final Account toUpdate = new Account(account == null ? 0 : account.getId(), uid, username, cookie, fullName, profilePicUrl);
        if (account != null) {
            accountDao.updateAccounts(toUpdate);
            return;
        }
        accountDao.insertAccounts(toUpdate);
    }

    public final void deleteAccount(@NonNull final Account account) {
        accountDao.deleteAccounts(account);
    }

    public final void deleteAllAccounts() {
        accountDao.deleteAllAccounts();
    }
}
