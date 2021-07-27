package awais.instagrabber.db.repositories;

import java.util.List;

import awais.instagrabber.db.datasources.FavoriteDataSource;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.utils.AppExecutors;

public class FavoriteRepository {
    private static final String TAG = FavoriteRepository.class.getSimpleName();

    private static FavoriteRepository instance;

    private final AppExecutors appExecutors;
    private final FavoriteDataSource favoriteDataSource;

    private FavoriteRepository(final AppExecutors appExecutors, final FavoriteDataSource favoriteDataSource) {
        this.appExecutors = appExecutors;
        this.favoriteDataSource = favoriteDataSource;
    }

    public static FavoriteRepository getInstance(final FavoriteDataSource favoriteDataSource) {
        if (instance == null) {
            instance = new FavoriteRepository(AppExecutors.getInstance(), favoriteDataSource);
        }
        return instance;
    }

    public void getFavorite(final String query, final FavoriteType type, final RepositoryCallback<Favorite> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final Favorite favorite = favoriteDataSource.getFavorite(query, type);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (favorite == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                callback.onSuccess(favorite);
            });
        });
    }

    public void getAllFavorites(final RepositoryCallback<List<Favorite>> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            final List<Favorite> favorites = favoriteDataSource.getAllFavorites();
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                if (favorites == null) {
                    callback.onDataNotAvailable();
                    return;
                }
                callback.onSuccess(favorites);
            });
        });
    }

    public void insertOrUpdateFavorite(final Favorite favorite,
                                       final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            favoriteDataSource.insertOrUpdateFavorite(favorite);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }

    public void deleteFavorite(final String query,
                               final FavoriteType type,
                               final RepositoryCallback<Void> callback) {
        // request on the I/O thread
        appExecutors.diskIO().execute(() -> {
            favoriteDataSource.deleteFavorite(query, type);
            // notify on the main thread
            appExecutors.mainThread().execute(() -> {
                if (callback == null) return;
                callback.onSuccess(null);
            });
        });
    }
}
