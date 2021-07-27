package awais.instagrabber.utils;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public final class DataBox extends SQLiteOpenHelper {
    private static final String TAG = "DataBox";

    private static DataBox sInstance;

    private final static int VERSION = 3;

    public static synchronized DataBox getInstance(final Context context) {
        if (sInstance == null) sInstance = new DataBox(context.getApplicationContext());
        return sInstance;
    }

    private DataBox(@Nullable final Context context) {
        super(context, "cookiebox.db", null, VERSION);
    }

    @Override
    public void onCreate(@NonNull final SQLiteDatabase db) {
        Log.i(TAG, "Creating tables...");
        Log.i(TAG, "Tables created!");
    }

    @Override
    public void onUpgrade(final SQLiteDatabase db, final int oldVersion, final int newVersion) {
        Log.i(TAG, String.format("Updating DB from v%d to v%d", oldVersion, newVersion));
        // switch without break, so that all migrations from a previous version to new are run
        switch (oldVersion) {
            case 1:
            case 2:
        }
        Log.i(TAG, String.format("DB update from v%d to v%d completed!", oldVersion, newVersion));
    }
}