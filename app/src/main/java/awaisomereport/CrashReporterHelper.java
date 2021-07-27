package awaisomereport;

import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Build;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;
import java.util.Date;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.R;
import awais.instagrabber.utils.Constants;

public final class CrashReporterHelper {
    private static final String TAG = CrashReporterHelper.class.getSimpleName();

    public static void startErrorReporterActivity(@NonNull final Application application,
                                                  @NonNull final Throwable exception) {
        final StringBuilder reportBuilder = new StringBuilder();
        reportBuilder.append("IMPORTANT: If sending by email, your email address and the entire content will be made public on GitHub issues.")
                     .append("\r\nIMPORTANT: When possible, please describe the steps leading to this crash. Thank you for your cooperation.")
                     .append("\r\n\r\nError report collected on: ").append(new Date().toString())
                     .append("\r\n\r\nInformation:\r\n==============")
                     .append("\r\nVERSION		: ").append(BuildConfig.VERSION_NAME)
                     .append("\r\nVERSION_CODE	: ").append(BuildConfig.VERSION_CODE)
                     .append("\r\nPHONE-MODEL	: ").append(Build.MODEL)
                     .append("\r\nANDROID_VERS	: ").append(Build.VERSION.RELEASE)
                     .append("\r\nANDROID_REL	: ").append(Build.VERSION.SDK_INT)
                     .append("\r\nBRAND			: ").append(Build.BRAND)
                     .append("\r\nMANUFACTURER	: ").append(Build.MANUFACTURER)
                     .append("\r\nBOARD			: ").append(Build.BOARD)
                     .append("\r\nDEVICE			: ").append(Build.DEVICE)
                     .append("\r\nPRODUCT		: ").append(Build.PRODUCT)
                     .append("\r\nHOST			: ").append(Build.HOST)
                     .append("\r\nTAGS			: ").append(Build.TAGS);

        reportBuilder.append("\r\n\r\nStack:\r\n==============\r\n");
        final Writer result = new StringWriter();
        try (final PrintWriter printWriter = new PrintWriter(result)) {
            exception.printStackTrace(printWriter);
            reportBuilder.append(result.toString());
            reportBuilder.append("\r\nCause:\r\n==============");
            // for AsyncTask crashes
            Throwable cause = exception.getCause();
            while (cause != null) {
                cause.printStackTrace(printWriter);
                reportBuilder.append(result.toString());
                cause = cause.getCause();
            }
        }
        reportBuilder.append("\r\n\r\n**** End of current Report ***");

        final String errorContent = reportBuilder.toString();
        try (final FileOutputStream trace = application.openFileOutput("stack-" + System.currentTimeMillis() + ".stacktrace", Context.MODE_PRIVATE)) {
            trace.write(errorContent.getBytes());
        } catch (final Exception ex) {
            if (BuildConfig.DEBUG) Log.e(TAG, "", ex);
        }

        application.startActivity(new Intent(application, ErrorReporterActivity.class).setFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
    }

    public static void startCrashEmailIntent(final Context context) {
        try {
            final String filePath = context.getFilesDir().getAbsolutePath();

            String[] errorFileList;

            try {
                final File dir = new File(filePath);
                if (dir.exists() && !dir.isDirectory()) {
                    //noinspection ResultOfMethodCallIgnored
                    dir.delete();
                }
                //noinspection ResultOfMethodCallIgnored
                dir.mkdirs();
                errorFileList = dir.list((d, name) -> name.endsWith(".stacktrace"));
            } catch (final Exception e) {
                errorFileList = null;
            }

            if (errorFileList == null || errorFileList.length <= 0) {
                return;
            }
            final StringBuilder errorStringBuilder;

            errorStringBuilder = new StringBuilder("\r\n\r\n");
            final int maxSendMail = 5;
            int curIndex = 0;
            for (final String curString : errorFileList) {
                final File file = new File(filePath + '/' + curString);

                if (curIndex++ <= maxSendMail) {
                    errorStringBuilder.append("New Trace collected:\r\n=====================\r\n");
                    try (final BufferedReader input = new BufferedReader(new FileReader(file))) {
                        String line;
                        while ((line = input.readLine()) != null)
                            errorStringBuilder.append(line).append("\r\n");
                    }
                }
                //noinspection ResultOfMethodCallIgnored
                file.delete();
            }

            errorStringBuilder.append("\r\n\r\n");
            final Resources resources = context.getResources();
            context.startActivity(Intent.createChooser(
                    new Intent(Intent.ACTION_SEND)
                            .setType("message/rfc822")
                            .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
                                              | Intent.FLAG_GRANT_READ_URI_PERMISSION
                                              | Intent.FLAG_GRANT_WRITE_URI_PERMISSION)
                            .putExtra(Intent.EXTRA_EMAIL, new String[]{Constants.CRASH_REPORT_EMAIL})
                            // .putExtra(Intent.EXTRA_STREAM, FileProvider.getUriForFile(application, BuildConfig.APPLICATION_ID + ".provider", crashLogsZip))
                            .putExtra(Intent.EXTRA_SUBJECT, resources.getString(R.string.crash_report_subject))
                            .putExtra(Intent.EXTRA_TEXT, errorStringBuilder.toString()),
                    context.getResources().getString(R.string.crash_report_title))
            );
        } catch (final Exception e) {
            Log.e(TAG, "", e);
        }
    }
}
