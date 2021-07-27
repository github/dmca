package awais.instagrabber.customviews;

import android.content.Context;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewParent;

import androidx.annotation.NonNull;

public class VerticalDragHelper {
    // private static final String TAG = "VerticalDragHelper";
    private static final double SWIPE_THRESHOLD_VELOCITY = 80;

    private final View view;

    private GestureDetector gestureDetector;
    private Context context;
    private double flingVelocity;
    private OnVerticalDragListener onVerticalDragListener;

    private final GestureDetector.OnGestureListener gestureListener = new GestureDetector.SimpleOnGestureListener() {

        @Override
        public boolean onSingleTapConfirmed(final MotionEvent e) {
            view.performClick();
            return true;
        }

        @Override
        public boolean onFling(final MotionEvent e1, final MotionEvent e2, final float velocityX, final float velocityY) {
            double yDir = e1.getY() - e2.getY();
            // Log.d(TAG, "onFling: yDir: " + yDir);
            if (yDir < -SWIPE_THRESHOLD_VELOCITY || yDir > SWIPE_THRESHOLD_VELOCITY) {
                flingVelocity = yDir;
            }
            return super.onFling(e1, e2, velocityX, velocityY);
        }
    };

    private float prevRawY;
    private boolean isDragging;
    private float prevRawX;
    private float dX;
    private float prevDY;

    public VerticalDragHelper(@NonNull final View view) {
        this.view = view;
        final Context context = view.getContext();
        if (context == null) return;
        this.context = context;
        init();
    }

    public void setOnVerticalDragListener(@NonNull final OnVerticalDragListener onVerticalDragListener) {
        this.onVerticalDragListener = onVerticalDragListener;
    }

    protected void init() {
        gestureDetector = new GestureDetector(context, gestureListener);
    }

    public boolean onDragTouch(final MotionEvent event) {
        if (onVerticalDragListener == null) {
            return false;
        }
        if (gestureDetector.onTouchEvent(event)) {
            return true;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                return true;
            case MotionEvent.ACTION_MOVE:
                boolean handled = false;
                final float rawY = event.getRawY();
                final float dY = rawY - prevRawY;
                if (!isDragging) {
                    final float rawX = event.getRawX();
                    if (prevRawX != 0) {
                        dX = rawX - prevRawX;
                    }
                    prevRawX = rawX;
                    if (prevRawY != 0) {
                        final float dYAbs = Math.abs(dY - prevDY);
                        if (!isDragging && dYAbs < 50) {
                            final float abs = Math.abs(dY) - Math.abs(dX);
                            if (abs > 0) {
                                isDragging = true;
                            }
                        }
                    }
                }
                if (isDragging) {
                    final ViewParent parent = view.getParent();
                    parent.requestDisallowInterceptTouchEvent(true);
                    onVerticalDragListener.onDrag(dY);
                    handled = true;
                }
                prevDY = dY;
                prevRawY = rawY;
                return handled;
            case MotionEvent.ACTION_UP:
                // Log.d(TAG, "onDragTouch: reset prevRawY");
                prevRawY = 0;
                if (flingVelocity != 0) {
                    onVerticalDragListener.onFling(flingVelocity);
                    flingVelocity = 0;
                    isDragging = false;
                    return true;
                }
                if (isDragging) {
                    onVerticalDragListener.onDragEnd();
                    isDragging = false;
                    return true;
                }
                return false;
            default:
                return false;
        }
    }

    public boolean isDragging() {
        return isDragging;
    }

    public boolean onGestureTouchEvent(final MotionEvent event) {
        return gestureDetector.onTouchEvent(event);
    }

    public interface OnVerticalDragListener {
        void onDrag(final float dY);

        void onDragEnd();

        void onFling(final double flingVelocity);
    }
}
