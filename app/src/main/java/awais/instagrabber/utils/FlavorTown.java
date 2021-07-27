package awais.instagrabber.utils;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import java.util.Objects;
import java.util.concurrent.ThreadLocalRandom;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.R;

import static awais.instagrabber.utils.Utils.settingsHelper;

public final class FlavorTown {
    private static final String TAG = "FlavorTown";
    private static final UpdateChecker UPDATE_CHECKER = UpdateChecker.getInstance();
    private static final Pattern VERSION_NAME_PATTERN = Pattern.compile("v?(\\d+\\.\\d+\\.\\d+)(?:_?)(\\w*)(?:-?)(\\w*)");

    private static boolean checking = false;

    public static void updateCheck(@NonNull final AppCompatActivity context) {
        updateCheck(context, false);
    }

    public static void updateCheck(@NonNull final AppCompatActivity context,
                                   final boolean force) {
        if (checking) return;
        checking = true;
        AppExecutors.getInstance().networkIO().execute(() -> {
            final String onlineVersionName = UPDATE_CHECKER.getLatestVersion();
            if (onlineVersionName == null) return;
            final String onlineVersion = getVersion(onlineVersionName);
            final String localVersion = getVersion(BuildConfig.VERSION_NAME);
            if (Objects.equals(onlineVersion, localVersion)) {
                if (force) {
                    AppExecutors.getInstance().mainThread().execute(() -> {
                        final Context applicationContext = context.getApplicationContext();
                        // Check if app was closed or crashed before reaching here
                        if (applicationContext == null) return;
                        // Show toast if version number preference was tapped
                        Toast.makeText(applicationContext, R.string.on_latest_version, Toast.LENGTH_SHORT).show();
                    });
                }
                return;
            }
            final boolean shouldShowDialog = UpdateCheckCommon.shouldShowUpdateDialog(force, onlineVersionName);
            if (!shouldShowDialog) return;
            UpdateCheckCommon.showUpdateDialog(context, onlineVersionName, (dialog, which) -> {
                UPDATE_CHECKER.onDownload(context);
                dialog.dismiss();
            });
        });
    }

    private static String getVersion(@NonNull final String versionName) {
        final Matcher matcher = VERSION_NAME_PATTERN.matcher(versionName);
        if (!matcher.matches()) return versionName;
        try {
            return matcher.group(1);
        } catch (Exception e) {
            Log.e(TAG, "getVersion: ", e);
        }
        return versionName;
    }

    public static void changelogCheck(@NonNull final Context context) {
        if (settingsHelper.getInteger(Constants.PREV_INSTALL_VERSION) < BuildConfig.VERSION_CODE) {
            int appUaCode = settingsHelper.getInteger(Constants.APP_UA_CODE);
            int browserUaCode = settingsHelper.getInteger(Constants.BROWSER_UA_CODE);
            if (browserUaCode == -1 || browserUaCode >= UserAgentUtils.browsers.length) {
                browserUaCode = ThreadLocalRandom.current().nextInt(0, UserAgentUtils.browsers.length);
                settingsHelper.putInteger(Constants.BROWSER_UA_CODE, browserUaCode);
            }
            if (appUaCode == -1 || appUaCode >= UserAgentUtils.devices.length) {
                appUaCode = ThreadLocalRandom.current().nextInt(0, UserAgentUtils.devices.length);
                settingsHelper.putInteger(Constants.APP_UA_CODE, appUaCode);
            }
            final String appUa = UserAgentUtils.generateAppUA(appUaCode, LocaleUtils.getCurrentLocale().getLanguage());
            settingsHelper.putString(Constants.APP_UA, appUa);
            final String browserUa = UserAgentUtils.generateBrowserUA(browserUaCode);
            settingsHelper.putString(Constants.BROWSER_UA, browserUa);
            Toast.makeText(context, R.string.updated, Toast.LENGTH_SHORT).show();
            settingsHelper.putInteger(Constants.PREV_INSTALL_VERSION, BuildConfig.VERSION_CODE);
        }
    }
}