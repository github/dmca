package awais.instagrabber.fragments.settings;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;

import java.text.SimpleDateFormat;
import java.util.Date;

import awais.instagrabber.R;
import awais.instagrabber.dialogs.TimeSettingsDialog;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.LocaleUtils;
import awais.instagrabber.utils.UserAgentUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class LocalePreferencesFragment extends BasePreferencesFragment {
    @Override
    void setupPreferenceScreen(final PreferenceScreen screen) {
        final Context context = getContext();
        if (context == null) return;
        screen.addPreference(getLanguagePreference(context));
        screen.addPreference(getPostTimeFormatPreference(context));
    }

    private Preference getLanguagePreference(@NonNull final Context context) {
        final ListPreference preference = new ListPreference(context);
        preference.setSummaryProvider(ListPreference.SimpleSummaryProvider.getInstance());
        final int length = getResources().getStringArray(R.array.languages).length;
        final String[] values = new String[length];
        for (int i = 0; i < length; i++) {
            values[i] = String.valueOf(i);
        }
        preference.setKey(PreferenceKeys.APP_LANGUAGE);
        preference.setTitle(R.string.select_language);
        preference.setDialogTitle(R.string.select_language);
        preference.setEntries(R.array.languages);
        preference.setIconSpaceReserved(false);
        preference.setEntryValues(values);
        preference.setOnPreferenceChangeListener((preference1, newValue) -> {
            shouldRecreate();
            final int appUaCode = settingsHelper.getInteger(Constants.APP_UA_CODE);
            final String appUa = UserAgentUtils.generateAppUA(appUaCode, LocaleUtils.getCurrentLocale().getLanguage());
            settingsHelper.putString(Constants.APP_UA, appUa);
            return true;
        });
        return preference;
    }

    private Preference getPostTimeFormatPreference(@NonNull final Context context) {
        final Preference preference = new Preference(context);
        preference.setTitle(R.string.time_settings);
        preference.setSummary(Utils.datetimeParser.format(new Date()));
        preference.setIconSpaceReserved(false);
        preference.setOnPreferenceClickListener(preference1 -> {
            new TimeSettingsDialog(
                    settingsHelper.getBoolean(PreferenceKeys.CUSTOM_DATE_TIME_FORMAT_ENABLED),
                    settingsHelper.getString(PreferenceKeys.CUSTOM_DATE_TIME_FORMAT),
                    settingsHelper.getString(PreferenceKeys.DATE_TIME_SELECTION),
                    settingsHelper.getBoolean(PreferenceKeys.SWAP_DATE_TIME_FORMAT_ENABLED),
                    (isCustomFormat,
                     formatSelection,
                     spTimeFormatSelectedItemPosition,
                     spSeparatorSelectedItemPosition,
                     spDateFormatSelectedItemPosition,
                     selectedFormat,
                     currentFormat,
                     swapDateTime) -> {
                        if (isCustomFormat) {
                            settingsHelper.putString(PreferenceKeys.CUSTOM_DATE_TIME_FORMAT, formatSelection);
                        } else {
                            final String formatSelectionUpdated = spTimeFormatSelectedItemPosition + ";"
                                    + spSeparatorSelectedItemPosition + ';'
                                    + spDateFormatSelectedItemPosition; // time;separator;date
                            settingsHelper.putString(PreferenceKeys.DATE_TIME_FORMAT, selectedFormat);
                            settingsHelper.putString(PreferenceKeys.DATE_TIME_SELECTION, formatSelectionUpdated);
                        }
                        settingsHelper.putBoolean(PreferenceKeys.CUSTOM_DATE_TIME_FORMAT_ENABLED, isCustomFormat);
                        settingsHelper.putBoolean(PreferenceKeys.SWAP_DATE_TIME_FORMAT_ENABLED, swapDateTime);
                        Utils.datetimeParser = (SimpleDateFormat) currentFormat.clone();
                        preference.setSummary(Utils.datetimeParser.format(new Date()));
                    }
            ).show(getParentFragmentManager(), null);
            return true;
        });
        return preference;
    }
}
