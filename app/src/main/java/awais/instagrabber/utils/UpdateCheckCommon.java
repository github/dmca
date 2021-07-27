package awais.instagrabber.utils;

import android.content.Context;
import android.content.DialogInterface;

import androidx.annotation.NonNull;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.R;

import static awais.instagrabber.utils.Utils.settingsHelper;

public final class UpdateCheckCommon {

    public static boolean shouldShowUpdateDialog(final boolean force,
                                                 @NonNull final String version) {
        final String skippedVersion = settingsHelper.getString(Constants.SKIPPED_VERSION);
        return force || (!BuildConfig.DEBUG && !skippedVersion.equals(version));
    }

    public static void showUpdateDialog(@NonNull final Context context,
                                        @NonNull final String version,
                                        @NonNull final DialogInterface.OnClickListener onDownloadClickListener) {
        AppExecutors.getInstance().mainThread().execute(() -> {
            new MaterialAlertDialogBuilder(context)
                    .setTitle(context.getString(R.string.update_available, version))
                    .setNeutralButton(R.string.skip_update, (dialog, which) -> {
                        settingsHelper.putString(Constants.SKIPPED_VERSION, version);
                        dialog.dismiss();
                    })
                    .setPositiveButton(R.string.action_download, onDownloadClickListener)
                    .setNegativeButton(R.string.cancel, null)
                    .show();
        });
    }
}
