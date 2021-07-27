package awais.instagrabber.services;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;

import java.time.Duration;
import java.time.temporal.ChronoUnit;
import java.time.temporal.TemporalUnit;

import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.utils.Constants;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class DMSyncAlarmReceiver extends BroadcastReceiver {
    private static final String TAG = DMSyncAlarmReceiver.class.getSimpleName();

    @Override
    public void onReceive(final Context context, final Intent intent) {
        final boolean enabled = settingsHelper.getBoolean(PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH);
        if (!enabled) {
            // If somehow the alarm was triggered even when auto refresh is disabled
            cancelAlarm(context);
            return;
        }
        try {
            final Context applicationContext = context.getApplicationContext();
            ContextCompat.startForegroundService(applicationContext, new Intent(applicationContext, DMSyncService.class));
        } catch (Exception e) {
            Log.e(TAG, "onReceive: ", e);
        }
    }

    public static void setAlarm(@NonNull final Context context) {
        Log.d(TAG, "setting DMSyncService Alarm");
        final AlarmManager alarmManager = getAlarmManager(context);
        if (alarmManager == null) return;
        final PendingIntent pendingIntent = getPendingIntent(context);
        alarmManager.setInexactRepeating(AlarmManager.RTC, System.currentTimeMillis(), getIntervalMillis(), pendingIntent);
    }

    public static void cancelAlarm(@NonNull final Context context) {
        Log.d(TAG, "cancelling DMSyncService Alarm");
        final AlarmManager alarmManager = getAlarmManager(context);
        if (alarmManager == null) return;
        final PendingIntent pendingIntent = getPendingIntent(context);
        alarmManager.cancel(pendingIntent);
    }

    private static AlarmManager getAlarmManager(@NonNull final Context context) {
        return (AlarmManager) context.getApplicationContext().getSystemService(Context.ALARM_SERVICE);
    }

    private static PendingIntent getPendingIntent(@NonNull final Context context) {
        final Context applicationContext = context.getApplicationContext();
        final Intent intent = new Intent(applicationContext, DMSyncAlarmReceiver.class);
        return PendingIntent.getBroadcast(applicationContext,
                                          Constants.DM_SYNC_SERVICE_REQUEST_CODE,
                                          intent,
                                          PendingIntent.FLAG_UPDATE_CURRENT);
    }

    private static long getIntervalMillis() {
        int amount = settingsHelper.getInteger(PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH_FREQ_NUMBER);
        if (amount <= 0) {
            amount = 30;
        }
        final String unit = settingsHelper.getString(PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH_FREQ_UNIT);
        final TemporalUnit temporalUnit;
        switch (unit) {
            case "mins":
                temporalUnit = ChronoUnit.MINUTES;
                break;
            default:
            case "secs":
                temporalUnit = ChronoUnit.SECONDS;
        }
        return Duration.of(amount, temporalUnit).toMillis();
    }
}