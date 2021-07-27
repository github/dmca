package awais.instagrabber.utils;

import android.content.Context;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.view.ContextThemeWrapper;

import androidx.annotation.Nullable;

import java.util.Locale;

import awais.instagrabber.fragments.settings.PreferenceKeys;

// taken from my app TESV Console Codes
public final class LocaleUtils {
    private static Locale defaultLocale, currentLocale;

    public static void setLocale(Context baseContext) {
        if (defaultLocale == null) defaultLocale = Locale.getDefault();

        if (baseContext instanceof ContextThemeWrapper)
            baseContext = ((ContextThemeWrapper) baseContext).getBaseContext();

        if (Utils.settingsHelper == null)
            Utils.settingsHelper = new SettingsHelper(baseContext);

        final String appLanguageSettings = Utils.settingsHelper.getString(PreferenceKeys.APP_LANGUAGE);
        final String lang = LocaleUtils.getCorrespondingLanguageCode(appLanguageSettings);

        currentLocale = TextUtils.isEmpty(lang) ? defaultLocale :
                        (lang.contains("_") ? new Locale(lang.split("_")[0], lang.split("_")[1]) : new Locale(lang));
        Locale.setDefault(currentLocale);

        final Resources res = baseContext.getResources();
        final Configuration config = res.getConfiguration();

        config.locale = currentLocale;
        config.setLocale(currentLocale);
        config.setLayoutDirection(currentLocale);

        res.updateConfiguration(config, res.getDisplayMetrics());
    }

    public static Locale getCurrentLocale() {
        return currentLocale;
    }

    public static void updateConfig(final ContextThemeWrapper wrapper) {
        if (currentLocale != null) {
            final Configuration configuration = new Configuration();
            configuration.locale = currentLocale;
            configuration.setLocale(currentLocale);
            wrapper.applyOverrideConfiguration(configuration);
        }
    }

    @Nullable
    public static String getCorrespondingLanguageCode(final String appLanguageSettings) {
        if (TextUtils.isEmpty(appLanguageSettings)) return null;

        final int appLanguageIndex = Integer.parseInt(appLanguageSettings);
        switch (appLanguageIndex) {
            case 1: return "en";
            case 2: return "fr";
            case 3: return "es";
            case 4: return "zh_CN";
            case 5: return "in";
            case 6: return "it";
            case 7: return "de";
            case 8: return "pl";
            case 9: return "tr";
            case 10: return "pt";
            case 11: return "fa";
            case 12: return "mk";
            case 13: return "vi";
            case 14: return "zh_TW";
            case 15: return "ca";
            case 16: return "ru";
            case 17: return "hi";
            case 18: return "nl";
            case 19: return "sk";
            case 20: return "ja";
            case 21: return "el";
            case 22: return "eu";
            case 23: return "sv";
            case 24: return "ko";
        }

        return null;
    }
}
