package awais.instagrabber.db.datasources;

import android.content.Context;

import androidx.annotation.NonNull;

import java.util.List;

import awais.instagrabber.db.AppDatabase;
import awais.instagrabber.db.dao.RecentSearchDao;
import awais.instagrabber.db.entities.RecentSearch;
import awais.instagrabber.models.enums.FavoriteType;

public class RecentSearchDataSource {
    private static final String TAG = RecentSearchDataSource.class.getSimpleName();

    private static RecentSearchDataSource INSTANCE;

    private final RecentSearchDao recentSearchDao;

    private RecentSearchDataSource(final RecentSearchDao recentSearchDao) {
        this.recentSearchDao = recentSearchDao;
    }

    public static synchronized RecentSearchDataSource getInstance(@NonNull Context context) {
        if (INSTANCE == null) {
            synchronized (RecentSearchDataSource.class) {
                if (INSTANCE == null) {
                    final AppDatabase database = AppDatabase.getDatabase(context);
                    INSTANCE = new RecentSearchDataSource(database.recentSearchDao());
                }
            }
        }
        return INSTANCE;
    }

    public RecentSearch getRecentSearchByIgIdAndType(@NonNull final String igId, @NonNull final FavoriteType type) {
        return recentSearchDao.getRecentSearchByIgIdAndType(igId, type);
    }

    @NonNull
    public final List<RecentSearch> getAllRecentSearches() {
        return recentSearchDao.getAllRecentSearches();
    }

    public final void insertOrUpdateRecentSearch(@NonNull final RecentSearch recentSearch) {
        if (recentSearch.getId() != 0) {
            recentSearchDao.updateRecentSearch(recentSearch);
            return;
        }
        recentSearchDao.insertRecentSearch(recentSearch);
    }

    public final void deleteRecentSearch(@NonNull final RecentSearch recentSearch) {
        recentSearchDao.deleteRecentSearch(recentSearch);
    }
}
