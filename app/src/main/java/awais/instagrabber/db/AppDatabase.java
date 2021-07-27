package awais.instagrabber.db;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;
import android.util.Pair;

import androidx.annotation.NonNull;
import androidx.room.Database;
import androidx.room.Room;
import androidx.room.RoomDatabase;
import androidx.room.TypeConverters;
import androidx.room.migration.Migration;
import androidx.sqlite.db.SupportSQLiteDatabase;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import awais.instagrabber.db.dao.AccountDao;
import awais.instagrabber.db.dao.DMLastNotifiedDao;
import awais.instagrabber.db.dao.FavoriteDao;
import awais.instagrabber.db.dao.RecentSearchDao;
import awais.instagrabber.db.entities.Account;
import awais.instagrabber.db.entities.DMLastNotified;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.db.entities.RecentSearch;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.utils.Utils;

@Database(entities = {Account.class, Favorite.class, DMLastNotified.class, RecentSearch.class},
          version = 6)
@TypeConverters({Converters.class})
public abstract class AppDatabase extends RoomDatabase {
    private static final String TAG = AppDatabase.class.getSimpleName();

    private static AppDatabase INSTANCE;

    public abstract AccountDao accountDao();

    public abstract FavoriteDao favoriteDao();

    public abstract DMLastNotifiedDao dmLastNotifiedDao();

    public abstract RecentSearchDao recentSearchDao();

    public static AppDatabase getDatabase(final Context context) {
        if (INSTANCE == null) {
            synchronized (AppDatabase.class) {
                if (INSTANCE == null) {
                    INSTANCE = Room.databaseBuilder(context.getApplicationContext(), AppDatabase.class, "cookiebox.db")
                                   .addMigrations(MIGRATION_1_2, MIGRATION_2_3, MIGRATION_3_4, MIGRATION_4_5, MIGRATION_5_6)
                                   .build();
                }
            }
        }
        return INSTANCE;
    }

    static final Migration MIGRATION_1_2 = new Migration(1, 2) {
        @Override
        public void migrate(@NonNull SupportSQLiteDatabase db) {
            db.execSQL("ALTER TABLE cookies ADD " + Account.COL_FULL_NAME + " TEXT");
            db.execSQL("ALTER TABLE cookies ADD " + Account.COL_PROFILE_PIC + " TEXT");
        }
    };

    static final Migration MIGRATION_2_3 = new Migration(2, 3) {
        @Override
        public void migrate(@NonNull SupportSQLiteDatabase db) {
            final List<Favorite> oldFavorites = backupOldFavorites(db);
            // recreate with new columns (as there will be no doubt about the `query_display` column being present or not in the future versions)
            db.execSQL("DROP TABLE " + Favorite.TABLE_NAME);
            db.execSQL("CREATE TABLE " + Favorite.TABLE_NAME + " ("
                               + Favorite.COL_ID + " INTEGER PRIMARY KEY,"
                               + Favorite.COL_QUERY + " TEXT,"
                               + Favorite.COL_TYPE + " TEXT,"
                               + Favorite.COL_DISPLAY_NAME + " TEXT,"
                               + Favorite.COL_PIC_URL + " TEXT,"
                               + Favorite.COL_DATE_ADDED + " INTEGER)");
            // add the old favorites back
            for (final Favorite oldFavorite : oldFavorites) {
                insertOrUpdateFavorite(db, oldFavorite);
            }
        }
    };

    static final Migration MIGRATION_3_4 = new Migration(3, 4) {
        @Override
        public void migrate(@NonNull SupportSQLiteDatabase db) {
            // Required when migrating to Room.
            // The original table primary keys were not 'NOT NULL', so the migration to Room were failing without the below migration.
            // Taking this opportunity to rename cookies table to accounts

            // Create new table with name 'accounts'
            db.execSQL("CREATE TABLE " + Account.TABLE_NAME + " ("
                               + Account.COL_ID + " INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                               + Account.COL_UID + " TEXT,"
                               + Account.COL_USERNAME + " TEXT,"
                               + Account.COL_COOKIE + " TEXT,"
                               + Account.COL_FULL_NAME + " TEXT,"
                               + Account.COL_PROFILE_PIC + " TEXT)");
            // Insert all data from table 'cookies' to 'accounts'
            db.execSQL("INSERT INTO " + Account.TABLE_NAME + " ("
                               + Account.COL_UID + ","
                               + Account.COL_USERNAME + ","
                               + Account.COL_COOKIE + ","
                               + Account.COL_FULL_NAME + ","
                               + Account.COL_PROFILE_PIC + ") "
                               + "SELECT "
                               + Account.COL_UID + ","
                               + Account.COL_USERNAME + ","
                               + Account.COL_COOKIE + ","
                               + Account.COL_FULL_NAME + ","
                               + Account.COL_PROFILE_PIC
                               + " FROM cookies");
            // Drop old cookies table
            db.execSQL("DROP TABLE cookies");

            // Create favorite backup table
            db.execSQL("CREATE TABLE " + Favorite.TABLE_NAME + "_backup ("
                               + Favorite.COL_ID + " INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                               + Favorite.COL_QUERY + " TEXT,"
                               + Favorite.COL_TYPE + " TEXT,"
                               + Favorite.COL_DISPLAY_NAME + " TEXT,"
                               + Favorite.COL_PIC_URL + " TEXT,"
                               + Favorite.COL_DATE_ADDED + " INTEGER)");
            // Insert all data from table 'favorite' to 'favorite_backup'
            db.execSQL("INSERT INTO " + Favorite.TABLE_NAME + "_backup ("
                               + Favorite.COL_QUERY + ","
                               + Favorite.COL_TYPE + ","
                               + Favorite.COL_DISPLAY_NAME + ","
                               + Favorite.COL_PIC_URL + ","
                               + Favorite.COL_DATE_ADDED + ") "
                               + "SELECT "
                               + Favorite.COL_QUERY + ","
                               + Favorite.COL_TYPE + ","
                               + Favorite.COL_DISPLAY_NAME + ","
                               + Favorite.COL_PIC_URL + ","
                               + Favorite.COL_DATE_ADDED
                               + " FROM " + Favorite.TABLE_NAME);
            // Drop favorites
            db.execSQL("DROP TABLE " + Favorite.TABLE_NAME);
            // Rename favorite_backup to favorites
            db.execSQL("ALTER TABLE " + Favorite.TABLE_NAME + "_backup RENAME TO " + Favorite.TABLE_NAME);
        }
    };

    static final Migration MIGRATION_4_5 = new Migration(4, 5) {
        @Override
        public void migrate(@NonNull final SupportSQLiteDatabase database) {
            database.execSQL("CREATE TABLE IF NOT EXISTS `dm_last_notified` (" +
                                     "`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, " +
                                     "`thread_id` TEXT, " +
                                     "`last_notified_msg_ts` INTEGER, " +
                                     "`last_notified_at` INTEGER)");
            database.execSQL("CREATE UNIQUE INDEX IF NOT EXISTS `index_dm_last_notified_thread_id` ON `dm_last_notified` (`thread_id`)");
        }
    };

    static final Migration MIGRATION_5_6 = new Migration(5, 6) {
        @Override
        public void migrate(@NonNull final SupportSQLiteDatabase database) {
            database.execSQL("CREATE TABLE IF NOT EXISTS `recent_searches` (" +
                                     "`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, " +
                                     "`ig_id` TEXT NOT NULL, " +
                                     "`name` TEXT NOT NULL, " +
                                     "`username` TEXT, " +
                                     "`pic_url` TEXT, " +
                                     "`type` TEXT NOT NULL, " +
                                     "`last_searched_on` INTEGER NOT NULL)");
            database.execSQL("CREATE UNIQUE INDEX IF NOT EXISTS `index_recent_searches_ig_id_type` ON `recent_searches` (`ig_id`, `type`)");
        }
    };

    @NonNull
    private static List<Favorite> backupOldFavorites(@NonNull final SupportSQLiteDatabase db) {
        // check if old favorites table had the column query_display
        final boolean queryDisplayExists = checkColumnExists(db, Favorite.TABLE_NAME, "query_display");
        Log.d(TAG, "backupOldFavorites: queryDisplayExists: " + queryDisplayExists);
        final List<Favorite> oldModels = new ArrayList<>();
        final String sql = "SELECT "
                + "query_text,"
                + "date_added"
                + (queryDisplayExists ? ",query_display" : "")
                + " FROM " + Favorite.TABLE_NAME;
        try (final Cursor cursor = db.query(sql)) {
            if (cursor != null && cursor.moveToFirst()) {
                do {
                    try {
                        final String queryText = cursor.getString(cursor.getColumnIndex("query_text"));
                        final Pair<FavoriteType, String> favoriteTypeQueryPair = Utils.migrateOldFavQuery(queryText);
                        if (favoriteTypeQueryPair == null) continue;
                        final FavoriteType type = favoriteTypeQueryPair.first;
                        final String query = favoriteTypeQueryPair.second;
                        oldModels.add(new Favorite(
                                0,
                                query,
                                type,
                                queryDisplayExists ? cursor.getString(cursor.getColumnIndex("query_display"))
                                                   : null,
                                null,
                                new Date(cursor.getLong(cursor.getColumnIndex("date_added")))
                        ));
                    } catch (Exception e) {
                        Log.e(TAG, "onUpgrade", e);
                    }
                } while (cursor.moveToNext());
            }
        } catch (Exception e) {
            Log.e(TAG, "onUpgrade", e);
        }
        Log.d(TAG, "backupOldFavorites: oldModels:" + oldModels);
        return oldModels;
    }

    private static synchronized void insertOrUpdateFavorite(@NonNull final SupportSQLiteDatabase db, @NonNull final Favorite model) {
        final ContentValues values = new ContentValues();
        values.put(Favorite.COL_QUERY, model.getQuery());
        values.put(Favorite.COL_TYPE, model.getType().toString());
        values.put(Favorite.COL_DISPLAY_NAME, model.getDisplayName());
        values.put(Favorite.COL_PIC_URL, model.getPicUrl());
        values.put(Favorite.COL_DATE_ADDED, model.getDateAdded().getTime());
        int rows;
        if (model.getId() >= 1) {
            rows = db.update(Favorite.TABLE_NAME,
                             SQLiteDatabase.CONFLICT_IGNORE,
                             values,
                             Favorite.COL_ID + "=?",
                             new String[]{String.valueOf(model.getId())});
        } else {
            rows = db.update(Favorite.TABLE_NAME,
                             SQLiteDatabase.CONFLICT_IGNORE,
                             values,
                             Favorite.COL_QUERY + "=?" + " AND " + Favorite.COL_TYPE + "=?",
                             new String[]{model.getQuery(), model.getType().toString()});
        }
        if (rows != 1) {
            db.insert(Favorite.TABLE_NAME, SQLiteDatabase.CONFLICT_IGNORE, values);
        }
    }

    private static boolean checkColumnExists(@NonNull final SupportSQLiteDatabase db,
                                             @NonNull final String tableName,
                                             @NonNull final String columnName) {
        boolean exists = false;
        try (Cursor cursor = db.query("PRAGMA table_info(" + tableName + ")")) {
            if (cursor.moveToFirst()) {
                do {
                    final String currentColumn = cursor.getString(cursor.getColumnIndex("name"));
                    if (currentColumn.equals(columnName)) {
                        exists = true;
                    }
                } while (cursor.moveToNext());

            }
        } catch (Exception ex) {
            Log.e(TAG, "checkColumnExists", ex);
        }
        return exists;
    }
}
