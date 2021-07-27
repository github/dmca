package awais.instagrabber.customviews;

import android.content.Context;
import android.util.AttributeSet;
import android.view.WindowInsets;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.coordinatorlayout.widget.CoordinatorLayout;

public class InsetsNotifyingCoordinatorLayout extends CoordinatorLayout {

    public InsetsNotifyingCoordinatorLayout(@NonNull final Context context) {
        super(context);
    }

    public InsetsNotifyingCoordinatorLayout(@NonNull final Context context, @Nullable final AttributeSet attrs) {
        super(context, attrs);
    }

    public InsetsNotifyingCoordinatorLayout(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    public WindowInsets onApplyWindowInsets(WindowInsets insets) {
        int childCount = getChildCount();
        for (int index = 0; index < childCount; index++) {
            getChildAt(index).dispatchApplyWindowInsets(insets);
        }
        return insets;
    }
}
