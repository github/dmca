package awais.instagrabber.db.dao;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.Query;
import androidx.room.Update;

import java.util.List;

import awais.instagrabber.db.entities.RecentSearch;
import awais.instagrabber.models.enums.FavoriteType;

@Dao
public interface RecentSearchDao {

    @Query("SELECT * FROM recent_searches ORDER BY last_searched_on DESC")
    List<RecentSearch> getAllRecentSearches();

    @Query("SELECT * FROM recent_searches WHERE `ig_id` = :igId AND `type` = :type")
    RecentSearch getRecentSearchByIgIdAndType(String igId, FavoriteType type);

    @Query("SELECT * FROM recent_searches WHERE instr(`name`, :query) > 0")
    List<RecentSearch> findRecentSearchesWithNameContaining(String query);

    @Insert
    Long insertRecentSearch(RecentSearch recentSearch);

    @Update
    void updateRecentSearch(RecentSearch recentSearch);

    @Delete
    void deleteRecentSearch(RecentSearch recentSearch);

    // @Query("DELETE from recent_searches")
    // void deleteAllRecentSearches();
}
