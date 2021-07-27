/*package awaisomereport;

import android.app.Application;
import android.os.Build;
import android.util.Log;
import android.util.Pair;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;

import awais.instagrabber.BuildConfig;

public final class LogCollector {
    private final File logDir;

    public LogCollector(@NonNull final Application app) {
        logDir = new File(Build.VERSION.SDK_INT >= Build.VERSION_CODES.N ? app.getDataDir() : app.getFilesDir(),
                "crashlogs");

        if (logDir.exists()) deleteRecursive(logDir);

        if (logDir.mkdirs()) {
            // create log files to zip later
            for (final LogFile logFile : LogFile.values()) {
                try {
                    //noinspection ResultOfMethodCallIgnored
                    new File(logDir, logFile.fileName).createNewFile();
                } catch (final IOException e) {
                    if (BuildConfig.DEBUG) Log.e("AWAISKING_APP", "", e);
                }
            }
        }
    }

    public File getLogDir() {
        return logDir;
    }

    @SafeVarargs
    public final void appendException(@NonNull final Exception exception, @NonNull final LogFile logFile, @NonNull final String method,
                                      @Nullable final Pair<String, Object>... vars) {
        final File excepionFile = new File(logDir, logFile.fileName);

        final StringBuilder stringBuilder = new StringBuilder();

        stringBuilder.append('\n').append('\n').append("----------------- ").append(method).append(" ------------------")
                .append('\n');

        if (vars != null && vars.length > 0) {
            stringBuilder.append("Variables: ").append('\n');
            for (Pair<String, Object> var : vars)
                stringBuilder.append('\t').append(var.first).append(" : ")
                        .append('\u201C').append(var.second).append('\u201D')
                        .append(" (type: ").append(var.second == null ? "null" : var.second.getClass().getSimpleName()).append(')')
                        .append('\n');
            stringBuilder.append("----------------------------------").append('\n');
        }

        final Writer stringWriter = new StringWriter();
        try (final PrintWriter printWriter = new PrintWriter(stringWriter)) {
            exception.printStackTrace(printWriter);
            stringBuilder.append(stringWriter.toString());

            // for AsyncTask crashes
            Throwable cause = exception.getCause();
            while (cause != null) {
                cause.printStackTrace(printWriter);
                stringBuilder.append(stringWriter.toString());
                cause = cause.getCause();
            }
        }

        try (final BufferedReader br = new BufferedReader(new FileReader(excepionFile))) {
            String line;
            while ((line = br.readLine()) != null) stringBuilder.append(line).append('\n');
        } catch (final Exception e) {
            if (BuildConfig.DEBUG) Log.e("AWAISKING_APP", "", e);
        }

        stringBuilder.append('\n');

        try (final BufferedWriter bw = new BufferedWriter(new FileWriter(excepionFile))) {
            bw.write(stringBuilder.toString());
        } catch (final Exception e) {
            if (BuildConfig.DEBUG) Log.e("AWAISKING_APP", "", e);
        }
    }

    public enum LogFile {
        UTILS("utils.txt"),
        MAIN_HELPER("main-helper.txt"),
        ////////////////////////
        ACTIVITY_STORY_VIEWER("act-story-viewer.txt"),
        ////////////////////////
        ASYNC_DOWNLOADER("async-download.txt"),
        ASYNC_MAIN_POSTS_FETCHER("async-main-posts-fetcher.txt"),
        ASYNC_DISCOVER_TOPICS_FETCHER("async-discover-topics-fetcher.txt"),
        ASYNC_POST_FETCHER("async-single-post-fetcher.txt"),
        ASYNC_FEED_FETCHER("async-feed-fetcher.txt"),
        ASYNC_HASHTAG_FETCHER("async-hashtag-fetcher.txt"),
        ASYNC_LOCATION_FETCHER("async-location-fetcher.txt"),
        ASYNC_NOTIFICATION_FETCHER("async-notification-fetcher.txt"),
        ASYNC_PROFILE_FETCHER("async-profile-fetcher.txt"),
        ASYNC_PROFILE_PICTURE_FETCHER("async-pfp-fetcher.txt"),
        ASYNC_SAVED_FETCHER("async-saved-fetcher.txt"),
        ASYNC_STORY_STATUS_FETCHER("async-story-status-fetcher.txt"),
        ASYNC_DISCOVER_FETCHER("async-discover-fetcher.txt"),
        ASYNC_COMMENTS_FETCHER("async-comments-fetcher.txt"),
        ASYNC_FOLLOW_FETCHER("async-follow-fetcher.txt"),
        ASYNC_FEED_STORY_FETCHER("async-feed-story-fetcher.txt"),
        ////////////////////////
        ASYNC_DMS("async-dms-inbox-fetcher.txt"),
        ASYNC_DMS_THREAD("async-dms-thread-fetcher.txt"),
        ////////////////////////
        DATA_BOX_FAVORITES("data-box-favs.txt"),
        UTILS_EXPORT("utils-export.txt"),
        UTILS_IMPORT("utils-import.txt"),
        ;
        private final String fileName;

        LogFile(final String fileName) {
            this.fileName = fileName;
        }
    }

    private static void deleteRecursive(@NonNull final File fileOrDirectory) {
        final File[] files;
        if (fileOrDirectory.isDirectory() && (files = fileOrDirectory.listFiles()) != null)
            for (final File child : files) deleteRecursive(child);
        //noinspection ResultOfMethodCallIgnored
        fileOrDirectory.delete();
    }
}*/