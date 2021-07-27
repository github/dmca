package awais.instagrabber.db.repositories;

import androidx.annotation.NonNull;

import java.time.LocalDateTime;
import java.util.List;

import awais.instagrabber.db.datasources.RecentSearchDataSource;
import awais.instagrabber.db.entities.RecentSearch;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.utils.AppExecutors;

public class RecentSearchRepository {
    private static final String TAG = RecentSearchRepository.class.getSimpleName();

    private static RecentSearchRepository instance;

    private final AppExecutors appExecutors;
    private final RecentSearchDataSource recentSearchDataSource;

    private RecentSearchRepository(final AppExecutors appExecutors, final RecentSearchDataSource recentSearchDataSource) {
        this.appExecutors = appExecutors;
        this.recentSearchDataSource = recentSearchDataSource;
    }

    public static RecentSearchRepository getInstance(final RecentSearchDataSource recentSearchDataSource) {
        if (instance == null) {
            instance = new RecentSearchRepository(AppExecutors.getInstance(), recentSearchDataSource);
        }
        return instance;
    }

    public void getRecentSearch(@NonNull final String igId,
                                @NonNull final FavoriteType type,
                                final RepositoryCallback<RecentSearch> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final RecentSearch recentSearch = recentSearchDataSource.getRecentSearchByIgIdAndType(igId, type);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (recentSearch == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                callback.onSuccess(recentSearch);
            });
        });
    }

    public void getAllRecentSearches(final RepositoryCallback<List<RecentSearch>> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final List<RecentSearch> recentSearches = recentSearchDataSource.getAllRecentSearches();
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(recentSearches);
            });
        });
    }

    public void insertOrUpdateRecentSearch(@NonNull final RecentSearch recentSearch,
                                           final RepositoryCallback<Void> callback) {
        insertOrUpdateRecentSearch(recentSearch.getIgId(), recentSearch.getName(), recentSearch.getUsername(), recentSearch.getPicUrl(),
                                   recentSearch.getType(), callback);
    }

    public void insertOrUpdateRecentSearch(@NonNull final String igId,
                                           @NonNull final String name,
                                           final String username,
                                           final String picUrl,
                                           @NonNull final FavoriteType type,
                                           final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            RecentSearch recentSearch = recentSearchDataSource.getRecentSearchByIgIdAndType(igId, type);
            recentSearch = recentSearch == null
                           ? new RecentSearch(igId, name, username, picUrl, type, LocalDateTime.now())
                           : new RecentSearch(recentSearch.getId(), igId, name, username, picUrl, type, LocalDateTime.now());
            recentSearchDataSource.insertOrUpdateRecentSearch(recentSearch);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

    public void deleteRecentSearchByIgIdAndType(@NonNull final String igId,
                                                @NonNull final FavoriteType type,
                                                final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final RecentSearch recentSearch = recentSearchDataSource.getRecentSearchByIgIdAndType(igId, type);
            if (recentSearch != null) {
                recentSearchDataSource.deleteRecentSearch(recentSearch);
            }
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (recentSearch == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                callback.onSuccess(null);
            });
        });
    }

    public void deleteRecentSearch(@NonNull final RecentSearch recentSearch,
                                   final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {

            recentSearchDataSource.deleteRecentSearch(recentSearch);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }
}
