package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
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

import awais.instagrabber.databinding.DialogRestoreBackupBinding;
import awais.instagrabber.utils.DirectoryChooser;
import awais.instagrabber.utils.ExportImportUtils;
import awais.instagrabber.utils.PasswordUtils.IncorrectPasswordException;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.fragments.settings.PreferenceKeys.FOLDER_PATH;
import static awais.instagrabber.utils.DownloadUtils.PERMS;

public class RestoreBackupDialogFragment extends DialogFragment {
    private static final int STORAGE_PERM_REQUEST_CODE = 8020;

    private OnResultListener onResultListener;

    private DialogRestoreBackupBinding binding;
    private File file;
    private boolean isEncrypted;

    public RestoreBackupDialogFragment() {}

    public RestoreBackupDialogFragment(final OnResultListener onResultListener) {
        this.onResultListener = onResultListener;
    }

    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             final ViewGroup container,
                             final Bundle savedInstanceState) {
        binding = DialogRestoreBackupBinding.inflate(inflater, container, false);
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

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == STORAGE_PERM_REQUEST_CODE && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            showChooser();
        }
    }

    private void init() {
        final Context context = getContext();
        if (context == null) {
            return;
        }
        binding.btnRestore.setEnabled(false);
        binding.btnRestore.setOnClickListener(v -> new Handler().post(() -> {
            int flags = 0;
            if (binding.cbFavorites.isChecked()) {
                flags |= ExportImportUtils.FLAG_FAVORITES;
            }
            if (binding.cbSettings.isChecked()) {
                flags |= ExportImportUtils.FLAG_SETTINGS;
            }
            if (binding.cbAccounts.isChecked()) {
                flags |= ExportImportUtils.FLAG_COOKIES;
            }
            final Editable text = binding.etPassword.getText();
            if (isEncrypted && text == null) return;
            try {
                ExportImportUtils.importData(
                        context,
                        flags,
                        file,
                        !isEncrypted ? null : text.toString(),
                        result -> {
                            if (onResultListener != null) {
                                onResultListener.onResult(result);
                            }
                            dismiss();
                        }
                );
            } catch (IncorrectPasswordException e) {
                binding.passwordField.setError("Incorrect password");
            }
        }));
        binding.etPassword.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(final CharSequence s, final int start, final int count, final int after) {}

            @Override
            public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {
                binding.btnRestore.setEnabled(!TextUtils.isEmpty(s));
                binding.passwordField.setError(null);
            }

            @Override
            public void afterTextChanged(final Editable s) {}
        });
        if (ContextCompat.checkSelfPermission(context, PERMS[0]) == PackageManager.PERMISSION_GRANTED) {
            showChooser();
            return;
        }
        requestPermissions(PERMS, STORAGE_PERM_REQUEST_CODE);
    }

    private void showChooser() {
        final String folderPath = Utils.settingsHelper.getString(FOLDER_PATH);
        final Context context = getContext();
        if (context == null) return;
        final DirectoryChooser directoryChooser = new DirectoryChooser()
                .setInitialDirectory(folderPath)
                .setShowBackupFiles(true)
                .setInteractionListener(file -> {
                    isEncrypted = ExportImportUtils.isEncrypted(file);
                    if (isEncrypted) {
                        binding.passwordGroup.setVisibility(View.VISIBLE);
                        binding.passwordGroup.post(() -> {
                            binding.etPassword.requestFocus();
                            binding.etPassword.post(() -> {
                                final InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
                                if (imm == null) return;
                                imm.showSoftInput(binding.etPassword, InputMethodManager.SHOW_IMPLICIT);
                            });
                            binding.btnRestore.setEnabled(!TextUtils.isEmpty(binding.etPassword.getText()));
                        });
                    } else {
                        binding.passwordGroup.setVisibility(View.GONE);
                        binding.btnRestore.setEnabled(true);
                    }
                    this.file = file;
                    binding.filePath.setText(file.getAbsolutePath());
                });
        directoryChooser.setEnterTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        directoryChooser.setExitTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        directoryChooser.setOnCancelListener(this::dismiss);
        directoryChooser.show(getChildFragmentManager(), "directory_chooser");
    }

    public interface OnResultListener {
        void onResult(boolean result);
    }
}
