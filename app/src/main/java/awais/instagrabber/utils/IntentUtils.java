package awais.instagrabber.utils;

import android.net.Uri;
import android.text.TextUtils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.List;

import awais.instagrabber.models.IntentModel;
import awais.instagrabber.models.enums.IntentModelType;

public final class IntentUtils {

    @Nullable
    public static IntentModel parseUrl(@NonNull final String url) {
        final Uri parsedUrl = Uri.parse(url).normalizeScheme();

        // final String domain = parsedUrl.getHost().replaceFirst("^www\\.", "");
        // final boolean isHttpsUri = "https".equals(parsedUrl.getScheme());

        final List<String> paths = parsedUrl.getPathSegments();

        if (paths.isEmpty()) {
            return null;
        }

        String path = paths.get(0);
        String text = null;
        IntentModelType type = IntentModelType.UNKNOWN;
        if (1 == paths.size()) {
            text = path;
            type = IntentModelType.USERNAME;
        } else if ("_u".equals(path)) {
            text = paths.get(1);
            type = IntentModelType.USERNAME;
        } else if ("p".equals(path) || "reel".equals(path) || "tv".equals(path)) {
            text = paths.get(1);
            type = IntentModelType.POST;
        } else if (2 < paths.size() && "explore".equals(path)) {
            path = paths.get(1);

            if ("locations".equals(path)) {
                text = paths.get(2);
                type = IntentModelType.LOCATION;
            }

            if ("tags".equals(path)) {
                text = paths.get(2);
                type = IntentModelType.HASHTAG;
            }
        }

        if (TextUtils.isEmpty(text)) {
            return null;
        }

        return new IntentModel(type, text);
    }
}
