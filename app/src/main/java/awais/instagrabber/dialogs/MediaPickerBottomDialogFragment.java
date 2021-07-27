package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.GridLayoutManager;

import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.bottomsheet.BottomSheetDialogFragment;

import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.adapters.MediaItemsAdapter;
import awais.instagrabber.databinding.LayoutMediaPickerBinding;
import awais.instagrabber.utils.MediaController;
import awais.instagrabber.utils.PermissionUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.viewmodels.MediaPickerViewModel;

public class MediaPickerBottomDialogFragment extends BottomSheetDialogFragment {
    private static final String TAG = MediaPickerBottomDialogFragment.class.getSimpleName();
    private static final int ATTACH_MEDIA_REQUEST_CODE = 100;
    // private static final int HEIGHT_PIXELS = Utils.displayMetrics.heightPixels;

    private LayoutMediaPickerBinding binding;
    private MediaPickerViewModel viewModel;
    private MediaItemsAdapter mediaItemsAdapter;
    private OnSelectListener onSelectListener;
    // private int actionBarHeight;
    // private int statusBarHeight;

    // private final BottomSheetBehavior.BottomSheetCallback bottomSheetCallback = new BottomSheetBehavior.BottomSheetCallback() {
    //     @Override
    //     public void onStateChanged(@NonNull final View bottomSheet, final int newState) {
    //
    //     }
    //
    //     @Override
    //     public void onSlide(@NonNull final View bottomSheet, final float slideOffset) {
    //         // Log.d(TAG, "onSlide: " + slideOffset);
    //         final float sheetHeight = HEIGHT_PIXELS * slideOffset;
    //         final Context context = getContext();
    //         if (context == null) return;
    //         final float remaining = HEIGHT_PIXELS - sheetHeight - statusBarHeight;
    //         if (remaining <= actionBarHeight) {
    //             final ViewGroup.LayoutParams layoutParams = binding.toolbar.getLayoutParams();
    //             layoutParams.height = (int) (actionBarHeight - remaining);
    //             binding.toolbar.requestLayout();
    //         }
    //     }
    // };

    public static MediaPickerBottomDialogFragment newInstance() {
        final Bundle args = new Bundle();
        final MediaPickerBottomDialogFragment fragment = new MediaPickerBottomDialogFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(DialogFragment.STYLE_NORMAL, R.style.ThemeOverlay_Rounded_BottomSheetDialog);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        binding = LayoutMediaPickerBinding.inflate(inflater, container, false);
        viewModel = new ViewModelProvider(this).get(MediaPickerViewModel.class);
        // final Context context = getContext();
        // if (context == null) return binding.getRoot();
        // actionBarHeight = Utils.getActionBarHeight(context);
        // statusBarHeight = Utils.getStatusBarHeight(context);
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        init();
        // final Dialog dialog = getDialog();
        // if (dialog == null) return;
        // dialog.setOnShowListener(dialog1 -> {
        //     final BottomSheetDialog bottomSheetDialog = (BottomSheetDialog) dialog;
        //     final View bottomSheetInternal = bottomSheetDialog.findViewById(com.google.android.material.R.id.design_bottom_sheet);
        //     if (bottomSheetInternal == null) return;
        //     final BottomSheetBehavior<View> behavior = BottomSheetBehavior.from(bottomSheetInternal);
        //     behavior.setState(BottomSheetBehavior.STATE_EXPANDED);
        // });
    }

    @Override
    public void onStart() {
        super.onStart();
        final Dialog dialog = getDialog();
        if (dialog == null) return;
        final BottomSheetDialog bottomSheetDialog = (BottomSheetDialog) dialog;
        final View bottomSheetInternal = bottomSheetDialog.findViewById(com.google.android.material.R.id.design_bottom_sheet);
        if (bottomSheetInternal == null) return;
        bottomSheetInternal.getLayoutParams().height = ViewGroup.LayoutParams.MATCH_PARENT;
        bottomSheetInternal.requestLayout();
        // final BottomSheetBehavior<View> behavior = BottomSheetBehavior.from(bottomSheetInternal);
        // behavior.addBottomSheetCallback(bottomSheetCallback);
    }

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        if (requestCode == ATTACH_MEDIA_REQUEST_CODE) {
            final Context context = getContext();
            if (context == null) return;
            final boolean hasAttachMediaPerms = PermissionUtils.hasAttachMediaPerms(context);
            if (hasAttachMediaPerms) {
                viewModel.loadMedia(context);
            }
        }
    }

    private void init() {
        setupList();
        setupAlbumPicker();
        final Context context = getContext();
        if (context == null) return;
        if (!PermissionUtils.hasAttachMediaPerms(context)) {
            PermissionUtils.requestAttachMediaPerms(this, ATTACH_MEDIA_REQUEST_CODE);
            return;
        }
        viewModel.loadMedia(context);
    }

    private void setupList() {
        final Context context = getContext();
        if (context == null) return;
        binding.mediaList.setLayoutManager(new GridLayoutManager(context, 3));
        binding.mediaList.setHasFixedSize(true);
        mediaItemsAdapter = new MediaItemsAdapter(entry -> {
            if (onSelectListener == null) return;
            onSelectListener.onSelect(entry);
        });
        binding.mediaList.setAdapter(mediaItemsAdapter);
    }

    private void setupAlbumPicker() {
        final Context context = getContext();
        if (context == null) return;
        final ArrayAdapter<String> albumPickerAdapter = new ArrayAdapter<>(context, android.R.layout.simple_spinner_item);
        albumPickerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        binding.albumPicker.setAdapter(albumPickerAdapter);
        binding.albumPicker.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(final AdapterView<?> parent, final View view, final int position, final long id) {
                final List<MediaController.AlbumEntry> albumEntries = viewModel.getAllAlbums().getValue();
                if (albumEntries == null) return;
                final MediaController.AlbumEntry albumEntry = albumEntries.get(position);
                mediaItemsAdapter.submitList(albumEntry.photos, () -> binding.mediaList.scrollToPosition(0));
            }

            @Override
            public void onNothingSelected(final AdapterView<?> parent) {
                mediaItemsAdapter.submitList(Collections.emptyList());
            }
        });
        viewModel.getAllAlbums().observe(getViewLifecycleOwner(), albums -> {
            albumPickerAdapter.clear();
            albumPickerAdapter.addAll(albums.stream()
                                            .map(albumEntry -> albumEntry.bucketName)
                                            .filter(name -> !TextUtils.isEmpty(name))
                                            .collect(Collectors.toList()));
            albumPickerAdapter.notifyDataSetChanged();
            if (albums.isEmpty()) return;
            mediaItemsAdapter.submitList(albums.get(0).photos);
        });
    }

    public void setOnSelectListener(final OnSelectListener onSelectListener) {
        this.onSelectListener = onSelectListener;
    }

    public interface OnSelectListener {
        void onSelect(MediaController.MediaEntry entry);
    }
}
