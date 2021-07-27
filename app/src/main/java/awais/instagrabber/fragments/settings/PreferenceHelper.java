package awais.instagrabber.fragments.settings;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.SwitchPreferenceCompat;

public final class PreferenceHelper {

    public static SwitchPreferenceCompat getSwitchPreference(@NonNull final Context context,
                                                             @NonNull final String key,
                                                             @StringRes final int titleResId,
                                                             @StringRes final int summaryResId,
                                                             final boolean iconSpaceReserved,
                                                             final OnPreferenceChangeListener onPreferenceChangeListener) {
        final SwitchPreferenceCompat preference = new SwitchPreferenceCompat(context);
        preference.setKey(key);
        preference.setTitle(titleResId);
        preference.setIconSpaceReserved(iconSpaceReserved);
        if (summaryResId != -1) {
            preference.setSummary(summaryResId);
        }
        if (onPreferenceChangeListener != null) {
            preference.setOnPreferenceChangeListener(onPreferenceChangeListener);
        }
        return preference;
    }
}
