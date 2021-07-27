package awais.instagrabber.db.datasources;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.time.LocalDateTime;
import java.util.List;

import awais.instagrabber.db.AppDatabase;
import awais.instagrabber.db.dao.DMLastNotifiedDao;
import awais.instagrabber.db.entities.DMLastNotified;

public class DMLastNotifiedDataSource {
    private static final String TAG = DMLastNotifiedDataSource.class.getSimpleName();

    private static DMLastNotifiedDataSource INSTANCE;

    private final DMLastNotifiedDao dmLastNotifiedDao;

    private DMLastNotifiedDataSource(final DMLastNotifiedDao dmLastNotifiedDao) {
        this.dmLastNotifiedDao = dmLastNotifiedDao;
    }

    public static DMLastNotifiedDataSource getInstance(@NonNull Context context) {
        if (INSTANCE == null) {
            synchronized (DMLastNotifiedDataSource.class) {
                if (INSTANCE == null) {
                    final AppDatabase database = AppDatabase.getDatabase(context);
                    INSTANCE = new DMLastNotifiedDataSource(database.dmLastNotifiedDao());
                }
            }
        }
        return INSTANCE;
    }

    @Nullable
    public final DMLastNotified getDMLastNotified(final String threadId) {
        return dmLastNotifiedDao.findDMLastNotifiedByThreadId(threadId);
    }

    @NonNull
    public final List<DMLastNotified> getAllDMDmLastNotified() {
        return dmLastNotifiedDao.getAllDMDmLastNotified();
    }

    public final void insertOrUpdateDMLastNotified(final String threadId,
                                                   final LocalDateTime lastNotifiedMsgTs,
                                                   final LocalDateTime lastNotifiedAt) {
        final DMLastNotified dmLastNotified = getDMLastNotified(threadId);
        final DMLastNotified toUpdate = new DMLastNotified(dmLastNotified == null ? 0 : dmLastNotified.getId(),
                                                           threadId,
                                                           lastNotifiedMsgTs,
                                                           lastNotifiedAt);
        if (dmLastNotified != null) {
            dmLastNotifiedDao.updateDMLastNotified(toUpdate);
            return;
        }
        dmLastNotifiedDao.insertDMLastNotified(toUpdate);
    }

    public final void deleteDMLastNotified(@NonNull final DMLastNotified dmLastNotified) {
        dmLastNotifiedDao.deleteDMLastNotified(dmLastNotified);
    }

    public final void deleteAllDMLastNotified() {
        dmLastNotifiedDao.deleteAllDMLastNotified();
    }
}
