package awais.instagrabber.utils;

import android.content.Context;
import android.os.Build;
import android.os.Environment;

import java.io.File;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.regex.Pattern;

import awais.instagrabber.R;

public class DirectoryUtils {
    private static final Pattern DIR_SEPORATOR = Pattern.compile("/");

    /**
     * From: https://stackoverflow.com/a/18871043/1436766
     * <p>
     * Returns all available SD-Cards in the system (include emulated)
     * <p>
     * Warning: Hack! Based on Android source code of version 4.3 (API 18)
     * Because there is no standard way to get it.
     * TODO: Test on future Android versions 4.4+
     *
     * @return paths to all available SD-Cards in the system (include emulated)
     */
    public static Set<String> getStorageDirectories() {
        // Final set of paths
        final Set<String> rv = new HashSet<>();
        // Primary physical SD-CARD (not emulated)
        final String rawExternalStorage = System.getenv("EXTERNAL_STORAGE");
        // All Secondary SD-CARDs (all exclude primary) separated by ":"
        final String rawSecondaryStoragesStr = System.getenv("SECONDARY_STORAGE");
        // Primary emulated SD-CARD
        final String rawEmulatedStorageTarget = System.getenv("EMULATED_STORAGE_TARGET");
        if (TextUtils.isEmpty(rawEmulatedStorageTarget)) {
            // Device has physical external storage; use plain paths.
            if (TextUtils.isEmpty(rawExternalStorage)) {
                // EXTERNAL_STORAGE undefined; falling back to default.
                rv.add("/storage/sdcard0");
            } else {
                rv.add(rawExternalStorage);
            }
        } else {
            // Device has emulated storage; external storage paths should have
            // userId burned into them.
            final String rawUserId;
            final String path = Environment.getExternalStorageDirectory().getAbsolutePath();
            final String[] folders = DIR_SEPORATOR.split(path);
            final String lastFolder = folders[folders.length - 1];
            boolean isDigit = false;
            try {
                Integer.valueOf(lastFolder);
                isDigit = true;
            } catch (NumberFormatException ignored) {
            }
            rawUserId = isDigit ? lastFolder : "";
            // /storage/emulated/0[1,2,...]
            if (TextUtils.isEmpty(rawUserId)) {
                rv.add(rawEmulatedStorageTarget);
            } else {
                rv.add(rawEmulatedStorageTarget + File.separator + rawUserId);
            }
        }
        // Add all secondary storages
        if (!TextUtils.isEmpty(rawSecondaryStoragesStr)) {
            // All Secondary SD-CARDs splited into array
            final String[] rawSecondaryStorages = rawSecondaryStoragesStr.split(File.pathSeparator);
            Collections.addAll(rv, rawSecondaryStorages);
        }
        return rv;
    }

    public static File getOutputMediaDirectory(final Context context, final String... dirs) {
        if (context == null) return null;
        final File[] externalMediaDirs = context.getExternalMediaDirs();
        if (externalMediaDirs == null || externalMediaDirs.length == 0) return context.getFilesDir();
        final File externalMediaDir = externalMediaDirs[0];
        File subDir = new File(externalMediaDir, context.getString(R.string.app_name));
        if (dirs != null) {
            for (final String dir : dirs) {
                subDir = new File(subDir, dir);
                //noinspection ResultOfMethodCallIgnored
                subDir.mkdirs();
            }
        }
        if (!subDir.exists()) {
            return context.getFilesDir();
        }
        return subDir;
    }
}
