package awais.instagrabber.utils;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Build;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatDelegate;

import awais.instagrabber.R;

import static awais.instagrabber.utils.Utils.settingsHelper;

public final class ThemeUtils {
    private static final String TAG = "ThemeUtils";

    public static void changeTheme(@NonNull final Context context) {
        int themeCode = AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM; // this is fallback / default

        if (settingsHelper != null) themeCode = settingsHelper.getThemeCode(false);

        if (themeCode == AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM && Build.VERSION.SDK_INT < 29) {
            themeCode = AppCompatDelegate.MODE_NIGHT_AUTO_BATTERY;
        }
        final boolean isNight = isNight(context, themeCode);
        final String themeResName = isNight ? settingsHelper.getString(Constants.PREF_DARK_THEME)
                                            : settingsHelper.getString(Constants.PREF_LIGHT_THEME);
        final int themeResId = context.getResources().getIdentifier(themeResName, "style", context.getPackageName());
        final int finalThemeResId;
        if (themeResId <= 0) {
            // Nothing set in settings
            finalThemeResId = isNight ? R.style.AppTheme_Dark_Black
                                      : R.style.AppTheme_Light_White;
        } else finalThemeResId = themeResId;
        // Log.d(TAG, "changeTheme: finalThemeResId: " + finalThemeResId);
        context.setTheme(finalThemeResId);
    }

    public static boolean isNight(final Context context, final int themeCode) {
        // check if setting is set to 'Dark'
        boolean isNight = themeCode == AppCompatDelegate.MODE_NIGHT_YES;
        // if not dark check if themeCode is MODE_NIGHT_FOLLOW_SYSTEM or MODE_NIGHT_AUTO_BATTERY
        if (!isNight && (themeCode == AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM || themeCode == AppCompatDelegate.MODE_NIGHT_AUTO_BATTERY)) {
            // check if resulting theme would be NIGHT
            final int uiMode = context.getResources().getConfiguration().uiMode & Configuration.UI_MODE_NIGHT_MASK;
            isNight = uiMode == Configuration.UI_MODE_NIGHT_YES;
        }
        return isNight;
    }
}
