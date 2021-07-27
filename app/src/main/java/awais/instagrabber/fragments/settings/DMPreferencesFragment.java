package awais.instagrabber.fragments.settings;

import android.content.Context;
import android.content.Intent;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;

import androidx.annotation.NonNull;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;
import androidx.preference.PreferenceViewHolder;

import java.util.Objects;

import awais.instagrabber.R;
import awais.instagrabber.customviews.helpers.TextWatcherAdapter;
import awais.instagrabber.databinding.PrefAutoRefreshDmFreqBinding;
import awais.instagrabber.services.DMSyncAlarmReceiver;
import awais.instagrabber.services.DMSyncService;
import awais.instagrabber.utils.Debouncer;
import awais.instagrabber.utils.TextUtils;

import static awais.instagrabber.fragments.settings.PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH_FREQ_NUMBER;
import static awais.instagrabber.fragments.settings.PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH_FREQ_UNIT;
import static awais.instagrabber.utils.Utils.settingsHelper;

public class DMPreferencesFragment extends BasePreferencesFragment {
    private static final String TAG = DMPreferencesFragment.class.getSimpleName();

    @Override
    void setupPreferenceScreen(final PreferenceScreen screen) {
        final Context context = getContext();
        if (context == null) return;
        screen.addPreference(getMarkDMSeenPreference(context));
        // screen.addPreference(getAutoRefreshDMPreference(context));
        // screen.addPreference(getAutoRefreshDMFreqPreference(context));
    }

    private Preference getMarkDMSeenPreference(@NonNull final Context context) {
        return PreferenceHelper.getSwitchPreference(
                context,
                PreferenceKeys.DM_MARK_AS_SEEN,
                R.string.dm_mark_as_seen_setting,
                R.string.dm_mark_as_seen_setting_summary,
                false,
                null
        );
    }

    private Preference getAutoRefreshDMPreference(@NonNull final Context context) {
        return PreferenceHelper.getSwitchPreference(
                context,
                PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH,
                R.string.enable_dm_auto_refesh,
                -1,
                false,
                (preference, newValue) -> {
                    if (!(newValue instanceof Boolean)) return false;
                    final boolean enabled = (Boolean) newValue;
                    if (enabled) {
                        DMSyncAlarmReceiver.setAlarm(context);
                        return true;
                    }
                    DMSyncAlarmReceiver.cancelAlarm(context);
                    try {
                        final Context applicationContext = context.getApplicationContext();
                        applicationContext.stopService(new Intent(applicationContext, DMSyncService.class));
                    } catch (Exception e) {
                        Log.e(TAG, "getAutoRefreshDMPreference: ", e);
                    }
                    return true;
                }
        );
    }

    private Preference getAutoRefreshDMFreqPreference(@NonNull final Context context) {
        return new AutoRefreshDMFrePreference(context);
    }

    public static class AutoRefreshDMFrePreference extends Preference {
        private static final String TAG = AutoRefreshDMFrePreference.class.getSimpleName();
        private static final String DEBOUNCE_KEY = "dm_sync_service_update";
        public static final int INTERVAL = 2000;

        private final Debouncer.Callback<String> changeCallback;

        private Debouncer<String> serviceUpdateDebouncer;
        private PrefAutoRefreshDmFreqBinding binding;

        public AutoRefreshDMFrePreference(final Context context) {
            super(context);
            setLayoutResource(R.layout.pref_auto_refresh_dm_freq);
            // setKey(key);
            setIconSpaceReserved(false);
            changeCallback = new Debouncer.Callback<String>() {
                @Override
                public void call(final String key) {
                    DMSyncAlarmReceiver.setAlarm(context);
                }

                @Override
                public void onError(final Throwable t) {
                    Log.e(TAG, "onError: ", t);
                }
            };
            serviceUpdateDebouncer = new Debouncer<>(changeCallback, INTERVAL);
        }

        @Override
        public void onDependencyChanged(final Preference dependency, final boolean disableDependent) {
            // super.onDependencyChanged(dependency, disableDependent);
            if (binding == null) return;
            binding.startText.setEnabled(!disableDependent);
            binding.freqNum.setEnabled(!disableDependent);
            binding.freqUnit.setEnabled(!disableDependent);
            if (disableDependent) {
                serviceUpdateDebouncer.terminate();
                return;
            }
            serviceUpdateDebouncer = new Debouncer<>(changeCallback, INTERVAL);
        }

        @Override
        public void onBindViewHolder(final PreferenceViewHolder holder) {
            super.onBindViewHolder(holder);
            setDependency(PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH);
            binding = PrefAutoRefreshDmFreqBinding.bind(holder.itemView);
            final Context context = getContext();
            if (context == null) return;
            setupUnitSpinner(context);
            setupNumberEditText(context);
        }

        private void setupUnitSpinner(final Context context) {
            final ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(context,
                                                                                       R.array.dm_auto_refresh_freq_unit_labels,
                                                                                       android.R.layout.simple_spinner_item);
            final String[] values = context.getResources().getStringArray(R.array.dm_auto_refresh_freq_units);
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            binding.freqUnit.setAdapter(adapter);

            String unit = settingsHelper.getString(PREF_ENABLE_DM_AUTO_REFRESH_FREQ_UNIT);
            if (TextUtils.isEmpty(unit)) {
                unit = "secs";
            }
            int position = 0;
            for (int i = 0; i < values.length; i++) {
                if (Objects.equals(unit, values[i])) {
                    position = i;
                    break;
                }
            }
            binding.freqUnit.setSelection(position);
            binding.freqUnit.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(final AdapterView<?> parent, final View view, final int position, final long id) {
                    settingsHelper.putString(PREF_ENABLE_DM_AUTO_REFRESH_FREQ_UNIT, values[position]);
                    if (!isEnabled()) {
                        serviceUpdateDebouncer.terminate();
                        return;
                    }
                    serviceUpdateDebouncer.call(DEBOUNCE_KEY);
                }

                @Override
                public void onNothingSelected(final AdapterView<?> parent) {}
            });
        }

        private void setupNumberEditText(final Context context) {
            int currentValue = settingsHelper.getInteger(PREF_ENABLE_DM_AUTO_REFRESH_FREQ_NUMBER);
            if (currentValue <= 0) {
                currentValue = 30;
            }
            binding.freqNum.setText(String.valueOf(currentValue));
            binding.freqNum.addTextChangedListener(new TextWatcherAdapter() {

                @Override
                public void afterTextChanged(final Editable s) {
                    if (TextUtils.isEmpty(s)) return;
                    try {
                        final int value = Integer.parseInt(s.toString());
                        if (value <= 0) return;
                        settingsHelper.putInteger(PREF_ENABLE_DM_AUTO_REFRESH_FREQ_NUMBER, value);
                        if (!isEnabled()) {
                            serviceUpdateDebouncer.terminate();
                            return;
                        }
                        serviceUpdateDebouncer.call(DEBOUNCE_KEY);
                    } catch (Exception e) {
                        Log.e(TAG, "afterTextChanged: ", e);
                    }
                }
            });
        }
    }
}
