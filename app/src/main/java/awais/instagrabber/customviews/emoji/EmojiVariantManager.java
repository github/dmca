package awais.instagrabber.customviews.emoji;

import android.util.Log;

import androidx.annotation.Nullable;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.utils.Constants.PREF_EMOJI_VARIANTS;

public class EmojiVariantManager {
    private static final String TAG = EmojiVariantManager.class.getSimpleName();
    private static final Object LOCK = new Object();

    private final AppExecutors appExecutors = AppExecutors.getInstance();
    private final Map<String, String> selectedVariantMap = new HashMap<>();

    private static EmojiVariantManager instance;

    public static EmojiVariantManager getInstance() {
        if (instance == null) {
            synchronized (LOCK) {
                if (instance == null) {
                    instance = new EmojiVariantManager();
                }
            }
        }
        return instance;
    }

    private EmojiVariantManager() {
        final String variantsJson = Utils.settingsHelper.getString(PREF_EMOJI_VARIANTS);
        if (TextUtils.isEmpty(variantsJson)) return;
        try {
            final JSONObject variantsJSONObject = new JSONObject(variantsJson);
            final Iterator<String> keys = variantsJSONObject.keys();
            keys.forEachRemaining(s -> selectedVariantMap.put(s, variantsJSONObject.optString(s)));
        } catch (JSONException e) {
            Log.e(TAG, "EmojiVariantManager: ", e);
        }
    }

    @Nullable
    public String getVariant(final String parentUnicode) {
        return selectedVariantMap.get(parentUnicode);
    }

    public void setVariant(final String parent, final String variant) {
        if (parent == null || variant == null) return;
        selectedVariantMap.put(parent, variant);
        appExecutors.tasksThread().execute(() -> {
            final JSONObject jsonObject = new JSONObject(selectedVariantMap);
            final String json = jsonObject.toString();
            Utils.settingsHelper.putString(PREF_EMOJI_VARIANTS, json);
        });
    }
}
