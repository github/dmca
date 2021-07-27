package awais.instagrabber.customviews;

import android.content.Context;
import android.util.AttributeSet;
import android.view.WindowInsets;
import android.widget.LinearLayout;

import androidx.annotation.Nullable;

public class InsetsNotifyingLinearLayout extends LinearLayout {
    public InsetsNotifyingLinearLayout(final Context context) {
        super(context);
    }

    public InsetsNotifyingLinearLayout(final Context context, @Nullable final AttributeSet attrs) {
        super(context, attrs);
    }

    public InsetsNotifyingLinearLayout(final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public InsetsNotifyingLinearLayout(final Context context, final AttributeSet attrs, final int defStyleAttr, final int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
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
