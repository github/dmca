package awais.instagrabber.db;

import androidx.room.Room;
import androidx.room.migration.Migration;
import androidx.room.testing.MigrationTestHelper;
import androidx.sqlite.db.SupportSQLiteDatabase;
import androidx.sqlite.db.framework.FrameworkSQLiteOpenHelperFactory;
import androidx.test.platform.app.InstrumentationRegistry;
import androidx.test.runner.AndroidJUnit4;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;

import static awais.instagrabber.db.AppDatabase.MIGRATION_4_5;
import static awais.instagrabber.db.AppDatabase.MIGRATION_5_6;

@RunWith(AndroidJUnit4.class)
public class MigrationTest {
    private static final String TEST_DB = "migration-test";
    private static final Migration[] ALL_MIGRATIONS = new Migration[]{MIGRATION_4_5, MIGRATION_5_6};

    @Rule
    public MigrationTestHelper helper;

    public MigrationTest() {
        final String canonicalName = AppDatabase.class.getCanonicalName();
        assert canonicalName != null;
        helper = new MigrationTestHelper(InstrumentationRegistry.getInstrumentation(),
                                         canonicalName,
                                         new FrameworkSQLiteOpenHelperFactory());
    }

    @Test
    public void migrateAll() throws IOException {
        // Create earliest version of the database. Have to start with 4 since that is the version we migrated to Room.
        SupportSQLiteDatabase db = helper.createDatabase(TEST_DB, 4);
        db.close();

        // Open latest version of the database. Room will validate the schema
        // once all migrations execute.
        AppDatabase appDb = Room.databaseBuilder(InstrumentationRegistry.getInstrumentation().getTargetContext(),
                                                 AppDatabase.class,
                                                 TEST_DB)
                                .addMigrations(ALL_MIGRATIONS).build();
        appDb.getOpenHelper().getWritableDatabase();
        appDb.close();
    }
}
