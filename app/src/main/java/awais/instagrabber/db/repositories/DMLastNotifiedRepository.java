package awais.instagrabber.db.repositories;

import java.time.LocalDateTime;
import java.util.List;

import awais.instagrabber.db.datasources.DMLastNotifiedDataSource;
import awais.instagrabber.db.entities.DMLastNotified;
import awais.instagrabber.utils.AppExecutors;

public class DMLastNotifiedRepository {
    private static final String TAG = DMLastNotifiedRepository.class.getSimpleName();

    private static DMLastNotifiedRepository instance;

    private final AppExecutors appExecutors;
    private final DMLastNotifiedDataSource dmLastNotifiedDataSource;

    private DMLastNotifiedRepository(final AppExecutors appExecutors, final DMLastNotifiedDataSource dmLastNotifiedDataSource) {
        this.appExecutors = appExecutors;
        this.dmLastNotifiedDataSource = dmLastNotifiedDataSource;
    }

    public static DMLastNotifiedRepository getInstance(final DMLastNotifiedDataSource dmLastNotifiedDataSource) {
        if (instance == null) {
            instance = new DMLastNotifiedRepository(AppExecutors.getInstance(), dmLastNotifiedDataSource);
        }
        return instance;
    }

    public void getDMLastNotified(final String threadId,
                                  final RepositoryCallback<DMLastNotified> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final DMLastNotified dmLastNotified = dmLastNotifiedDataSource.getDMLastNotified(threadId);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (dmLastNotified == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                callback.onSuccess(dmLastNotified);
            });
        });
    }

    public void getAllDMDmLastNotified(final RepositoryCallback<List<DMLastNotified>> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final List<DMLastNotified> allDMDmLastNotified = dmLastNotifiedDataSource.getAllDMDmLastNotified();
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (allDMDmLastNotified == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                // cachedAccounts = accounts;
                callback.onSuccess(allDMDmLastNotified);
            });
        });
    }

    public void insertOrUpdateDMLastNotified(final List<DMLastNotified> dmLastNotifiedList,
                                             final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            for (final DMLastNotified dmLastNotified : dmLastNotifiedList) {
                dmLastNotifiedDataSource.insertOrUpdateDMLastNotified(dmLastNotified.getThreadId(),
                                                                      dmLastNotified.getLastNotifiedMsgTs(),
                                                                      dmLastNotified.getLastNotifiedAt());
            }
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

    public void insertOrUpdateDMLastNotified(final String threadId,
                                             final LocalDateTime lastNotifiedMsgTs,
                                             final LocalDateTime lastNotifiedAt,
                                             final RepositoryCallback<DMLastNotified> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            dmLastNotifiedDataSource.insertOrUpdateDMLastNotified(threadId, lastNotifiedMsgTs, lastNotifiedAt);
            final DMLastNotified updated = dmLastNotifiedDataSource.getDMLastNotified(threadId);
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

    public void deleteDMLastNotified(final DMLastNotified dmLastNotified,
                                     final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            dmLastNotifiedDataSource.deleteDMLastNotified(dmLastNotified);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

    public void deleteAllDMLastNotified(final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            dmLastNotifiedDataSource.deleteAllDMLastNotified();
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

}
