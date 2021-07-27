package awais.instagrabber;

import android.app.Application;
import android.content.ClipboardManager;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.imagepipeline.core.ImagePipelineConfig;

import java.net.CookieHandler;
import java.text.SimpleDateFormat;
import java.util.UUID;

import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.LocaleUtils;
import awais.instagrabber.utils.SettingsHelper;
import awais.instagrabber.utils.TextUtils;
import awaisomereport.CrashReporter;

import static awais.instagrabber.utils.CookieUtils.NET_COOKIE_MANAGER;
import static awais.instagrabber.utils.Utils.applicationHandler;
import static awais.instagrabber.utils.Utils.cacheDir;
import static awais.instagrabber.utils.Utils.clipboardManager;
import static awais.instagrabber.utils.Utils.datetimeParser;
import static awais.instagrabber.utils.Utils.settingsHelper;

//import awaisomereport.LogCollector;
//import static awais.instagrabber.utils.Utils.logCollector;

public final class InstaGrabberApplication extends Application {
    private static final String TAG = "InstaGrabberApplication";

    @Override
    public void onCreate() {
        super.onCreate();
        CookieHandler.setDefault(NET_COOKIE_MANAGER);

        if (settingsHelper == null) {
            settingsHelper = new SettingsHelper(this);
        }

        if (!BuildConfig.DEBUG) {
            CrashReporter.get(this).start();
        }
        // logCollector = new LogCollector(this);

        if (BuildConfig.DEBUG) {
            try {
                Class.forName("dalvik.system.CloseGuard")
                     .getMethod("setEnabled", boolean.class)
                     .invoke(null, true);
            } catch (Exception e) {
                Log.e(TAG, "Error", e);
            }
        }
      
        // final Set<RequestListener> requestListeners = new HashSet<>();
        // requestListeners.add(new RequestLoggingListener());
        final ImagePipelineConfig imagePipelineConfig = ImagePipelineConfig
                .newBuilder(this)
                // .setMainDiskCacheConfig(diskCacheConfig)
                // .setRequestListeners(requestListeners)
                .setDownsampleEnabled(true)
                .build();
        Fresco.initialize(this, imagePipelineConfig);
        // FLog.setMinimumLoggingLevel(FLog.VERBOSE);

        if (applicationHandler == null) {
            applicationHandler = new Handler(getApplicationContext().getMainLooper());
        }

        if (cacheDir == null) {
            cacheDir = getCacheDir().getAbsolutePath();
        }

        LocaleUtils.setLocale(getBaseContext());

        if (clipboardManager == null)
            clipboardManager = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);

        if (datetimeParser == null)
            datetimeParser = new SimpleDateFormat(
                    settingsHelper.getBoolean(PreferenceKeys.CUSTOM_DATE_TIME_FORMAT_ENABLED) ?
                    settingsHelper.getString(PreferenceKeys.CUSTOM_DATE_TIME_FORMAT) :
                    settingsHelper.getString(PreferenceKeys.DATE_TIME_FORMAT), LocaleUtils.getCurrentLocale());

        if (TextUtils.isEmpty(settingsHelper.getString(Constants.DEVICE_UUID))) {
            settingsHelper.putString(Constants.DEVICE_UUID, UUID.randomUUID().toString());
        }
    }
}