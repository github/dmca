package awais.instagrabber.utils;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.net.HttpURLConnection;
import java.net.URL;

public class UpdateChecker {
    private static final Object LOCK = new Object();
    private static final String TAG = UpdateChecker.class.getSimpleName();

    private static UpdateChecker instance;

    public static UpdateChecker getInstance() {
        if (instance == null) {
            synchronized (LOCK) {
                if (instance == null) {
                    instance = new UpdateChecker();
                }
            }
        }
        return instance;
    }

    /**
     * Needs to be called asynchronously
     *
     * @return the latest version from Github
     */
    @Nullable
    public String getLatestVersion() {
        HttpURLConnection conn = null;
        try {
            conn = (HttpURLConnection) new URL("https://github.com/austinhuang0131/barinsta/releases/latest").openConnection();
            conn.setInstanceFollowRedirects(false);
            conn.setUseCaches(false);
            conn.setRequestProperty("User-Agent", "https://Barinsta.AustinHuang.me / mailto:Barinsta@AustinHuang.me");
            conn.connect();
            final int responseCode = conn.getResponseCode();
            if (responseCode == HttpURLConnection.HTTP_MOVED_TEMP) {
                return "v" + conn.getHeaderField("Location").split("/v")[1];
                // return !version.equals(BuildConfig.VERSION_NAME);
            }
        } catch (final Exception e) {
            Log.e(TAG, "", e);
        } finally {
            if (conn != null) {
                conn.disconnect();
            }
        }
        return null;
    }

    public void onDownload(@NonNull final Context context) {
        Utils.openURL(context, "https://github.com/austinhuang0131/instagrabber/releases/latest");
    }
}
