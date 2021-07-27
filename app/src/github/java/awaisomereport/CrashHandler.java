package awaisomereport;

import android.app.Application;

import androidx.annotation.NonNull;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import io.sentry.SentryLevel;
import io.sentry.android.core.SentryAndroid;
import io.sentry.protocol.Contexts;
import io.sentry.protocol.Device;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class CrashHandler implements ICrashHandler {
    private static final String TAG = CrashHandler.class.getSimpleName();

    private final Application application;
    private final boolean enabled;

    public CrashHandler(@NonNull final Application application) {
        this.application = application;
        if (!settingsHelper.hasPreference(PreferenceKeys.PREF_ENABLE_SENTRY)) {
            // disabled by default (change to true if we need enabled by default)
            enabled = false;
        } else {
            enabled = settingsHelper.getBoolean(PreferenceKeys.PREF_ENABLE_SENTRY);
        }
        if (!enabled) return;
        SentryAndroid.init(application, options -> {
            options.setDsn(BuildConfig.dsn);
            options.setDiagnosticLevel(SentryLevel.ERROR);
            options.setBeforeSend((event, hint) -> {
                // Removing unneeded info from event
                final Contexts contexts = event.getContexts();
                final Device device = contexts.getDevice();
                device.setName(null);
                device.setTimezone(null);
                device.setCharging(null);
                device.setBootTime(null);
                device.setFreeStorage(null);
                device.setBatteryTemperature(null);
                return event;
            });
        });
    }

    @Override
    public void uncaughtException(@NonNull final Thread t,
                                  @NonNull final Throwable exception,
                                  @NonNull final Thread.UncaughtExceptionHandler defaultEH) {
        // When enabled, Sentry auto captures unhandled exceptions
        if (!enabled) {
            CrashReporterHelper.startErrorReporterActivity(application, exception);
        }
        defaultEH.uncaughtException(t, exception);
    }
}
