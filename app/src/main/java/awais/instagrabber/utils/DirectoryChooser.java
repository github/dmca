package awais.instagrabber.utils;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Environment;
import android.os.FileObserver;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.google.android.material.snackbar.BaseTransientBottomBar;
import com.google.android.material.snackbar.Snackbar;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectoryFilesAdapter;
import awais.instagrabber.databinding.LayoutDirectoryChooserBinding;
import awais.instagrabber.viewmodels.FileListViewModel;

public final class DirectoryChooser extends DialogFragment {
    private static final String TAG = "DirectoryChooser";

    public static final String KEY_CURRENT_DIRECTORY = "CURRENT_DIRECTORY";
    private static final File sdcardPathFile = Environment.getExternalStorageDirectory();
    private static final String sdcardPath = sdcardPathFile.getPath();

    private Context context;
    private LayoutDirectoryChooserBinding binding;
    private FileObserver fileObserver;
    private File selectedDir;
    private String initialDirectory;
    private OnFragmentInteractionListener interactionListener;
    private boolean showBackupFiles = false;
    private View.OnClickListener navigationOnClickListener;
    private FileListViewModel fileListViewModel;
    private OnCancelListener onCancelListener;

    public DirectoryChooser() {
        super();
    }

    public DirectoryChooser setInitialDirectory(final String initialDirectory) {
        if (!TextUtils.isEmpty(initialDirectory))
            this.initialDirectory = initialDirectory;
        return this;
    }

    public DirectoryChooser setShowBackupFiles(final boolean showBackupFiles) {
        this.showBackupFiles = showBackupFiles;
        return this;
    }

    @Override
    public void onAttach(@NonNull final Context context) {
        super.onAttach(context);

        this.context = context;

        if (this.context instanceof OnFragmentInteractionListener)
            interactionListener = (OnFragmentInteractionListener) this.context;
        else {
            final Fragment owner = getTargetFragment();
            if (owner instanceof OnFragmentInteractionListener)
                interactionListener = (OnFragmentInteractionListener) owner;
        }
    }

    @NonNull
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        binding = LayoutDirectoryChooserBinding.inflate(inflater, container, false);
        init(container);
        return binding.getRoot();
    }

    private void init(final ViewGroup container) {
        Context context = this.context;
        if (context == null) context = getContext();
        if (context == null) context = getActivity();
        if (context == null) return;
        if (ContextCompat.checkSelfPermission(context, DownloadUtils.PERMS[0]) != PackageManager.PERMISSION_GRANTED) {
            final String text = "Storage permissions denied!";
            if (container == null) {
                Toast.makeText(context, text, Toast.LENGTH_LONG).show();
            } else {
                Snackbar.make(container, text, BaseTransientBottomBar.LENGTH_LONG).show();
            }
            dismiss();
        }
        final View.OnClickListener clickListener = v -> {
            if (v == binding.btnConfirm) {
                if (interactionListener != null && isValidFile(selectedDir))
                    interactionListener.onSelectDirectory(selectedDir);
                dismiss();
            } else if (v == binding.btnCancel) {
                if (onCancelListener != null) {
                    onCancelListener.onCancel();
                }
                dismiss();
            }
        };

        navigationOnClickListener = v -> {
            final File parent;
            if (selectedDir != null && (parent = selectedDir.getParentFile()) != null) {
                changeDirectory(parent);
            }
        };
        binding.toolbar.setNavigationOnClickListener(navigationOnClickListener);
        binding.toolbar.setSubtitle(showBackupFiles ? R.string.select_backup_file : R.string.select_folder);
        binding.btnCancel.setOnClickListener(clickListener);
        // no need to show confirm for file picker
        binding.btnConfirm.setVisibility(showBackupFiles ? View.GONE : View.VISIBLE);
        if (!showBackupFiles) {
            binding.btnConfirm.setOnClickListener(clickListener);
        }
        fileListViewModel = new ViewModelProvider(this).get(FileListViewModel.class);
        final DirectoryFilesAdapter listDirectoriesAdapter = new DirectoryFilesAdapter(file -> {
            if (file.isDirectory()) {
                changeDirectory(file);
                return;
            }
            if (showBackupFiles && file.isFile()) {
                if (interactionListener != null && file.canRead()) {
                    interactionListener.onSelectDirectory(file);
                }
                dismiss();
            }
        });
        fileListViewModel.getList().observe(this, listDirectoriesAdapter::submitList);
        binding.directoryList.setLayoutManager(new LinearLayoutManager(context));
        binding.directoryList.setAdapter(listDirectoriesAdapter);
        final File initDir = new File(initialDirectory);
        final File initialDir = !TextUtils.isEmpty(initialDirectory) && isValidFile(initDir) ? initDir : Environment.getExternalStorageDirectory();
        changeDirectory(initialDir);
    }

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (TextUtils.isEmpty(initialDirectory)) {
            initialDirectory = new File(sdcardPath, "Download").getAbsolutePath();
            if (savedInstanceState != null) {
                final String savedDir = savedInstanceState.getString(KEY_CURRENT_DIRECTORY);
                if (!TextUtils.isEmpty(savedDir)) initialDirectory = savedDir;
            }
        }

        setStyle(DialogFragment.STYLE_NO_TITLE, 0);
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(final Bundle savedInstanceState) {
        return new Dialog(context, R.attr.alertDialogTheme) {
            @Override
            public void onBackPressed() {
                if (selectedDir != null) {
                    final String absolutePath = selectedDir.getAbsolutePath();
                    if (absolutePath.equals(sdcardPath) || absolutePath.equals(sdcardPathFile.getAbsolutePath())) {
                        if (onCancelListener != null) {
                            onCancelListener.onCancel();
                        }
                        dismiss();
                    } else {
                        changeDirectory(selectedDir.getParentFile());
                    }
                }
            }
        };
    }

    @Override
    public void onSaveInstanceState(@NonNull final Bundle outState) {
        super.onSaveInstanceState(outState);
        if (selectedDir != null) outState.putString(KEY_CURRENT_DIRECTORY, selectedDir.getAbsolutePath());
    }

    @Override
    public void onResume() {
        super.onResume();
        if (fileObserver != null) fileObserver.startWatching();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (fileObserver != null) fileObserver.stopWatching();
    }

    @Override
    public void onDetach() {
        super.onDetach();
        interactionListener = null;
    }

    private void changeDirectory(final File dir) {
        if (dir != null && dir.isDirectory()) {
            final String path = dir.getAbsolutePath();
            binding.toolbar.setTitle(path);
            final File[] contents = dir.listFiles();
            if (contents != null) {
                final List<File> fileNames = new ArrayList<>();
                for (final File f : contents) {
                    final String name = f.getName();
                    final String nameLowerCase = name.toLowerCase();
                    final boolean isBackupFile = nameLowerCase.endsWith(".zaai") || nameLowerCase.endsWith(".backup");
                    if (f.isDirectory() || (showBackupFiles && f.isFile() && isBackupFile))
                        fileNames.add(f);
                }
                Collections.sort(fileNames, (o1, o2) -> {
                    if ((o1.isDirectory() && o2.isDirectory())
                            || (o1.isFile() && o2.isFile())) {
                        return o1.getName().compareToIgnoreCase(o2.getName());
                    }
                    if (o1.isDirectory()) return -1;
                    if (o2.isDirectory()) return 1;
                    return 0;
                });
                fileListViewModel.getList().postValue(fileNames);
                selectedDir = dir;
                fileObserver = new FileObserver(path, FileObserver.CREATE | FileObserver.DELETE | FileObserver.MOVED_FROM | FileObserver.MOVED_TO) {
                    private final Runnable currentDirRefresher = () -> changeDirectory(selectedDir);

                    @Override
                    public void onEvent(final int event, final String path) {
                        if (context instanceof Activity) ((Activity) context).runOnUiThread(currentDirRefresher);
                    }
                };
                fileObserver.startWatching();
            }
        }
        refreshButtonState();
    }

    private void refreshButtonState() {
        if (selectedDir != null) {
            final String path = selectedDir.getAbsolutePath();
            toggleUpButton(!path.equals(sdcardPathFile.getAbsolutePath()) && selectedDir != sdcardPathFile);
            binding.btnConfirm.setEnabled(isValidFile(selectedDir));
        }
    }

    private void toggleUpButton(final boolean enable) {
        binding.toolbar.setNavigationOnClickListener(enable ? navigationOnClickListener : null);
        final Drawable navigationIcon = binding.toolbar.getNavigationIcon();
        if (navigationIcon == null) return;
        navigationIcon.setAlpha(enable ? 255 : (int) (255 * 0.617));
    }

    private boolean isValidFile(final File file) {
        return file != null && file.isDirectory() && file.canRead();
    }

    public DirectoryChooser setInteractionListener(final OnFragmentInteractionListener interactionListener) {
        this.interactionListener = interactionListener;
        return this;
    }

    public void setOnCancelListener(final OnCancelListener onCancelListener) {
        if (onCancelListener != null) {
            this.onCancelListener = onCancelListener;
        }
    }

    public interface OnCancelListener {
        void onCancel();
    }

    public interface OnFragmentInteractionListener {
        void onSelectDirectory(final File file);
    }
}