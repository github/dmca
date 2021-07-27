package awaisomereport;

import android.app.Application;

import androidx.annotation.NonNull;

public class CrashHandler implements ICrashHandler {
    private static final String TAG = CrashHandler.class.getSimpleName();

    private final Application application;

    public CrashHandler(@NonNull final Application application) {
        this.application = application;
    }

    @Override
    public void uncaughtException(@NonNull final Thread t,
                                  @NonNull final Throwable exception,
                                  @NonNull final Thread.UncaughtExceptionHandler defaultEH) {
        CrashReporterHelper.startErrorReporterActivity(application, exception);
        // zipLogs();
        defaultEH.uncaughtException(t, exception);
    }

    //    public synchronized CrashReporter zipLogs() {
    //        final File logDir = Utils.logCollector != null ? Utils.logCollector.getLogDir() :
    //                new File(Build.VERSION.SDK_INT >= Build.VERSION_CODES.N ? application.getDataDir() : application.getFilesDir(), "crashlogs");
    //
    //        try (final FileOutputStream fos = new FileOutputStream(crashLogsZip);
    //             final ZipOutputStream zos = new ZipOutputStream(fos)) {
    //
    //            final File[] files = logDir.listFiles();
    //
    //            if (files != null) {
    //                zos.setLevel(5);
    //                byte[] buffer;
    //                for (final File file : files) {
    //                    if (file != null && file.length() > 0) {
    //                        buffer = new byte[1024];
    //                        try (final FileInputStream fis = new FileInputStream(file)) {
    //                            zos.putNextEntry(new ZipEntry(file.getName()));
    //                            int length;
    //                            while ((length = fis.read(buffer)) > 0) zos.write(buffer, 0, length);
    //                            zos.closeEntry();
    //                        }
    //                    }
    //                }
    //            }
    //
    //        } catch (final Exception e) {
    //            if (BuildConfig.DEBUG) Log.e("AWAISKING_APP", "", e);
    //        }
    //
    //        return this;
    //    }
}
