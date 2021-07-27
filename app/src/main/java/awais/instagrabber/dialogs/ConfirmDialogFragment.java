package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import awais.instagrabber.R;

public class ConfirmDialogFragment extends DialogFragment {
    private Context context;
    private ConfirmDialogFragmentCallback callback;

    private final int defaultPositiveButtonText = R.string.ok;
    // private final int defaultNegativeButtonText = R.string.cancel;

    @NonNull
    public static ConfirmDialogFragment newInstance(final int requestCode,
                                                    @StringRes final int title,
                                                    @StringRes final int message,
                                                    @StringRes final int positiveText,
                                                    @StringRes final int negativeText,
                                                    @StringRes final int neutralText) {
        Bundle args = new Bundle();
        args.putInt("requestCode", requestCode);
        if (title != 0) {
            args.putInt("title", title);
        }
        if (message != 0) {
            args.putInt("message", message);
        }
        if (positiveText != 0) {
            args.putInt("positive", positiveText);
        }
        if (negativeText != 0) {
            args.putInt("negative", negativeText);
        }
        if (neutralText != 0) {
            args.putInt("neutral", neutralText);
        }
        ConfirmDialogFragment fragment = new ConfirmDialogFragment();
        fragment.setArguments(args);
        return fragment;
    }

    public ConfirmDialogFragment() {}

    @Override
    public void onAttach(@NonNull final Context context) {
        super.onAttach(context);
        final Fragment parentFragment = getParentFragment();
        if (parentFragment instanceof ConfirmDialogFragmentCallback) {
            callback = (ConfirmDialogFragmentCallback) parentFragment;
        }
        this.context = context;
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(@Nullable final Bundle savedInstanceState) {
        final Bundle arguments = getArguments();
        int title = 0;
        int message = 0;
        int neutralButtonText = 0;
        int negativeButtonText = 0;

        final int positiveButtonText;
        final int requestCode;
        if (arguments != null) {
            title = arguments.getInt("title", 0);
            message = arguments.getInt("message", 0);
            positiveButtonText = arguments.getInt("positive", defaultPositiveButtonText);
            negativeButtonText = arguments.getInt("negative", 0);
            neutralButtonText = arguments.getInt("neutral", 0);
            requestCode = arguments.getInt("requestCode", 0);
        } else {
            requestCode = 0;
            positiveButtonText = defaultPositiveButtonText;
        }
        final MaterialAlertDialogBuilder builder = new MaterialAlertDialogBuilder(context)
                .setPositiveButton(positiveButtonText, (d, w) -> {
                    if (callback == null) return;
                    callback.onPositiveButtonClicked(requestCode);
                });
        if (title != 0) {
            builder.setTitle(title);
        }
        if (message != 0) {
            builder.setMessage(message);
        }
        if (negativeButtonText != 0) {
            builder.setNegativeButton(negativeButtonText, (dialog, which) -> {
                if (callback == null) return;
                callback.onNegativeButtonClicked(requestCode);
            });
        }
        if (neutralButtonText != 0) {
            builder.setNeutralButton(neutralButtonText, (dialog, which) -> {
                if (callback == null) return;
                callback.onNeutralButtonClicked(requestCode);
            });
        }
        return builder.create();
    }

    public interface ConfirmDialogFragmentCallback {
        void onPositiveButtonClicked(int requestCode);

        void onNegativeButtonClicked(int requestCode);

        void onNeutralButtonClicked(int requestCode);
    }
}
