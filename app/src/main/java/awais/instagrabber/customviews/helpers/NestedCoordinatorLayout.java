package awais.instagrabber.customviews.helpers;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;

import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.core.view.NestedScrollingChild;
import androidx.core.view.NestedScrollingChildHelper;

public class NestedCoordinatorLayout extends CoordinatorLayout implements NestedScrollingChild {

    private NestedScrollingChildHelper mChildHelper;

    public NestedCoordinatorLayout(Context context) {
        super(context);
        mChildHelper = new NestedScrollingChildHelper(this);
        setNestedScrollingEnabled(true);
    }

    public NestedCoordinatorLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        mChildHelper = new NestedScrollingChildHelper(this);
        setNestedScrollingEnabled(true);
    }

    public NestedCoordinatorLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mChildHelper = new NestedScrollingChildHelper(this);
        setNestedScrollingEnabled(true);
    }

    @Override
    public void onNestedPreScroll(View target, int dx, int dy, int[] consumed, int type) {
        int[][] tConsumed = new int[2][2];
        super.onNestedPreScroll(target, dx, dy, consumed, type);
        dispatchNestedPreScroll(dx, dy, tConsumed[1], null);
        consumed[0] = tConsumed[0][0] + tConsumed[1][0];
        consumed[1] = tConsumed[0][1] + tConsumed[1][1];
    }

    @Override
    public void onNestedScroll(View target, int dxConsumed, int dyConsumed, int dxUnconsumed, int dyUnconsumed, int type) {
        super.onNestedScroll(target, dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed, type);
        dispatchNestedScroll(dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed, null);
    }

    @Override
    public void onStopNestedScroll(View target, int type) {
        /* Disable the scrolling behavior of our own children */
        super.onStopNestedScroll(target, type);
        /* Disable the scrolling behavior of the parent's other children  */
        stopNestedScroll();
    }

    @Override
    public boolean onStartNestedScroll(View child, View target, int nestedScrollAxes, int type) {
        /* Enable the scrolling behavior of our own children */
        boolean tHandled = super.onStartNestedScroll(child, target, nestedScrollAxes, type);
        /* Enable the scrolling behavior of the parent's other children  */
        return startNestedScroll(nestedScrollAxes) || tHandled;
    }

    @Override
    public boolean onStartNestedScroll(View child, View target, int nestedScrollAxes) {
        /* Enable the scrolling behavior of our own children */
        boolean tHandled = super.onStartNestedScroll(child, target, nestedScrollAxes);
        /* Enable the scrolling behavior of the parent's other children  */
        return startNestedScroll(nestedScrollAxes) || tHandled;
    }

    @Override
    public void onStopNestedScroll(View target) {
        /* Disable the scrolling behavior of our own children */
        super.onStopNestedScroll(target);
        /* Disable the scrolling behavior of the parent's other children  */
        stopNestedScroll();
    }

    @Override
    public void onNestedPreScroll(View target, int dx, int dy, int[] consumed) {
        int[][] tConsumed = new int[2][2];
        super.onNestedPreScroll(target, dx, dy, tConsumed[0]);
        dispatchNestedPreScroll(dx, dy, tConsumed[1], null);
        consumed[0] = tConsumed[0][0] + tConsumed[1][0];
        consumed[1] = tConsumed[0][1] + tConsumed[1][1];
    }

    @Override
    public void onNestedScroll(View target, int dxConsumed, int dyConsumed,
                               int dxUnconsumed, int dyUnconsumed) {
        super.onNestedScroll(target, dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed);
        dispatchNestedScroll(dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed, null);
    }

    @Override
    public boolean onNestedPreFling(View target, float velocityX, float velocityY) {
        boolean tHandled = super.onNestedPreFling(target, velocityX, velocityY);
        return dispatchNestedPreFling(velocityX, velocityY) || tHandled;
    }

    @Override
    public boolean onNestedFling(View target, float velocityX, float velocityY, boolean consumed) {
        boolean tHandled = super.onNestedFling(target, velocityX, velocityY, consumed);
        return dispatchNestedFling(velocityX, velocityY, consumed) || tHandled;
    }

    @Override
    public boolean isNestedScrollingEnabled() {
        return mChildHelper.isNestedScrollingEnabled();
    }

    @Override
    public void setNestedScrollingEnabled(boolean enabled) {
        mChildHelper.setNestedScrollingEnabled(enabled);
    }

    @Override
    public boolean startNestedScroll(int axes) {
        return mChildHelper.startNestedScroll(axes);
    }

    @Override
    public void stopNestedScroll() {
        mChildHelper.stopNestedScroll();
    }

    @Override
    public boolean hasNestedScrollingParent() {
        return mChildHelper.hasNestedScrollingParent();
    }

    @Override
    public boolean dispatchNestedScroll(int dxConsumed, int dyConsumed, int dxUnconsumed,
                                        int dyUnconsumed, int[] offsetInWindow) {
        return mChildHelper.dispatchNestedScroll(dxConsumed, dyConsumed, dxUnconsumed,
                dyUnconsumed, offsetInWindow);
    }

    @Override
    public boolean dispatchNestedPreScroll(int dx, int dy, int[] consumed, int[] offsetInWindow) {
        return mChildHelper.dispatchNestedPreScroll(dx, dy, consumed, offsetInWindow);
    }

    @Override
    public boolean dispatchNestedFling(float velocityX, float velocityY, boolean consumed) {
        return mChildHelper.dispatchNestedFling(velocityX, velocityY, consumed);
    }

    @Override
    public boolean dispatchNestedPreFling(float velocityX, float velocityY) {
        return mChildHelper.dispatchNestedPreFling(velocityX, velocityY);
    }
}
