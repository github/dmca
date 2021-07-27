package awais.instagrabber.fragments.settings;

import android.content.Context;
import android.content.res.TypedArray;

import androidx.annotation.NonNull;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;

import awais.instagrabber.R;
import awais.instagrabber.utils.Constants;

public class ThemePreferencesFragment extends BasePreferencesFragment {
    @Override
    void setupPreferenceScreen(final PreferenceScreen screen) {
        final Context context = getContext();
        if (context == null) return;
        screen.addPreference(getThemePreference(context));
        screen.addPreference(getLightThemePreference(context));
        screen.addPreference(getDarkThemePreference(context));
    }

    private Preference getThemePreference(@NonNull final Context context) {
        final ListPreference preference = new ListPreference(context);
        preference.setSummaryProvider(ListPreference.SimpleSummaryProvider.getInstance());
        final int length = getResources().getStringArray(R.array.theme_presets).length;
        final String[] values = new String[length];
        for (int i = 0; i < length; i++) {
            values[i] = String.valueOf(i);
        }
        preference.setKey(PreferenceKeys.APP_THEME);
        preference.setTitle(R.string.theme_settings);
        preference.setDialogTitle(R.string.theme_settings);
        preference.setEntries(R.array.theme_presets);
        preference.setIconSpaceReserved(false);
        preference.setEntryValues(values);
        preference.setOnPreferenceChangeListener((preference1, newValue) -> {
            shouldRecreate();
            return true;
        });
        return preference;
    }

    private Preference getLightThemePreference(final Context context) {
        final ListPreference preference = new ListPreference(context);
        preference.setSummaryProvider(ListPreference.SimpleSummaryProvider.getInstance());
        final TypedArray lightThemeValues = getResources().obtainTypedArray(R.array.light_theme_values);
        final int length = lightThemeValues.length();
        final String[] values = new String[length];
        for (int i = 0; i < length; i++) {
            final int resourceId = lightThemeValues.getResourceId(i, -1);
            if (resourceId < 0) continue;
            values[i] = getResources().getResourceEntryName(resourceId);
        }
        lightThemeValues.recycle();
        preference.setKey(Constants.PREF_LIGHT_THEME);
        preference.setTitle(R.string.light_theme_settings);
        preference.setDialogTitle(R.string.light_theme_settings);
        preference.setEntries(R.array.light_themes);
        preference.setIconSpaceReserved(false);
        preference.setEntryValues(values);
        preference.setOnPreferenceChangeListener((preference1, newValue) -> {
            shouldRecreate();
            return true;
        });
        return preference;
    }

    private Preference getDarkThemePreference(final Context context) {
        final ListPreference preference = new ListPreference(context);
        preference.setSummaryProvider(ListPreference.SimpleSummaryProvider.getInstance());
        final TypedArray darkThemeValues = getResources().obtainTypedArray(R.array.dark_theme_values);
        final int length = darkThemeValues.length();
        final String[] values = new String[length];
        for (int i = 0; i < length; i++) {
            final int resourceId = darkThemeValues.getResourceId(i, -1);
            if (resourceId < 0) continue;
            values[i] = getResources().getResourceEntryName(resourceId);
        }
        darkThemeValues.recycle();
        preference.setKey(Constants.PREF_DARK_THEME);
        preference.setTitle(R.string.dark_theme_settings);
        preference.setDialogTitle(R.string.dark_theme_settings);
        preference.setEntries(R.array.dark_themes);
        preference.setIconSpaceReserved(false);
        preference.setEntryValues(values);
        preference.setOnPreferenceChangeListener((preference1, newValue) -> {
            shouldRecreate();
            return true;
        });
        return preference;
    }
}
