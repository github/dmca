package awais.instagrabber.fragments.settings;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;
import androidx.preference.SwitchPreferenceCompat;

import awais.instagrabber.R;

public class StoriesPreferencesFragment extends BasePreferencesFragment {
    @Override
    void setupPreferenceScreen(final PreferenceScreen screen) {
        final Context context = getContext();
        if (context == null) return;
        screen.addPreference(getStorySortPreference(context));
        screen.addPreference(getHideMutedReelsPreference(context));
        screen.addPreference(getMarkStoriesSeenPreference(context));
    }

    private Preference getStorySortPreference(@NonNull final Context context) {
        final ListPreference preference = new ListPreference(context);
        preference.setSummaryProvider(ListPreference.SimpleSummaryProvider.getInstance());
        final int length = getResources().getStringArray(R.array.story_sorts).length;
        final String[] values = new String[length];
        for (int i = 0; i < length; i++) {
            values[i] = String.valueOf(i);
        }
        preference.setKey(PreferenceKeys.STORY_SORT);
        preference.setTitle(R.string.story_sort_setting);
        preference.setDialogTitle(R.string.story_sort_setting);
        preference.setEntries(R.array.story_sorts);
        preference.setIconSpaceReserved(false);
        preference.setEntryValues(values);
        return preference;
    }

    private Preference getHideMutedReelsPreference(@NonNull final Context context) {
        final SwitchPreferenceCompat preference = new SwitchPreferenceCompat(context);
        preference.setKey(PreferenceKeys.HIDE_MUTED_REELS);
        preference.setTitle(R.string.hide_muted_reels_setting);
        preference.setIconSpaceReserved(false);
        return preference;
    }

    private Preference getMarkStoriesSeenPreference(@NonNull final Context context) {
        final SwitchPreferenceCompat preference = new SwitchPreferenceCompat(context);
        preference.setKey(PreferenceKeys.MARK_AS_SEEN);
        preference.setTitle(R.string.mark_as_seen_setting);
        preference.setSummary(R.string.mark_as_seen_setting_summary);
        preference.setIconSpaceReserved(false);
        return preference;
    }
}
