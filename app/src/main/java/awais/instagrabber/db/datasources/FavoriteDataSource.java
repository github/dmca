package awais.instagrabber.db.datasources;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.List;

import awais.instagrabber.db.AppDatabase;
import awais.instagrabber.db.dao.FavoriteDao;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.models.enums.FavoriteType;

public class FavoriteDataSource {
    private static final String TAG = FavoriteDataSource.class.getSimpleName();

    private static FavoriteDataSource INSTANCE;

    private final FavoriteDao favoriteDao;

    private FavoriteDataSource(final FavoriteDao favoriteDao) {
        this.favoriteDao = favoriteDao;
    }

    public static synchronized FavoriteDataSource getInstance(@NonNull Context context) {
        if (INSTANCE == null) {
            synchronized (FavoriteDataSource.class) {
                if (INSTANCE == null) {
                    final AppDatabase database = AppDatabase.getDatabase(context);
                    INSTANCE = new FavoriteDataSource(database.favoriteDao());
                }
            }
        }
        return INSTANCE;
    }


    @Nullable
    public final Favorite getFavorite(@NonNull final String query, @NonNull final FavoriteType type) {
        return favoriteDao.findFavoriteByQueryAndType(query, type);
    }

    @NonNull
    public final List<Favorite> getAllFavorites() {
        return favoriteDao.getAllFavorites();
    }

    public final void insertOrUpdateFavorite(@NonNull final Favorite favorite) {
        if (favorite.getId() != 0) {
            favoriteDao.updateFavorites(favorite);
            return;
        }
        favoriteDao.insertFavorites(favorite);
    }

    public final void deleteFavorite(@NonNull final String query, @NonNull final FavoriteType type) {
        final Favorite favorite = getFavorite(query, type);
        if (favorite == null) return;
        favoriteDao.deleteFavorites(favorite);
    }
}
