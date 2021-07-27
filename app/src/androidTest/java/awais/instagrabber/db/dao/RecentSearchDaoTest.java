package awais.instagrabber.db.dao;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.room.Room;
import androidx.test.core.app.ApplicationProvider;
import androidx.test.runner.AndroidJUnit4;

import com.google.common.collect.ImmutableList;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.jupiter.api.Assertions;
import org.junit.runner.RunWith;

import java.time.LocalDateTime;
import java.util.List;

import awais.instagrabber.db.AppDatabase;
import awais.instagrabber.db.entities.RecentSearch;
import awais.instagrabber.models.enums.FavoriteType;

@RunWith(AndroidJUnit4.class)
public class RecentSearchDaoTest {
    private static final String TAG = RecentSearchDaoTest.class.getSimpleName();

    private RecentSearchDao dao;
    private AppDatabase db;

    @Before
    public void createDb() {
        final Context context = ApplicationProvider.getApplicationContext();
        db = Room.inMemoryDatabaseBuilder(context, AppDatabase.class).build();
        dao = db.recentSearchDao();
    }

    @After
    public void closeDb() {
        db.close();
    }

    @Test
    public void writeQueryDelete() {
        final RecentSearch recentSearch = insertRecentSearch("1", "test1", FavoriteType.HASHTAG);
        final RecentSearch byIgIdAndType = dao.getRecentSearchByIgIdAndType("1", FavoriteType.HASHTAG);
        Assertions.assertEquals(recentSearch, byIgIdAndType);
        dao.deleteRecentSearch(byIgIdAndType);
        final RecentSearch deleted = dao.getRecentSearchByIgIdAndType("1", FavoriteType.HASHTAG);
        Assertions.assertNull(deleted);
    }

    @Test
    public void queryAllOrdered() {
        final List<RecentSearch> insertListReversed = ImmutableList
                .<RecentSearch>builder()
                .add(insertRecentSearch("1", "test1", FavoriteType.HASHTAG))
                .add(insertRecentSearch("2", "test2", FavoriteType.LOCATION))
                .add(insertRecentSearch("3", "test3", FavoriteType.USER))
                .add(insertRecentSearch("4", "test4", FavoriteType.USER))
                .add(insertRecentSearch("5", "test5", FavoriteType.USER))
                .build()
                .reverse(); // important
        final List<RecentSearch> fromDb = dao.getAllRecentSearches();
        Assertions.assertIterableEquals(insertListReversed, fromDb);
    }

    @NonNull
    private RecentSearch insertRecentSearch(final String igId, final String name, final FavoriteType type) {
        final RecentSearch recentSearch = new RecentSearch(
                igId,
                name,
                null,
                null,
                type,
                LocalDateTime.now()
        );
        dao.insertRecentSearch(recentSearch);
        return recentSearch;
    }
}