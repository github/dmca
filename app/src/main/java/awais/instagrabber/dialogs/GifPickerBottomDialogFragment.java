package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.text.Editable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.GridLayoutManager;

import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.bottomsheet.BottomSheetDialogFragment;
import com.google.android.material.snackbar.Snackbar;

import awais.instagrabber.R;
import awais.instagrabber.adapters.GifItemsAdapter;
import awais.instagrabber.customviews.helpers.TextWatcherAdapter;
import awais.instagrabber.databinding.LayoutGifPickerBinding;
import awais.instagrabber.repositories.responses.giphy.GiphyGif;
import awais.instagrabber.utils.Debouncer;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.viewmodels.GifPickerViewModel;

public class GifPickerBottomDialogFragment extends BottomSheetDialogFragment {
    private static final String TAG = GifPickerBottomDialogFragment.class.getSimpleName();
    private static final int INPUT_DEBOUNCE_INTERVAL = 500;
    private static final String INPUT_KEY = "gif_search_input";

    private LayoutGifPickerBinding binding;
    private GifPickerViewModel viewModel;
    private GifItemsAdapter gifItemsAdapter;
    private OnSelectListener onSelectListener;
    private Debouncer<String> inputDebouncer;

    public static GifPickerBottomDialogFragment newInstance() {
        final Bundle args = new Bundle();
        final GifPickerBottomDialogFragment fragment = new GifPickerBottomDialogFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(DialogFragment.STYLE_NORMAL, R.style.ThemeOverlay_Rounded_BottomSheetDialog);
        final Debouncer.Callback<String> callback = new Debouncer.Callback<String>() {
            @Override
            public void call(final String key) {
                final Editable text = binding.input.getText();
                if (TextUtils.isEmpty(text)) {
                    viewModel.search(null);
                    return;
                }
                viewModel.search(text.toString().trim());
            }

            @Override
            public void onError(final Throwable t) {
                Log.e(TAG, "onError: ", t);
            }
        };
        inputDebouncer = new Debouncer<>(callback, INPUT_DEBOUNCE_INTERVAL);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        binding = LayoutGifPickerBinding.inflate(inflater, container, false);
        viewModel = new ViewModelProvider(this).get(GifPickerViewModel.class);
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        init();
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
    }

    private void init() {
        setupList();
        setupInput();
        setupObservers();
    }

    private void setupList() {
        final Context context = getContext();
        if (context == null) return;
        binding.gifList.setLayoutManager(new GridLayoutManager(context, 3));
        binding.gifList.setHasFixedSize(true);
        gifItemsAdapter = new GifItemsAdapter(entry -> {
            if (onSelectListener == null) return;
            onSelectListener.onSelect(entry);
        });
        binding.gifList.setAdapter(gifItemsAdapter);
    }

    private void setupInput() {
        binding.input.addTextChangedListener(new TextWatcherAdapter() {
            @Override
            public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {
                inputDebouncer.call(INPUT_KEY);
            }
        });
    }

    private void setupObservers() {
        viewModel.getImages().observe(getViewLifecycleOwner(), imagesResource -> {
            if (imagesResource == null) return;
            switch (imagesResource.status) {
                case SUCCESS:
                    gifItemsAdapter.submitList(imagesResource.data);
                    break;
                case ERROR:
                    final Context context = getContext();
                    if (context != null && imagesResource.message != null) {
                        Snackbar.make(context, binding.getRoot(), imagesResource.message, Snackbar.LENGTH_LONG).show();
                    }
                    if (context != null && imagesResource.resId != 0) {
                        Snackbar.make(context, binding.getRoot(), getString(imagesResource.resId), Snackbar.LENGTH_LONG).show();
                    }
                    break;
                case LOADING:
                    break;
            }
        });
    }

    public void setOnSelectListener(final OnSelectListener onSelectListener) {
        this.onSelectListener = onSelectListener;
    }

    public interface OnSelectListener {
        void onSelect(GiphyGif giphyGif);
    }
}
