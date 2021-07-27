package awais.instagrabber.fragments.settings;

import android.content.Context;
import android.util.Pair;

import androidx.annotation.NonNull;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;
import androidx.preference.SwitchPreferenceCompat;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.dialogs.ConfirmDialogFragment;
import awais.instagrabber.dialogs.TabOrderPreferenceDialogFragment;
import awais.instagrabber.models.Tab;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class GeneralPreferencesFragment extends BasePreferencesFragment implements TabOrderPreferenceDialogFragment.Callback {

    @Override
    void setupPreferenceScreen(final PreferenceScreen screen) {
        final Context context = getContext();
        if (context == null) return;
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        final boolean isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) > 0;
        if (isLoggedIn) {
            screen.addPreference(getDefaultTabPreference(context));
            screen.addPreference(getTabOrderPreference(context));
        }
        screen.addPreference(getUpdateCheckPreference(context));
        screen.addPreference(getFlagSecurePreference(context));
        screen.addPreference(getSearchFocusPreference(context));
        final List<Preference> preferences = FlavorSettings.getInstance()
                                                           .getPreferences(context,
                                                                           getChildFragmentManager(),
                                                                           SettingCategory.GENERAL);
        if (preferences != null) {
            for (final Preference preference : preferences) {
                screen.addPreference(preference);
            }
        }
    }

    private Preference getDefaultTabPreference(@NonNull final Context context) {
        final ListPreference preference = new ListPreference(context);
        preference.setSummaryProvider(ListPreference.SimpleSummaryProvider.getInstance());
        final Pair<List<Tab>, List<Tab>> listPair = Utils.getNavTabList(context);
        final List<Tab> tabs = listPair.first;
        final String[] titles = tabs.stream()
                                    .map(Tab::getTitle)
                                    .toArray(String[]::new);
        final String[] navGraphFileNames = tabs.stream()
                                               .map(Tab::getGraphName)
                                               .toArray(String[]::new);
        preference.setKey(Constants.DEFAULT_TAB);
        preference.setTitle(R.string.pref_start_screen);
        preference.setDialogTitle(R.string.pref_start_screen);
        preference.setEntries(titles);
        preference.setEntryValues(navGraphFileNames);
        preference.setIconSpaceReserved(false);
        return preference;
    }

    @NonNull
    private Preference getTabOrderPreference(@NonNull final Context context) {
        final Preference preference = new Preference(context);
        preference.setTitle(R.string.tab_order);
        preference.setIconSpaceReserved(false);
        preference.setOnPreferenceClickListener(preference1 -> {
            final TabOrderPreferenceDialogFragment dialogFragment = TabOrderPreferenceDialogFragment.newInstance();
            dialogFragment.show(getChildFragmentManager(), "tab_order_dialog");
            return true;
        });
        return preference;
    }

    private Preference getUpdateCheckPreference(@NonNull final Context context) {
        final SwitchPreferenceCompat preference = new SwitchPreferenceCompat(context);
        preference.setKey(PreferenceKeys.CHECK_UPDATES);
        preference.setTitle(R.string.update_check);
        preference.setIconSpaceReserved(false);
        return preference;
    }

    private Preference getFlagSecurePreference(@NonNull final Context context) {
        return PreferenceHelper.getSwitchPreference(
                context,
                PreferenceKeys.FLAG_SECURE,
                R.string.flag_secure,
                -1,
                false,
                (preference, newValue) -> {
                    shouldRecreate();
                    return true;
                });
    }

    private Preference getSearchFocusPreference(@NonNull final Context context) {
        final SwitchPreferenceCompat preference = new SwitchPreferenceCompat(context);
        preference.setKey(PreferenceKeys.PREF_SEARCH_FOCUS_KEYBOARD);
        preference.setTitle(R.string.pref_search_focus_keyboard);
        preference.setIconSpaceReserved(false);
        return preference;
    }

    @Override
    public void onSave(final boolean orderHasChanged) {
        if (!orderHasChanged) return;
        final ConfirmDialogFragment dialogFragment = ConfirmDialogFragment.newInstance(
                111,
                0,
                R.string.tab_order_start_next_launch,
                R.string.ok,
                0,
                0);
        dialogFragment.show(getChildFragmentManager(), "tab_order_set_dialog");
    }

    @Override
    public void onCancel() {

    }
}
