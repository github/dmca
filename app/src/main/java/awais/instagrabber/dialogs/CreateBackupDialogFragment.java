package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.inputmethod.InputMethodManager;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.FragmentTransaction;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import awais.instagrabber.databinding.DialogCreateBackupBinding;
import awais.instagrabber.utils.DirectoryChooser;
import awais.instagrabber.utils.ExportImportUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.fragments.settings.PreferenceKeys.FOLDER_PATH;
import static awais.instagrabber.utils.DownloadUtils.PERMS;

public class CreateBackupDialogFragment extends DialogFragment {
    private static final int STORAGE_PERM_REQUEST_CODE = 8020;
    private static final SimpleDateFormat BACKUP_FILE_DATE_TIME_FORMAT = new SimpleDateFormat("yyyyMMddHHmmss", Locale.US);

    private final OnResultListener onResultListener;
    private DialogCreateBackupBinding binding;

    public CreateBackupDialogFragment(final OnResultListener onResultListener) {
        this.onResultListener = onResultListener;
    }

    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             final ViewGroup container,
                             final Bundle savedInstanceState) {
        binding = DialogCreateBackupBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Dialog dialog = super.onCreateDialog(savedInstanceState);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        return dialog;
    }

    @Override
    public void onStart() {
        super.onStart();
        final Dialog dialog = getDialog();
        if (dialog == null) return;
        final Window window = dialog.getWindow();
        if (window == null) return;
        final int height = ViewGroup.LayoutParams.WRAP_CONTENT;
        final int width = (int) (Utils.displayMetrics.widthPixels * 0.8);
        window.setLayout(width, height);
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        init();
    }

    private void init() {
        binding.etPassword.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(final CharSequence s, final int start, final int count, final int after) {}

            @Override
            public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {
                binding.btnSaveTo.setEnabled(!TextUtils.isEmpty(s));
            }

            @Override
            public void afterTextChanged(final Editable s) {}
        });
        final Context context = getContext();
        if (context == null) {
            return;
        }
        binding.cbPassword.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                if (TextUtils.isEmpty(binding.etPassword.getText())) {
                    binding.btnSaveTo.setEnabled(false);
                }
                binding.passwordField.setVisibility(View.VISIBLE);
                binding.etPassword.requestFocus();
                final InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
                if (imm == null) return;
                imm.showSoftInput(binding.etPassword, InputMethodManager.SHOW_IMPLICIT);
                return;
            }
            binding.btnSaveTo.setEnabled(true);
            binding.passwordField.setVisibility(View.GONE);
            final InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm == null) return;
            imm.hideSoftInputFromWindow(binding.etPassword.getWindowToken(), InputMethodManager.RESULT_UNCHANGED_SHOWN);
        });
        binding.btnSaveTo.setOnClickListener(v -> {
            if (ContextCompat.checkSelfPermission(context, PERMS[0]) == PackageManager.PERMISSION_GRANTED) {
                showChooser(context);
            } else {
                requestPermissions(PERMS, STORAGE_PERM_REQUEST_CODE);
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == STORAGE_PERM_REQUEST_CODE && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            final Context context = getContext();
            if (context == null) return;
            showChooser(context);
        }
    }

    private void showChooser(@NonNull final Context context) {
        final String folderPath = Utils.settingsHelper.getString(FOLDER_PATH);
        final Editable passwordText = binding.etPassword.getText();
        final String password = binding.cbPassword.isChecked()
                                        && passwordText != null
                                        && !TextUtils.isEmpty(passwordText.toString())
                                ? passwordText.toString().trim()
                                : null;
        final DirectoryChooser directoryChooser = new DirectoryChooser()
                .setInitialDirectory(folderPath)
                .setInteractionListener(path -> {
                    final Date now = new Date();
                    final File file = new File(path, String.format("barinsta_%s.backup", BACKUP_FILE_DATE_TIME_FORMAT.format(now)));
                    int flags = 0;
                    if (binding.cbExportFavorites.isChecked()) {
                        flags |= ExportImportUtils.FLAG_FAVORITES;
                    }
                    if (binding.cbExportSettings.isChecked()) {
                        flags |= ExportImportUtils.FLAG_SETTINGS;
                    }
                    if (binding.cbExportLogins.isChecked()) {
                        flags |= ExportImportUtils.FLAG_COOKIES;
                    }
                    ExportImportUtils.exportData(context, flags, file, password, result -> {
                        if (onResultListener != null) {
                            onResultListener.onResult(result);
                        }
                        dismiss();
                    });

                });
        directoryChooser.setEnterTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        directoryChooser.setExitTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        directoryChooser.show(getChildFragmentManager(), "directory_chooser");
    }

    public interface OnResultListener {
        void onResult(boolean result);
    }
}
