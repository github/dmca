package awais.instagrabber.utils;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import org.json.JSONObject;

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
     * @return the latest version from f-droid
     */
    @Nullable
    public String getLatestVersion() {
        HttpURLConnection conn = null;
        try {
            conn = (HttpURLConnection) new URL("https://f-droid.org/api/v1/packages/me.austinhuang.instagrabber").openConnection();
            conn.setUseCaches(false);
            conn.setRequestProperty("User-Agent", "https://Barinsta.AustinHuang.me / mailto:Barinsta@AustinHuang.me");
            conn.connect();
            final int responseCode = conn.getResponseCode();
            if (responseCode == HttpURLConnection.HTTP_OK) {
                final JSONObject data = new JSONObject(NetworkUtils.readFromConnection(conn));
                return "v" + data.getJSONArray("packages").getJSONObject(0).getString("versionName");
                // if (BuildConfig.VERSION_CODE < data.getInt("suggestedVersionCode")) {
                // }
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

    public void onDownload(@NonNull final AppCompatActivity context) {
        Utils.openURL(context, "https://f-droid.org/packages/me.austinhuang.instagrabber/");
    }
}
