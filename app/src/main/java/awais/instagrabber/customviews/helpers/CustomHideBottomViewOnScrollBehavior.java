package awais.instagrabber.customviews.helpers;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.core.view.ViewCompat;

import com.google.android.material.behavior.HideBottomViewOnScrollBehavior;
import com.google.android.material.bottomnavigation.BottomNavigationView;

public class CustomHideBottomViewOnScrollBehavior extends HideBottomViewOnScrollBehavior<BottomNavigationView> {
    private static final String TAG = "CustomHideBottomView";

    public CustomHideBottomViewOnScrollBehavior() {
    }

    public CustomHideBottomViewOnScrollBehavior(final Context context, final AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public boolean onStartNestedScroll(@NonNull final CoordinatorLayout coordinatorLayout,
                                       @NonNull final BottomNavigationView child,
                                       @NonNull final View directTargetChild,
                                       @NonNull final View target,
                                       final int nestedScrollAxes,
                                       final int type) {
        return nestedScrollAxes == ViewCompat.SCROLL_AXIS_VERTICAL;
    }

    @Override
    public void onNestedPreScroll(@NonNull final CoordinatorLayout coordinatorLayout,
                                  @NonNull final BottomNavigationView child,
                                  @NonNull final View target,
                                  final int dx,
                                  final int dy,
                                  @NonNull final int[] consumed,
                                  final int type) {
        if (dy > 0) {
            slideDown(child);
        } else if (dy < 0) {
            slideUp(child);
        }
    }
}
