package awais.instagrabber.customviews.helpers;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.viewpager2.widget.ViewPager2;

import static androidx.viewpager2.widget.ViewPager2.ORIENTATION_HORIZONTAL;

public class NestedScrollableHost extends FrameLayout {

    private int touchSlop;
    private float initialX = 0f;
    private float initialY = 0f;

    public NestedScrollableHost(@NonNull final Context context) {
        this(context, null);
    }

    public NestedScrollableHost(@NonNull final Context context, @Nullable final AttributeSet attrs) {
        super(context, attrs);
        touchSlop = ViewConfiguration.get(context).getScaledTouchSlop();
    }

    @Override
    public boolean onInterceptTouchEvent(final MotionEvent ev) {
        handleInterceptTouchEvent(ev);
        return super.onInterceptTouchEvent(ev);
    }

    private void handleInterceptTouchEvent(final MotionEvent e) {
        if (getParentViewPager() == null) return;
        final int orientation = getParentViewPager().getOrientation();
        // Early return if child can't scroll in same direction as parent
        if (!canChildScroll(orientation, -1f) && !canChildScroll(orientation, 1f)) return;

        if (e.getAction() == MotionEvent.ACTION_DOWN) {
            initialX = e.getX();
            initialY = e.getY();
            getParent().requestDisallowInterceptTouchEvent(true);
        } else if (e.getAction() == MotionEvent.ACTION_MOVE) {
            final float dx = e.getX() - initialX;
            final float dy = e.getY() - initialY;
            final boolean isVpHorizontal = orientation == ORIENTATION_HORIZONTAL;

            // assuming ViewPager2 touch-slop is 2x touch-slop of child
            final float scaledDx = Math.abs(dx) * (isVpHorizontal ? .5f : 1f);
            final float scaledDy = Math.abs(dy) * (isVpHorizontal ? 1f : .5f);

            if (scaledDx > touchSlop || scaledDy > touchSlop) {
                if (isVpHorizontal == (scaledDy > scaledDx)) {
                    // Gesture is perpendicular, allow all parents to intercept
                    getParent().requestDisallowInterceptTouchEvent(false);
                } else {
                    // Gesture is parallel, query child if movement in that direction is possible
                    if (canChildScroll(orientation, (isVpHorizontal ? dx : dy))) {
                        // Child can scroll, disallow all parents to intercept
                        getParent().requestDisallowInterceptTouchEvent(true);
                    } else {
                        // Child cannot scroll, allow all parents to intercept
                        getParent().requestDisallowInterceptTouchEvent(false);
                    }
                }
            }
        }
    }

    private boolean canChildScroll(final int orientation, final float delta) {
        final int direction = -(int) Math.signum(delta);
        final View child = getChild();
        if (child == null) return false;
        ViewPager2 viewPagerChild = null;
        if (child instanceof ViewPager2) {
            viewPagerChild = (ViewPager2) child;
        }

        boolean canScroll;
        switch (orientation) {
            case 0:
                canScroll = child.canScrollHorizontally(direction);
                break;
            case 1:
                canScroll = child.canScrollVertically(direction);
                break;
            default:
                throw new IllegalArgumentException();
        }
        if (!canScroll || viewPagerChild == null || viewPagerChild.getAdapter() == null)
            return canScroll;
        // check if viewpager has reached its limits and decide accordingly
        return (direction < 0 && viewPagerChild.getCurrentItem() > 0)
                || (direction > 0 && viewPagerChild.getCurrentItem() < viewPagerChild.getAdapter().getItemCount() - 1);
    }

    public ViewPager2 getParentViewPager() {
        View v = (View) getParent();
        while (v != null && !(v instanceof ViewPager2)) {
            v = (View) v.getParent();
        }
        return (ViewPager2) v;
    }

    public View getChild() {
        return getChildCount() > 0 ? getChildAt(0) : null;
    }
}
