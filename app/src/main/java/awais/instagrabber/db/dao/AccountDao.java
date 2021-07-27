package awais.instagrabber.db.dao;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.OnConflictStrategy;
import androidx.room.Query;
import androidx.room.Update;

import java.util.List;

import awais.instagrabber.db.entities.Account;

@Dao
public interface AccountDao {

    @Query("SELECT * FROM accounts")
    List<Account> getAllAccounts();

    @Query("SELECT * FROM accounts WHERE uid = :uid")
    Account findAccountByUid(String uid);

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    List<Long> insertAccounts(Account... accounts);

    @Update
    void updateAccounts(Account... accounts);

    @Delete
    void deleteAccounts(Account... accounts);

    @Query("DELETE from accounts")
    void deleteAllAccounts();
}
