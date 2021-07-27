package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.util.SparseBooleanArray;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.DialogFragment;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.common.primitives.Booleans;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

public class MultiOptionDialogFragment<T extends Serializable> extends DialogFragment {
    private static final String TAG = MultiOptionDialogFragment.class.getSimpleName();

    public enum Type {
        MULTIPLE,
        SINGLE_CHECKED,
        SINGLE
    }

    private Context context;
    private Type type;
    private MultiOptionDialogCallback<T> callback;
    private MultiOptionDialogSingleCallback<T> singleCallback;
    private List<Option<?>> options;

    @NonNull
    public static <E extends Serializable> MultiOptionDialogFragment<E> newInstance(@StringRes final int title,
                                                                                    @NonNull final ArrayList<Option<E>> options) {
        return newInstance(title, 0, 0, options, Type.SINGLE);
    }

    @NonNull
    public static <E extends Serializable> MultiOptionDialogFragment<E> newInstance(@StringRes final int title,
                                                                                    @StringRes final int positiveButtonText,
                                                                                    @StringRes final int negativeButtonText,
                                                                                    @NonNull final ArrayList<Option<E>> options,
                                                                                    @NonNull final Type type) {
        Bundle args = new Bundle();
        args.putInt("title", title);
        args.putInt("positiveButtonText", positiveButtonText);
        args.putInt("negativeButtonText", negativeButtonText);
        args.putSerializable("options", options);
        args.putSerializable("type", type);
        MultiOptionDialogFragment<E> fragment = new MultiOptionDialogFragment<>();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onAttach(@NonNull final Context context) {
        super.onAttach(context);
        this.context = context;
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final Bundle arguments = getArguments();
        int title = 0;
        if (arguments != null) {
            title = arguments.getInt("title");
            type = (Type) arguments.getSerializable("type");
        }
        final MaterialAlertDialogBuilder builder = new MaterialAlertDialogBuilder(context);
        if (title > 0) {
            builder.setTitle(title);
        }
        try {
            //noinspection unchecked
            options = arguments != null ? (List<Option<?>>) arguments.getSerializable("options")
                                        : Collections.emptyList();
        } catch (Exception e) {
            Log.e(TAG, "onCreateDialog: ", e);
            options = Collections.emptyList();
        }
        final int negativeButtonText = arguments != null ? arguments.getInt("negativeButtonText", -1) : -1;
        if (negativeButtonText > 0) {
            builder.setNegativeButton(negativeButtonText, (dialog, which) -> {
                if (callback != null) {
                    callback.onCancel();
                    return;
                }
                if (singleCallback != null) {
                    singleCallback.onCancel();
                }
            });
        }
        if (type == Type.MULTIPLE || type == Type.SINGLE_CHECKED) {
            final int positiveButtonText = arguments != null ? arguments.getInt("positiveButtonText", -1) : -1;
            if (positiveButtonText > 0) {
                builder.setPositiveButton(positiveButtonText, (dialog, which) -> {
                    if (callback == null || options == null || options.isEmpty()) return;
                    try {
                        final List<T> selected = new ArrayList<>();
                        final SparseBooleanArray checkedItemPositions = ((AlertDialog) dialog).getListView().getCheckedItemPositions();
                        for (int i = 0; i < checkedItemPositions.size(); i++) {
                            final int position = checkedItemPositions.keyAt(i);
                            final boolean checked = checkedItemPositions.get(position);
                            if (!checked) continue;
                            //noinspection unchecked
                            final Option<T> option = (Option<T>) options.get(position);
                            selected.add(option.value);
                        }
                        callback.onMultipleSelect(selected);
                    } catch (Exception e) {
                        Log.e(TAG, "onCreateDialog: ", e);
                    }
                });
            }
        }
        if (type == Type.MULTIPLE) {
            if (options != null && !options.isEmpty()) {
                final String[] items = options.stream()
                                              .map(option -> option.label)
                                              .toArray(String[]::new);
                final boolean[] checkedItems = Booleans.toArray(options.stream()
                                                                       .map(option -> option.checked)
                                                                       .collect(Collectors.toList()));
                builder.setMultiChoiceItems(items, checkedItems, (dialog, which, isChecked) -> {
                    if (callback == null) return;
                    try {
                        final Option<?> option = options.get(which);
                        //noinspection unchecked
                        callback.onCheckChange((T) option.value, isChecked);
                    } catch (Exception e) {
                        Log.e(TAG, "onCreateDialog: ", e);
                    }
                });
            }
        } else {
            if (options != null && !options.isEmpty()) {
                final String[] items = options.stream()
                                              .map(option -> option.label)
                                              .toArray(String[]::new);
                if (type == Type.SINGLE_CHECKED) {
                    int index = -1;
                    for (int i = 0; i < options.size(); i++) {
                        if (options.get(i).checked) {
                            index = i;
                            break;
                        }
                    }
                    builder.setSingleChoiceItems(items, index, (dialog, which) -> {
                        if (callback == null) return;
                        try {
                            final Option<?> option = options.get(which);
                            //noinspection unchecked
                            callback.onCheckChange((T) option.value, true);
                        } catch (Exception e) {
                            Log.e(TAG, "onCreateDialog: ", e);
                        }
                    });
                } else if (type == Type.SINGLE) {
                    builder.setItems(items, (dialog, which) -> {
                        if (singleCallback == null) return;
                        try {
                            final Option<?> option = options.get(which);
                            //noinspection unchecked
                            singleCallback.onSelect((T) option.value);
                        } catch (Exception e) {
                            Log.e(TAG, "onCreateDialog: ", e);
                        }
                    });
                }
            }
        }
        return builder.create();
    }

    public void setCallback(final MultiOptionDialogCallback<T> callback) {
        if (callback == null) return;
        this.callback = callback;
    }

    public void setSingleCallback(final MultiOptionDialogSingleCallback<T> callback) {
        if (callback == null) return;
        this.singleCallback = callback;
    }

    public interface MultiOptionDialogCallback<T> {
        void onSelect(T result);

        void onMultipleSelect(List<T> result);

        void onCheckChange(T item, boolean isChecked);

        void onCancel();
    }

    public interface MultiOptionDialogSingleCallback<T> {
        void onSelect(T result);

        void onCancel();
    }

    public static class Option<T extends Serializable> {
        private final String label;
        private final T value;
        private final boolean checked;

        public Option(final String label, final T value) {
            this.label = label;
            this.value = value;
            this.checked = false;
        }

        public Option(final String label, final T value, final boolean checked) {
            this.label = label;
            this.value = value;
            this.checked = checked;
        }

        public String getLabel() {
            return label;
        }

        public T getValue() {
            return value;
        }

        public boolean isChecked() {
            return checked;
        }
    }
}
