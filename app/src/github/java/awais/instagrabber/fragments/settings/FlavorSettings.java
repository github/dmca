package awais.instagrabber.fragments.settings;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.fragment.app.FragmentManager;
import androidx.preference.Preference;

import com.google.common.collect.ImmutableList;

import java.util.Collections;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.dialogs.ConfirmDialogFragment;

import static awais.instagrabber.fragments.settings.PreferenceKeys.PREF_ENABLE_SENTRY;
import static awais.instagrabber.utils.Utils.settingsHelper;

public final class FlavorSettings implements IFlavorSettings {

    private static FlavorSettings instance;

    private FlavorSettings() {
    }

    public static FlavorSettings getInstance() {
        if (instance == null) {
            instance = new FlavorSettings();
        }
        return instance;
    }

    @NonNull
    @Override
    public List<Preference> getPreferences(@NonNull final Context context,
                                           @NonNull final FragmentManager fragmentManager,
                                           @NonNull final SettingCategory settingCategory) {
        switch (settingCategory) {
            case GENERAL:
                return getGeneralPrefs(context, fragmentManager);
            default:
                break;
        }
        return Collections.emptyList();
    }

    private List<Preference> getGeneralPrefs(@NonNull final Context context,
                                             @NonNull final FragmentManager fragmentManager) {
        return ImmutableList.of(
                getSentryPreference(context, fragmentManager)
        );
    }

    private Preference getSentryPreference(@NonNull final Context context,
                                           @NonNull final FragmentManager fragmentManager) {
        if (!settingsHelper.hasPreference(PREF_ENABLE_SENTRY)) {
            // disabled by default
            settingsHelper.putBoolean(PREF_ENABLE_SENTRY, false);
        }
        return PreferenceHelper.getSwitchPreference(
                context,
                PREF_ENABLE_SENTRY,
                R.string.enable_sentry,
                R.string.sentry_summary,
                false,
                (preference, newValue) -> {
                    if (!(newValue instanceof Boolean)) return true;
                    final boolean enabled = (Boolean) newValue;
                    if (enabled) {
                        final ConfirmDialogFragment dialogFragment = ConfirmDialogFragment.newInstance(
                                111,
                                0,
                                R.string.sentry_start_next_launch,
                                R.string.ok,
                                0,
                                0);
                        dialogFragment.show(fragmentManager, "sentry_dialog");
                    }
                    return true;
                });
    }
}
