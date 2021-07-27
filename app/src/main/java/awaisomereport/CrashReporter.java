package awaisomereport;

import android.app.Application;

import androidx.annotation.NonNull;

public final class CrashReporter implements Thread.UncaughtExceptionHandler {
    private static final String TAG = CrashReporter.class.getSimpleName();

    private static CrashReporter reporterInstance;

    // private final File crashLogsZip;
    private final CrashHandler crashHandler;

    private boolean startAttempted = false;
    private Thread.UncaughtExceptionHandler defaultEH;

    public static CrashReporter get(final Application application) {
        if (reporterInstance == null) {
            reporterInstance = new CrashReporter(application);
        }
        return reporterInstance;
    }

    private CrashReporter(@NonNull final Application application) {
        crashHandler = new CrashHandler(application);
        // this.crashLogsZip = new File(application.getExternalCacheDir(), "crash_logs.zip");
    }

    public void start() {
        if (!startAttempted) {
            defaultEH = Thread.getDefaultUncaughtExceptionHandler();
            Thread.setDefaultUncaughtExceptionHandler(this);
            startAttempted = true;
        }
    }

    @Override
    public void uncaughtException(@NonNull final Thread t, @NonNull final Throwable exception) {
        if (crashHandler == null) {
            defaultEH.uncaughtException(t, exception);
            return;
        }
        crashHandler.uncaughtException(t, exception, defaultEH);
    }
}
