package awais.instagrabber.fragments.settings;

import android.content.Context;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.FragmentActivity;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;

import com.google.android.material.snackbar.BaseTransientBottomBar;
import com.google.android.material.snackbar.Snackbar;

import awais.instagrabber.R;
import awais.instagrabber.dialogs.CreateBackupDialogFragment;
import awais.instagrabber.dialogs.RestoreBackupDialogFragment;

public class BackupPreferencesFragment extends BasePreferencesFragment {

    @Override
    void setupPreferenceScreen(final PreferenceScreen screen) {
        final Context context = getContext();
        if (context == null) {
            return;
        }
        screen.addPreference(getAboutPreference(context));
        screen.addPreference(getWarningPreference(context));
        screen.addPreference(getCreatePreference(context));
        screen.addPreference(getRestorePreference(context));
    }

    private Preference getAboutPreference(@NonNull final Context context) {
        final Preference preference = new Preference(context);
        preference.setSummary(R.string.backup_summary);
        preference.setEnabled(false);
        preference.setIcon(R.drawable.ic_outline_info_24);
        preference.setIconSpaceReserved(true);
        return preference;
    }

    private Preference getWarningPreference(@NonNull final Context context) {
        final Preference preference = new Preference(context);
        preference.setSummary(R.string.backup_warning);
        preference.setEnabled(false);
        preference.setIcon(R.drawable.ic_warning);
        preference.setIconSpaceReserved(true);
        return preference;
    }

    private Preference getCreatePreference(@NonNull final Context context) {
        final Preference preference = new Preference(context);
        preference.setTitle(R.string.create_backup);
        preference.setIconSpaceReserved(false);
        preference.setOnPreferenceClickListener(preference1 -> {
            final FragmentManager fragmentManager = getParentFragmentManager();
            final CreateBackupDialogFragment fragment = new CreateBackupDialogFragment(result -> {
                final View view = getView();
                if (view != null) {
                    Snackbar.make(view,
                                  result ? R.string.dialog_export_success
                                         : R.string.dialog_export_failed,
                                  BaseTransientBottomBar.LENGTH_LONG)
                            .setAnimationMode(BaseTransientBottomBar.ANIMATION_MODE_SLIDE)
                            .setAction(R.string.ok, v -> {})
                            .show();
                    return;
                }
                Toast.makeText(context,
                               result ? R.string.dialog_export_success
                                      : R.string.dialog_export_failed,
                               Toast.LENGTH_LONG)
                     .show();
            });
            final FragmentTransaction ft = fragmentManager.beginTransaction();
            ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_OPEN)
              .add(fragment, "createBackup")
              .commit();
            return true;
        });
        return preference;
    }

    private Preference getRestorePreference(@NonNull final Context context) {
        final Preference preference = new Preference(context);
        preference.setTitle(R.string.restore_backup);
        preference.setIconSpaceReserved(false);
        preference.setOnPreferenceClickListener(preference1 -> {
            final FragmentManager fragmentManager = getParentFragmentManager();
            final RestoreBackupDialogFragment fragment = new RestoreBackupDialogFragment(result -> {
                final View view = getView();
                if (view != null) {
                    Snackbar.make(view,
                                  result ? R.string.dialog_import_success
                                         : R.string.dialog_import_failed,
                                  BaseTransientBottomBar.LENGTH_LONG)
                            .setAnimationMode(BaseTransientBottomBar.ANIMATION_MODE_SLIDE)
                            .setAction(R.string.ok, v -> {})
                            .addCallback(new BaseTransientBottomBar.BaseCallback<Snackbar>() {
                                @Override
                                public void onDismissed(final Snackbar transientBottomBar, final int event) {
                                    recreateActivity(result);
                                }
                            })
                            .show();
                    return;
                }
                recreateActivity(result);
            });
            final FragmentTransaction ft = fragmentManager.beginTransaction();
            ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_OPEN)
              .add(fragment, "restoreBackup")
              .commit();
            return true;
        });
        return preference;
    }

    private void recreateActivity(final boolean result) {
        if (!result) return;
        final FragmentActivity activity = getActivity();
        if (activity == null) return;
        activity.recreate();
    }
}
