package awais.instagrabber.customviews;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.IntDef;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.view.GravityCompat;
import androidx.core.view.ViewCompat;
import androidx.customview.view.AbsSavedState;
import androidx.customview.widget.ViewDragHelper;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.BuildConfig;

// exactly same as the LayoutDrawer with some edits
@SuppressLint("RtlHardcoded")
public class MouseDrawer extends ViewGroup {
    @IntDef({ViewDragHelper.STATE_IDLE, ViewDragHelper.STATE_DRAGGING, ViewDragHelper.STATE_SETTLING})
    @Retention(RetentionPolicy.SOURCE)
    private @interface State {}

    @IntDef(value = {Gravity.NO_GRAVITY, Gravity.LEFT, Gravity.RIGHT, GravityCompat.START, GravityCompat.END}, flag = true)
    @Retention(RetentionPolicy.SOURCE)
    public @interface EdgeGravity {}

    ////////////////////////////////////////////////////////////////////////////////////
    private static final boolean CHILDREN_DISALLOW_INTERCEPT = true;
    ////////////////////////////////////////////////////////////////////////////////////
    private final ArrayList<View> mNonDrawerViews = new ArrayList<>();
    private final ViewDragHelper mLeftDragger, mRightDragger;
    private boolean mInLayout, mFirstLayout = true;
    private float mDrawerElevation, mInitialMotionX, mInitialMotionY;
    private int mDrawerState;
    private List<DrawerListener> mListeners;
    private Matrix mChildInvertedMatrix;
    private Rect mChildHitRect;

    public interface DrawerListener {
        void onDrawerSlide(final View drawerView, @EdgeGravity final int gravity, final float slideOffset);
        default void onDrawerOpened(final View drawerView, @EdgeGravity final int gravity) {}
        default void onDrawerClosed(final View drawerView, @EdgeGravity final int gravity) {}
        default void onDrawerStateChanged() {}
    }

    public MouseDrawer(@NonNull final Context context) {
        this(context, null);
    }

    public MouseDrawer(@NonNull final Context context, @Nullable final AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MouseDrawer(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyle) {
        super(context, attrs, defStyle);
        setDescendantFocusability(ViewGroup.FOCUS_AFTER_DESCENDANTS);

        final float density = getResources().getDisplayMetrics().density;
        this.mDrawerElevation = 10 * density;

        final float touchSlopSensitivity = 0.5f; // was 1.0f
        final float minFlingVelocity = 400 /* dips per second */ * density;

        final ViewDragCallback mLeftCallback = new ViewDragCallback(Gravity.LEFT);
        this.mLeftDragger = ViewDragHelper.create(this, touchSlopSensitivity, mLeftCallback);
        this.mLeftDragger.setEdgeTrackingEnabled(ViewDragHelper.EDGE_LEFT);
        this.mLeftDragger.setMinVelocity(minFlingVelocity);

        final ViewDragCallback mRightCallback = new ViewDragCallback(Gravity.RIGHT);
        this.mRightDragger = ViewDragHelper.create(this, touchSlopSensitivity, mRightCallback);
        this.mRightDragger.setEdgeTrackingEnabled(ViewDragHelper.EDGE_RIGHT);
        this.mRightDragger.setMinVelocity(minFlingVelocity);

        try {
            final Field edgeSizeField = ViewDragHelper.class.getDeclaredField("mEdgeSize");
            if (!edgeSizeField.isAccessible()) edgeSizeField.setAccessible(true);
            final int widthPixels = getResources().getDisplayMetrics().widthPixels; // whole screen
            edgeSizeField.set(this.mLeftDragger, widthPixels / 2);
            edgeSizeField.set(this.mRightDragger, widthPixels / 2);
        } catch (final Exception e) {
            if (BuildConfig.DEBUG) Log.e("AWAISKING_APP", "", e);
        }

        mLeftCallback.setDragger(mLeftDragger);
        mRightCallback.setDragger(mRightDragger);

        setFocusableInTouchMode(true);
        //setMotionEventSplittingEnabled(false);
    }

    public void setDrawerElevation(final float elevation) {
        mDrawerElevation = elevation;
        for (int i = 0; i < getChildCount(); i++) {
            final View child = getChildAt(i);
            if (isDrawerView(child)) ViewCompat.setElevation(child, mDrawerElevation);
        }
    }

    public float getDrawerElevation() {
        return Build.VERSION.SDK_INT >= 21 ? mDrawerElevation : 0f;
    }

    public void addDrawerListener(@NonNull final DrawerListener listener) {
        if (mListeners == null) mListeners = new ArrayList<>();
        mListeners.add(listener);
    }

    private boolean isInBoundsOfChild(final float x, final float y, final View child) {
        if (mChildHitRect == null) mChildHitRect = new Rect();
        child.getHitRect(mChildHitRect);
        return mChildHitRect.contains((int) x, (int) y);
    }

    private boolean dispatchTransformedGenericPointerEvent(final MotionEvent event, @NonNull final View child) {
        final boolean handled;
        final Matrix childMatrix = child.getMatrix();
        if (!childMatrix.isIdentity()) {
            final MotionEvent transformedEvent = getTransformedMotionEvent(event, child);
            handled = child.dispatchGenericMotionEvent(transformedEvent);
            transformedEvent.recycle();
        } else {
            final float offsetX = getScrollX() - child.getLeft();
            final float offsetY = getScrollY() - child.getTop();
            event.offsetLocation(offsetX, offsetY);
            handled = child.dispatchGenericMotionEvent(event);
            event.offsetLocation(-offsetX, -offsetY);
        }
        return handled;
    }

    @NonNull
    private MotionEvent getTransformedMotionEvent(final MotionEvent event, @NonNull final View child) {
        final float offsetX = getScrollX() - child.getLeft();
        final float offsetY = getScrollY() - child.getTop();
        final MotionEvent transformedEvent = MotionEvent.obtain(event);
        transformedEvent.offsetLocation(offsetX, offsetY);
        final Matrix childMatrix = child.getMatrix();
        if (!childMatrix.isIdentity()) {
            if (mChildInvertedMatrix == null) mChildInvertedMatrix = new Matrix();
            childMatrix.invert(mChildInvertedMatrix);
            transformedEvent.transform(mChildInvertedMatrix);
        }
        return transformedEvent;
    }

    void updateDrawerState(@State final int activeState, final View activeDrawer) {
        final int leftState = mLeftDragger.getViewDragState();
        final int rightState = mRightDragger.getViewDragState();

        final int state;
        if (leftState == ViewDragHelper.STATE_DRAGGING || rightState == ViewDragHelper.STATE_DRAGGING)
            state = ViewDragHelper.STATE_DRAGGING;
        else if (leftState == ViewDragHelper.STATE_SETTLING || rightState == ViewDragHelper.STATE_SETTLING)
            state = ViewDragHelper.STATE_SETTLING;
        else state = ViewDragHelper.STATE_IDLE;

        if (activeDrawer != null && activeState == ViewDragHelper.STATE_IDLE) {
            final LayoutParams lp = (LayoutParams) activeDrawer.getLayoutParams();
            if (lp.onScreen == 0) dispatchOnDrawerClosed(activeDrawer);
            else if (lp.onScreen == 1) dispatchOnDrawerOpened(activeDrawer);
        }

        if (state != mDrawerState) {
            mDrawerState = state;

            if (mListeners != null) {
                final int listenerCount = mListeners.size();
                for (int i = listenerCount - 1; i >= 0; i--) mListeners.get(i).onDrawerStateChanged();
            }
        }
    }

    void dispatchOnDrawerClosed(@NonNull final View drawerView) {
        final LayoutParams lp = (LayoutParams) drawerView.getLayoutParams();
        if ((lp.openState & LayoutParams.FLAG_IS_OPENED) == 1) {
            lp.openState = 0;

            if (mListeners != null) {
                final int listenerCount = mListeners.size();
                for (int i = listenerCount - 1; i >= 0; i--) mListeners.get(i).onDrawerClosed(drawerView, lp.gravity);
            }
        }
    }

    void dispatchOnDrawerOpened(@NonNull final View drawerView) {
        final LayoutParams lp = (LayoutParams) drawerView.getLayoutParams();
        if ((lp.openState & LayoutParams.FLAG_IS_OPENED) == 0) {
            lp.openState = LayoutParams.FLAG_IS_OPENED;
            if (mListeners != null) {
                final int listenerCount = mListeners.size();
                for (int i = listenerCount - 1; i >= 0; i--) mListeners.get(i).onDrawerOpened(drawerView, lp.gravity);
            }
        }
    }

    void setDrawerViewOffset(@NonNull final View drawerView, final float slideOffset) {
        final LayoutParams lp = (LayoutParams) drawerView.getLayoutParams();
        if (slideOffset != lp.onScreen) {
            lp.onScreen = slideOffset;

            if (mListeners != null) {
                final int listenerCount = mListeners.size();
                for (int i = listenerCount - 1; i >= 0; i--)
                    mListeners.get(i).onDrawerSlide(drawerView, lp.gravity, slideOffset);
            }
        }
    }

    float getDrawerViewOffset(@NonNull final View drawerView) {
        return ((LayoutParams) drawerView.getLayoutParams()).onScreen;
    }

    int getDrawerViewAbsoluteGravity(@NonNull final View drawerView) {
        final int gravity = ((LayoutParams) drawerView.getLayoutParams()).gravity;
        return GravityCompat.getAbsoluteGravity(gravity, ViewCompat.getLayoutDirection(this));
    }

    boolean checkDrawerViewAbsoluteGravity(final View drawerView, final int checkFor) {
        final int absGravity = getDrawerViewAbsoluteGravity(drawerView);
        return (absGravity & checkFor) == checkFor;
    }

    void moveDrawerToOffset(final View drawerView, final float slideOffset) {
        final float oldOffset = getDrawerViewOffset(drawerView);
        final int width = drawerView.getWidth();
        final int oldPos = (int) (width * oldOffset);
        final int newPos = (int) (width * slideOffset);
        final int dx = newPos - oldPos;

        drawerView.offsetLeftAndRight(checkDrawerViewAbsoluteGravity(drawerView, Gravity.LEFT) ? dx : -dx);
        setDrawerViewOffset(drawerView, slideOffset);
    }

    public View findOpenDrawer() {
        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = getChildAt(i);
            final LayoutParams childLp = (LayoutParams) child.getLayoutParams();
            if ((childLp.openState & LayoutParams.FLAG_IS_OPENED) == 1) return child;
        }
        return null;
    }

    public View findDrawerWithGravity(final int gravity) {
        final int absHorizGravity = GravityCompat.getAbsoluteGravity(gravity, ViewCompat.getLayoutDirection(this)) & Gravity.HORIZONTAL_GRAVITY_MASK;
        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = getChildAt(i);
            final int childAbsGravity = getDrawerViewAbsoluteGravity(child);
            if ((childAbsGravity & Gravity.HORIZONTAL_GRAVITY_MASK) == absHorizGravity) return child;
        }
        return null;
    }

    @NonNull
    static String gravityToString(@EdgeGravity final int gravity) {
        if ((gravity & Gravity.LEFT) == Gravity.LEFT) return "LEFT";
        if ((gravity & Gravity.RIGHT) == Gravity.RIGHT) return "RIGHT";
        return Integer.toHexString(gravity);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        mFirstLayout = true;
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        mFirstLayout = true;
    }

    @SuppressLint("WrongConstant")
    @Override
    protected void onMeasure(final int widthMeasureSpec, final int heightMeasureSpec) {
        final int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        final int heightSize = MeasureSpec.getSize(heightMeasureSpec);

        setMeasuredDimension(widthSize, heightSize);

        boolean hasDrawerOnLeftEdge = false;
        boolean hasDrawerOnRightEdge = false;
        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = getChildAt(i);

            if (child.getVisibility() != GONE) {
                final LayoutParams lp = (LayoutParams) child.getLayoutParams();

                if (isContentView(child)) {
                    // Content views get measured at exactly the layout's size.
                    final int contentWidthSpec = MeasureSpec.makeMeasureSpec(widthSize - lp.leftMargin - lp.rightMargin, MeasureSpec.EXACTLY);
                    final int contentHeightSpec = MeasureSpec.makeMeasureSpec(heightSize - lp.topMargin - lp.bottomMargin, MeasureSpec.EXACTLY);
                    child.measure(contentWidthSpec, contentHeightSpec);

                } else if (isDrawerView(child)) {
                    if (Build.VERSION.SDK_INT >= 21 && ViewCompat.getElevation(child) != mDrawerElevation)
                        ViewCompat.setElevation(child, mDrawerElevation);
                    final int childGravity = getDrawerViewAbsoluteGravity(child) & Gravity.HORIZONTAL_GRAVITY_MASK;

                    final boolean isLeftEdgeDrawer = (childGravity == Gravity.LEFT);
                    if (isLeftEdgeDrawer && hasDrawerOnLeftEdge || !isLeftEdgeDrawer && hasDrawerOnRightEdge)
                        throw new IllegalStateException("Child drawer has absolute gravity " + gravityToString(childGravity)
                                + " but this MouseDrawer already has a drawer view along that edge");

                    if (isLeftEdgeDrawer) hasDrawerOnLeftEdge = true;
                    else hasDrawerOnRightEdge = true;

                    final int drawerWidthSpec = getChildMeasureSpec(widthMeasureSpec, lp.leftMargin + lp.rightMargin, lp.width);
                    final int drawerHeightSpec = getChildMeasureSpec(heightMeasureSpec, lp.topMargin + lp.bottomMargin, lp.height);
                    child.measure(drawerWidthSpec, drawerHeightSpec);
                } else
                    throw new IllegalStateException("Child " + child + " at index " + i
                            + " does not have a valid layout_gravity - must be Gravity.LEFT, Gravity.RIGHT or Gravity.NO_GRAVITY");
            }
        }
    }

    @Override
    protected void onLayout(final boolean changed, final int left, final int top, final int right, final int bottom) {
        mInLayout = true;
        final int width = right - left;
        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = getChildAt(i);

            if (child.getVisibility() != GONE) {
                final LayoutParams lp = (LayoutParams) child.getLayoutParams();

                if (isContentView(child)) {
                    child.layout(lp.leftMargin, lp.topMargin, lp.leftMargin + child.getMeasuredWidth(),
                            lp.topMargin + child.getMeasuredHeight());

                } else { // Drawer, if it wasn't onMeasure would have thrown an exception.
                    final int childWidth = child.getMeasuredWidth();
                    final int childHeight = child.getMeasuredHeight();
                    final int childLeft;
                    final float newOffset;

                    if (checkDrawerViewAbsoluteGravity(child, Gravity.LEFT)) {
                        childLeft = -childWidth + (int) (childWidth * lp.onScreen);
                        newOffset = (float) (childWidth + childLeft) / childWidth;
                    } else { // Right; onMeasure checked for us.
                        childLeft = width - (int) (childWidth * lp.onScreen);
                        newOffset = (float) (width - childLeft) / childWidth;
                    }

                    final boolean changeOffset = newOffset != lp.onScreen;

                    final int vgrav = lp.gravity & Gravity.VERTICAL_GRAVITY_MASK;
                    switch (vgrav) {
                        default:
                        case Gravity.TOP:
                            child.layout(childLeft, lp.topMargin, childLeft + childWidth, lp.topMargin + childHeight);
                            break;

                        case Gravity.BOTTOM: {
                            final int height = bottom - top;
                            child.layout(childLeft, height - lp.bottomMargin - child.getMeasuredHeight(),
                                    childLeft + childWidth, height - lp.bottomMargin);
                            break;
                        }

                        case Gravity.CENTER_VERTICAL: {
                            final int height = bottom - top;
                            int childTop = (height - childHeight) / 2;

                            if (childTop < lp.topMargin) childTop = lp.topMargin;
                            else if (childTop + childHeight > height - lp.bottomMargin)
                                childTop = height - lp.bottomMargin - childHeight;

                            child.layout(childLeft, childTop, childLeft + childWidth, childTop + childHeight);
                            break;
                        }
                    }

                    if (changeOffset) setDrawerViewOffset(child, newOffset);

                    final int newVisibility = lp.onScreen > 0 ? VISIBLE : INVISIBLE;
                    if (child.getVisibility() != newVisibility) child.setVisibility(newVisibility);
                }
            }
        }
        mInLayout = false;
        mFirstLayout = false;
    }

    @Override
    public void requestLayout() {
        if (!mInLayout) super.requestLayout();
    }

    @Override
    public void computeScroll() {
        final boolean leftDraggerSettling = mLeftDragger.continueSettling(true);
        final boolean rightDraggerSettling = mRightDragger.continueSettling(true);
        if (leftDraggerSettling || rightDraggerSettling) postInvalidateOnAnimation();
    }

    private static boolean hasOpaqueBackground(@NonNull final View v) {
        final Drawable bg = v.getBackground();
        if (bg != null) return bg.getOpacity() == PixelFormat.OPAQUE;
        return false;
    }

    @Override
    protected boolean drawChild(@NonNull final Canvas canvas, final View child, final long drawingTime) {
        final int height = getHeight();
        final boolean drawingContent = isContentView(child);
        int clipLeft = 0, clipRight = getWidth();

        final int restoreCount = canvas.save();
        if (drawingContent) {
            final int childCount = getChildCount();
            for (int i = 0; i < childCount; i++) {
                final View v = getChildAt(i);
                if (v != child && v.getVisibility() == VISIBLE && hasOpaqueBackground(v) && isDrawerView(v) && v.getHeight() >= height) {
                    if (checkDrawerViewAbsoluteGravity(v, Gravity.LEFT)) {
                        final int vright = v.getRight();
                        if (vright > clipLeft) clipLeft = vright;
                    } else {
                        final int vleft = v.getLeft();
                        if (vleft < clipRight) clipRight = vleft;
                    }
                }
            }
            canvas.clipRect(clipLeft, 0, clipRight, getHeight());
        }

        final boolean result = super.drawChild(canvas, child, drawingTime);
        canvas.restoreToCount(restoreCount);

        return result;
    }

    boolean isContentView(@NonNull final View child) {
        return ((LayoutParams) child.getLayoutParams()).gravity == Gravity.NO_GRAVITY;
    }

    boolean isDrawerView(@NonNull final View child) {
        final int gravity = ((LayoutParams) child.getLayoutParams()).gravity;
        final int absGravity = GravityCompat.getAbsoluteGravity(gravity, ViewCompat.getLayoutDirection(child));
        return (absGravity & Gravity.LEFT) != 0 || (absGravity & Gravity.RIGHT) != 0;
    }

    @Override
    public boolean onInterceptTouchEvent(@NonNull final MotionEvent ev) {
        final int action = ev.getActionMasked();

        // "|" used deliberately here; both methods should be invoked.
        final boolean interceptForDrag = mLeftDragger.shouldInterceptTouchEvent(ev) | mRightDragger.shouldInterceptTouchEvent(ev);

        switch (action) {
            case MotionEvent.ACTION_DOWN:
                mInitialMotionX = ev.getX();
                mInitialMotionY = ev.getY();
                break;

            case MotionEvent.ACTION_MOVE:
                mLeftDragger.checkTouchSlop(ViewDragHelper.DIRECTION_ALL);
                break;

            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP:
                closeDrawers(true);
        }

        return interceptForDrag || hasPeekingDrawer();
    }

    @Override
    public boolean dispatchGenericMotionEvent(@NonNull final MotionEvent event) {
        if ((event.getSource() & InputDevice.SOURCE_CLASS_POINTER) == 0 || event.getAction() == MotionEvent.ACTION_HOVER_EXIT)
            return super.dispatchGenericMotionEvent(event);

        final int childrenCount = getChildCount();
        if (childrenCount != 0) {
            final float x = event.getX();
            final float y = event.getY();

            // Walk through children from top to bottom.
            for (int i = childrenCount - 1; i >= 0; i--) {
                final View child = getChildAt(i);
                if (isInBoundsOfChild(x, y, child) && !isContentView(child) && dispatchTransformedGenericPointerEvent(event, child))
                    return true;
            }
        }

        return false;
    }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public boolean onTouchEvent(final MotionEvent ev) {
        mLeftDragger.processTouchEvent(ev);
        mRightDragger.processTouchEvent(ev);

        final int action = ev.getActionMasked();
        switch (action) {
            case MotionEvent.ACTION_DOWN:
                mInitialMotionX = ev.getX();
                mInitialMotionY = ev.getY();
                break;

            case MotionEvent.ACTION_UP:
                final float x = ev.getX();
                final float y = ev.getY();

                boolean peekingOnly = true;
                final View touchedView = mLeftDragger.findTopChildUnder((int) x, (int) y);
                if (touchedView != null && isContentView(touchedView)) {
                    final float dx = x - mInitialMotionX;
                    final float dy = y - mInitialMotionY;
                    final int slop = mLeftDragger.getTouchSlop();
                    if (dx * dx + dy * dy < slop * slop) {
                        // Taps close a dimmed open drawer but only if it isn't locked open.
                        final View openDrawer = findOpenDrawer();
                        if (openDrawer != null) peekingOnly = false;
                    }
                }
                closeDrawers(peekingOnly);
                break;

            case MotionEvent.ACTION_CANCEL:
                closeDrawers(true);
                break;
        }

        return true;
    }

    @Override
    public void requestDisallowInterceptTouchEvent(final boolean disallowIntercept) {
        if (CHILDREN_DISALLOW_INTERCEPT || (!mLeftDragger.isEdgeTouched(ViewDragHelper.EDGE_LEFT) && !mRightDragger.isEdgeTouched(ViewDragHelper.EDGE_RIGHT)))
            super.requestDisallowInterceptTouchEvent(disallowIntercept);
        if (disallowIntercept) closeDrawers(true);
    }

    public void closeDrawers() {
        closeDrawers(false);
    }

    void closeDrawers(final boolean peekingOnly) {
        boolean needsInvalidate = false;
        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = getChildAt(i);
            final LayoutParams lp = (LayoutParams) child.getLayoutParams();

            if (isDrawerView(child) && (!peekingOnly || lp.isPeeking)) {
                final int childWidth = child.getWidth();

                if (checkDrawerViewAbsoluteGravity(child, Gravity.LEFT))
                    needsInvalidate |= mLeftDragger.smoothSlideViewTo(child, -childWidth, child.getTop());
                else
                    needsInvalidate |= mRightDragger.smoothSlideViewTo(child, getWidth(), child.getTop());

                lp.isPeeking = false;
            }
        }

        if (needsInvalidate) invalidate();
    }

    public void openDrawer(@NonNull final View drawerView, final boolean animate) {
        if (isDrawerView(drawerView)) {
            final LayoutParams lp = (LayoutParams) drawerView.getLayoutParams();

            if (mFirstLayout) {
                lp.onScreen = 1.f;
                lp.openState = LayoutParams.FLAG_IS_OPENED;
            } else if (animate) {
                lp.openState |= LayoutParams.FLAG_IS_OPENING;

                if (checkDrawerViewAbsoluteGravity(drawerView, Gravity.LEFT))
                    mLeftDragger.smoothSlideViewTo(drawerView, 0, drawerView.getTop());
                else
                    mRightDragger.smoothSlideViewTo(drawerView, getWidth() - drawerView.getWidth(), drawerView.getTop());
            } else {
                moveDrawerToOffset(drawerView, 1.f);
                updateDrawerState(ViewDragHelper.STATE_IDLE, drawerView);
                drawerView.setVisibility(VISIBLE);
            }

            invalidate();
            return;
        }
        throw new IllegalArgumentException("View " + drawerView + " is not a sliding drawer");
    }

    public void openDrawer(@NonNull final View drawerView) {
        openDrawer(drawerView, true);
    }

    // public void openDrawer(@EdgeGravity final int gravity, final boolean animate) {
    //     final View drawerView = findDrawerWithGravity(gravity);
    //     if (drawerView != null) openDrawer(drawerView, animate);
    //     else throw new IllegalArgumentException("No drawer view found with gravity " + gravityToString(gravity));
    // }

    // public void openDrawer(@EdgeGravity final int gravity) {
    //     openDrawer(gravity, true);
    // }

    public void closeDrawer(@NonNull final View drawerView) {
        closeDrawer(drawerView, true);
    }

    public void closeDrawer(@NonNull final View drawerView, final boolean animate) {
        if (isDrawerView(drawerView)) {
            final LayoutParams lp = (LayoutParams) drawerView.getLayoutParams();
            if (mFirstLayout) {
                lp.onScreen = 0.f;
                lp.openState = 0;
            } else if (animate) {
                lp.openState |= LayoutParams.FLAG_IS_CLOSING;

                if (checkDrawerViewAbsoluteGravity(drawerView, Gravity.LEFT))
                    mLeftDragger.smoothSlideViewTo(drawerView, -drawerView.getWidth(), drawerView.getTop());
                else
                    mRightDragger.smoothSlideViewTo(drawerView, getWidth(), drawerView.getTop());
            } else {
                moveDrawerToOffset(drawerView, 0.f);
                updateDrawerState(ViewDragHelper.STATE_IDLE, drawerView);
                drawerView.setVisibility(INVISIBLE);
            }
            invalidate();
        } else throw new IllegalArgumentException("View " + drawerView + " is not a sliding drawer");
    }

    // public void closeDrawer(@EdgeGravity final int gravity) {
    //     closeDrawer(gravity, true);
    // }

    // public void closeDrawer(@EdgeGravity final int gravity, final boolean animate) {
    //     final View drawerView = findDrawerWithGravity(gravity);
    //     if (drawerView != null) closeDrawer(drawerView, animate);
    //     else throw new IllegalArgumentException("No drawer view found with gravity " + gravityToString(gravity));
    // }

    public boolean isDrawerOpen(@NonNull final View drawer) {
        if (isDrawerView(drawer)) return (((LayoutParams) drawer.getLayoutParams()).openState & LayoutParams.FLAG_IS_OPENED) == 1;
        else throw new IllegalArgumentException("View " + drawer + " is not a drawer");
    }

    // public boolean isDrawerOpen(@EdgeGravity final int drawerGravity) {
    //     final View drawerView = findDrawerWithGravity(drawerGravity);
    //     return drawerView != null && isDrawerOpen(drawerView);
    // }

    public boolean isDrawerVisible(@NonNull final View drawer) {
        if (isDrawerView(drawer)) return ((LayoutParams) drawer.getLayoutParams()).onScreen > 0;
        throw new IllegalArgumentException("View " + drawer + " is not a drawer");
    }

    // public boolean isDrawerVisible(@EdgeGravity final int drawerGravity) {
    //     final View drawerView = findDrawerWithGravity(drawerGravity);
    //     return drawerView != null && isDrawerVisible(drawerView);
    // }

    private boolean hasPeekingDrawer() {
        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final LayoutParams lp = (LayoutParams) getChildAt(i).getLayoutParams();
            if (lp.isPeeking) return true;
        }
        return false;
    }

    @Override
    protected ViewGroup.LayoutParams generateDefaultLayoutParams() {
        return new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
    }

    @Override
    protected ViewGroup.LayoutParams generateLayoutParams(final ViewGroup.LayoutParams params) {
        return params instanceof LayoutParams ? new LayoutParams((LayoutParams) params) :
                params instanceof ViewGroup.MarginLayoutParams ? new LayoutParams((MarginLayoutParams) params) : new LayoutParams(params);
    }

    @Override
    protected boolean checkLayoutParams(final ViewGroup.LayoutParams params) {
        return params instanceof LayoutParams && super.checkLayoutParams(params);
    }

    @Override
    public ViewGroup.LayoutParams generateLayoutParams(final AttributeSet attrs) {
        return new LayoutParams(getContext(), attrs);
    }

    @Override
    public void addFocusables(final ArrayList<View> views, final int direction, final int focusableMode) {
        if (getDescendantFocusability() != FOCUS_BLOCK_DESCENDANTS) {
            final int childCount = getChildCount();
            boolean isDrawerOpen = false;
            for (int i = 0; i < childCount; i++) {
                final View child = getChildAt(i);
                if (!isDrawerView(child)) mNonDrawerViews.add(child);
                else if (isDrawerOpen(child)) {
                    isDrawerOpen = true;
                    child.addFocusables(views, direction, focusableMode);
                }
            }

            if (!isDrawerOpen) {
                final int nonDrawerViewsCount = mNonDrawerViews.size();
                for (int i = 0; i < nonDrawerViewsCount; ++i) {
                    final View child = mNonDrawerViews.get(i);
                    if (child.getVisibility() == View.VISIBLE) child.addFocusables(views, direction, focusableMode);
                }
            }

            mNonDrawerViews.clear();
        }
    }

    private boolean hasVisibleDrawer() {
        return findVisibleDrawer() != null;
    }

    @Nullable
    final View findVisibleDrawer() {
        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = getChildAt(i);
            if (isDrawerView(child) && isDrawerVisible(child)) return child;
        }
        return null;
    }

    @Override
    public boolean onKeyDown(final int keyCode, final KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && hasVisibleDrawer()) {
            event.startTracking();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(final int keyCode, final KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            final View visibleDrawer = findVisibleDrawer();
            if (visibleDrawer != null && isDrawerView(visibleDrawer)) closeDrawers();
            return visibleDrawer != null;
        }
        return super.onKeyUp(keyCode, event);
    }

    @Override
    protected void onRestoreInstanceState(final Parcelable state) {
        if (state instanceof SavedState) {
            final SavedState ss = (SavedState) state;
            super.onRestoreInstanceState(ss.getSuperState());

            if (ss.openDrawerGravity != Gravity.NO_GRAVITY) {
                final View toOpen = findDrawerWithGravity(ss.openDrawerGravity);
                if (toOpen != null) openDrawer(toOpen);
            }
        } else super.onRestoreInstanceState(state);
    }

    @Override
    protected Parcelable onSaveInstanceState() {
        final Parcelable superState = super.onSaveInstanceState();
        assert superState != null;
        final SavedState ss = new SavedState(superState);

        final int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = getChildAt(i);
            final LayoutParams lp = (LayoutParams) child.getLayoutParams();
            // Is the current child fully opened (that is, not closing)?
            final boolean isOpenedAndNotClosing = (lp.openState == LayoutParams.FLAG_IS_OPENED);
            // Is the current child opening?
            final boolean isClosedAndOpening = (lp.openState == LayoutParams.FLAG_IS_OPENING);
            if (isOpenedAndNotClosing || isClosedAndOpening) {
                // If one of the conditions above holds, save the child's gravity so that we open that child during state restore.
                ss.openDrawerGravity = lp.gravity;
                break;
            }
        }

        return ss;
    }

    @Override
    public void addView(final View child, final int index, final ViewGroup.LayoutParams params) {
        super.addView(child, index, params);
        final View openDrawer = findOpenDrawer();
        if (openDrawer == null) isDrawerView(child);
    }

    protected static class SavedState extends AbsSavedState {
        public static final Creator<SavedState> CREATOR = new ClassLoaderCreator<SavedState>() {
            @NonNull
            @Override
            public SavedState createFromParcel(final Parcel in, final ClassLoader loader) {
                return new SavedState(in, loader);
            }

            @NonNull
            @Override
            public SavedState createFromParcel(final Parcel in) {
                return new SavedState(in, null);
            }

            @NonNull
            @Override
            public SavedState[] newArray(final int size) {
                return new SavedState[size];
            }
        };
        int openDrawerGravity = Gravity.NO_GRAVITY;

        public SavedState(@NonNull final Parcelable superState) {
            super(superState);
        }

        public SavedState(@NonNull final Parcel in, @Nullable final ClassLoader loader) {
            super(in, loader);
            openDrawerGravity = in.readInt();
        }

        @Override
        public void writeToParcel(final Parcel dest, final int flags) {
            super.writeToParcel(dest, flags);
            dest.writeInt(openDrawerGravity);
        }
    }

    private class ViewDragCallback extends ViewDragHelper.Callback {
        private final int mAbsGravity;
        private ViewDragHelper mDragger;

        ViewDragCallback(final int gravity) {
            mAbsGravity = gravity;
        }

        public void setDragger(final ViewDragHelper dragger) {
            mDragger = dragger;
        }

        @Override
        public boolean tryCaptureView(@NonNull final View child, final int pointerId) {
            return isDrawerView(child) && checkDrawerViewAbsoluteGravity(child, mAbsGravity);
        }

        @Override
        public void onViewDragStateChanged(final int state) {
            updateDrawerState(state, mDragger.getCapturedView());
        }

        @Override
        public void onViewPositionChanged(@NonNull final View changedView, final int left, final int top, final int dx, final int dy) {
            final float offset;
            final int childWidth = changedView.getWidth();

            if (checkDrawerViewAbsoluteGravity(changedView, Gravity.LEFT)) offset = (float) (childWidth + left) / childWidth;
            else offset = (float) (getWidth() - left) / childWidth;

            setDrawerViewOffset(changedView, offset);
            changedView.setVisibility(offset == 0 ? INVISIBLE : VISIBLE);
            invalidate();
        }

        @Override
        public void onViewCaptured(@NonNull final View capturedChild, final int activePointerId) {
            final LayoutParams lp = (LayoutParams) capturedChild.getLayoutParams();
            lp.isPeeking = false;
            closeOtherDrawer();
        }

        private void closeOtherDrawer() {
            final int otherGrav = mAbsGravity == Gravity.LEFT ? Gravity.RIGHT : Gravity.LEFT;
            final View toClose = findDrawerWithGravity(otherGrav);
            if (toClose != null) closeDrawer(toClose);
        }

        @Override
        public void onViewReleased(@NonNull final View releasedChild, final float xvel, final float yvel) {
            final float offset = getDrawerViewOffset(releasedChild);
            final int childWidth = releasedChild.getWidth();

            final int left;
            if (checkDrawerViewAbsoluteGravity(releasedChild, Gravity.LEFT))
                left = xvel > 0 || (xvel == 0 && offset > 0.5f) ? 0 : -childWidth;
            else {
                final int width = getWidth();
                left = xvel < 0 || (xvel == 0 && offset > 0.5f) ? width - childWidth : width;
            }

            mDragger.settleCapturedViewAt(left, releasedChild.getTop());
            invalidate();
        }

        @Override
        public void onEdgeDragStarted(final int edgeFlags, final int pointerId) {
            final View toCapture;
            if ((edgeFlags & ViewDragHelper.EDGE_LEFT) == ViewDragHelper.EDGE_LEFT)
                toCapture = findDrawerWithGravity(Gravity.LEFT);
            else toCapture = findDrawerWithGravity(Gravity.RIGHT);

            if (toCapture != null && isDrawerView(toCapture)) mDragger.captureChildView(toCapture, pointerId);
        }

        @Override
        public int getViewHorizontalDragRange(@NonNull final View child) {
            return isDrawerView(child) ? child.getWidth() : 0;
        }

        @Override
        public int clampViewPositionHorizontal(@NonNull final View child, final int left, final int dx) {
            if (checkDrawerViewAbsoluteGravity(child, Gravity.LEFT)) return Math.max(-child.getWidth(), Math.min(left, 0));
            final int width = getWidth();
            return Math.max(width - child.getWidth(), Math.min(left, width));
        }

        @Override
        public int clampViewPositionVertical(@NonNull final View child, final int top, final int dy) {
            return child.getTop();
        }
    }

    public static class LayoutParams extends ViewGroup.MarginLayoutParams {
        private static final int FLAG_IS_CLOSING = 0x4;
        public static final int FLAG_IS_OPENED = 0x1;
        public static final int FLAG_IS_OPENING = 0x2;
        public int openState;
        @EdgeGravity
        public int gravity = Gravity.NO_GRAVITY;
        public boolean isPeeking;
        public float onScreen;

        public LayoutParams(@NonNull final Context c, @Nullable final AttributeSet attrs) {
            super(c, attrs);
            final TypedArray a = c.obtainStyledAttributes(attrs, new int[]{android.R.attr.layout_gravity});
            try {
                this.gravity = a.getInt(0, Gravity.NO_GRAVITY);
            } finally {
                a.recycle();
            }
        }

        public LayoutParams(final int width, final int height) {
            super(width, height);
        }

        public LayoutParams(@NonNull final LayoutParams source) {
            super(source);
            this.gravity = source.gravity;
        }

        public LayoutParams(@NonNull final ViewGroup.LayoutParams source) {
            super(source);
        }

        public LayoutParams(@NonNull final ViewGroup.MarginLayoutParams source) {
            super(source);
        }
    }
}