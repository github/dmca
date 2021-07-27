package awais.instagrabber.db.dao;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.OnConflictStrategy;
import androidx.room.Query;
import androidx.room.Update;

import java.util.List;

import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.models.enums.FavoriteType;

@Dao
public interface FavoriteDao {

    @Query("SELECT * FROM favorites")
    List<Favorite> getAllFavorites();

    @Query("SELECT * FROM favorites WHERE query_text = :query and type = :type")
    Favorite findFavoriteByQueryAndType(String query, FavoriteType type);

    @Insert
    List<Long> insertFavorites(Favorite... favorites);

    @Update
    void updateFavorites(Favorite... favorites);

    @Delete
    void deleteFavorites(Favorite... favorites);

    @Query("DELETE from favorites")
    void deleteAllFavorites();
}
