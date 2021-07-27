package awais.instagrabber.customviews.emoji;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.bottomsheet.BottomSheetDialogFragment;

import awais.instagrabber.R;
import awais.instagrabber.customviews.helpers.GridSpacingItemDecoration;
import awais.instagrabber.utils.Utils;

public class EmojiBottomSheetDialog extends BottomSheetDialogFragment {
    public static final String TAG = EmojiBottomSheetDialog.class.getSimpleName();

    private RecyclerView grid;
    private EmojiPicker.OnEmojiClickListener callback;

    @NonNull
    public static EmojiBottomSheetDialog newInstance() {
        // Bundle args = new Bundle();
        // fragment.setArguments(args);
        return new EmojiBottomSheetDialog();
    }

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(DialogFragment.STYLE_NORMAL, R.style.ThemeOverlay_Rounded_BottomSheetDialog);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        final Context context = getContext();
        if (context == null) return null;
        grid = new RecyclerView(context);
        return grid;
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

    @Override
    public void onAttach(@NonNull final Context context) {
        super.onAttach(context);
        final Fragment parentFragment = getParentFragment();
        if (parentFragment instanceof EmojiPicker.OnEmojiClickListener) {
            callback = (EmojiPicker.OnEmojiClickListener) parentFragment;
        }
    }

    @Override
    public void onDestroyView() {
        grid = null;
        super.onDestroyView();
    }

    private void init() {
        final Context context = getContext();
        if (context == null) return;
        final GridLayoutManager gridLayoutManager = new GridLayoutManager(context, 9);
        grid.setLayoutManager(gridLayoutManager);
        grid.setHasFixedSize(true);
        grid.setClipToPadding(false);
        grid.addItemDecoration(new GridSpacingItemDecoration(Utils.convertDpToPx(8)));
        final EmojiGridAdapter adapter = new EmojiGridAdapter(null, (view, emoji) -> {
            if (callback != null) {
                callback.onClick(view, emoji);
            }
            dismiss();
        }, null);
        grid.setAdapter(adapter);
    }
}
