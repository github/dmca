package awais.instagrabber.db.dao;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.OnConflictStrategy;
import androidx.room.Query;
import androidx.room.Update;

import java.util.List;

import awais.instagrabber.db.entities.DMLastNotified;

@Dao
public interface DMLastNotifiedDao {

    @Query("SELECT * FROM dm_last_notified")
    List<DMLastNotified> getAllDMDmLastNotified();

    @Query("SELECT * FROM dm_last_notified WHERE thread_id = :threadId")
    DMLastNotified findDMLastNotifiedByThreadId(String threadId);

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    List<Long> insertDMLastNotified(DMLastNotified... dmLastNotified);

    @Update
    void updateDMLastNotified(DMLastNotified... dmLastNotified);

    @Delete
    void deleteDMLastNotified(DMLastNotified... dmLastNotified);

    @Query("DELETE from dm_last_notified")
    void deleteAllDMLastNotified();
}
