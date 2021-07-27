package awais.instagrabber.customviews.helpers;

import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsAnimationCompat;
import androidx.core.view.WindowInsetsCompat;

import java.util.List;

/**
 * A customized {@link TranslateDeferringInsetsAnimationCallback} for the emoji picker
 */
public class EmojiPickerInsetsAnimationCallback extends WindowInsetsAnimationCompat.Callback {
    private static final String TAG = EmojiPickerInsetsAnimationCallback.class.getSimpleName();

    private final View view;
    private final int persistentInsetTypes;
    private final int deferredInsetTypes;

    private int kbHeight;
    private onKbVisibilityChangeListener listener;
    private boolean shouldTranslate;

    public EmojiPickerInsetsAnimationCallback(final View view,
                                              final int persistentInsetTypes,
                                              final int deferredInsetTypes) {
        this(view, persistentInsetTypes, deferredInsetTypes, DISPATCH_MODE_STOP);
    }

    public EmojiPickerInsetsAnimationCallback(final View view,
                                              final int persistentInsetTypes,
                                              final int deferredInsetTypes,
                                              final int dispatchMode) {
        super(dispatchMode);
        if ((persistentInsetTypes & deferredInsetTypes) != 0) {
            throw new IllegalArgumentException("persistentInsetTypes and deferredInsetTypes can not contain " +
                                                       "any of same WindowInsetsCompat.Type values");
        }
        this.view = view;
        this.persistentInsetTypes = persistentInsetTypes;
        this.deferredInsetTypes = deferredInsetTypes;
    }

    @NonNull
    @Override
    public WindowInsetsCompat onProgress(@NonNull final WindowInsetsCompat insets,
                                         @NonNull final List<WindowInsetsAnimationCompat> runningAnimations) {
        // onProgress() is called when any of the running animations progress...

        // First we get the insets which are potentially deferred
        final Insets typesInset = insets.getInsets(deferredInsetTypes);
        // Then we get the persistent inset types which are applied as padding during layout
        final Insets otherInset = insets.getInsets(persistentInsetTypes);

        // Now that we subtract the two insets, to calculate the difference. We also coerce
        // the insets to be >= 0, to make sure we don't use negative insets.
        final Insets subtract = Insets.subtract(typesInset, otherInset);
        final Insets diff = Insets.max(subtract, Insets.NONE);

        // The resulting `diff` insets contain the values for us to apply as a translation
        // to the view
        view.setTranslationX(diff.left - diff.right);
        view.setTranslationY(shouldTranslate ? diff.top - diff.bottom : -kbHeight);

        return insets;
    }

    @Override
    public void onEnd(@NonNull final WindowInsetsAnimationCompat animation) {
        try {
            final WindowInsetsCompat rootWindowInsets = ViewCompat.getRootWindowInsets(view);
            if (kbHeight == 0) {
                if (rootWindowInsets == null) return;
                final Insets imeInsets = rootWindowInsets.getInsets(WindowInsetsCompat.Type.ime());
                final Insets navBarInsets = rootWindowInsets.getInsets(WindowInsetsCompat.Type.navigationBars());
                kbHeight = imeInsets.bottom - navBarInsets.bottom;
                final ViewGroup.MarginLayoutParams layoutParams = (ViewGroup.MarginLayoutParams) view.getLayoutParams();
                if (layoutParams != null) {
                    layoutParams.height = kbHeight;
                    layoutParams.setMargins(layoutParams.leftMargin, layoutParams.topMargin, layoutParams.rightMargin, -kbHeight);
                }
            }
            view.setTranslationX(0f);
            final boolean visible = rootWindowInsets != null && rootWindowInsets.isVisible(WindowInsetsCompat.Type.ime());
            float translationY = 0;
            if (!shouldTranslate) {
                translationY = -kbHeight;
                if (visible) {
                    translationY = 0;
                }
            }
            view.setTranslationY(translationY);

            if (listener != null && rootWindowInsets != null) {
                listener.onChange(visible);
            }
        } finally {
            shouldTranslate = true;
        }
    }

    public void setShouldTranslate(final boolean shouldTranslate) {
        this.shouldTranslate = shouldTranslate;
    }

    public void setKbVisibilityListener(final onKbVisibilityChangeListener listener) {
        this.listener = listener;
    }

    public interface onKbVisibilityChangeListener {
        void onChange(boolean isVisible);
    }
}
