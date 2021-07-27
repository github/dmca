package awais.instagrabber.customviews.helpers;

import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;

import awais.instagrabber.interfaces.SwipeEvent;

public final class SwipeGestureListener extends GestureDetector.SimpleOnGestureListener {
    public static final int SWIPE_THRESHOLD = 200;
    public static final int SWIPE_VELOCITY_THRESHOLD = 200;
    private final SwipeEvent swipeEvent;

    public SwipeGestureListener(final SwipeEvent swipeEvent) {
        this.swipeEvent = swipeEvent;
    }

    @Override
    public boolean onFling(final MotionEvent e1, final MotionEvent e2, final float velocityX, final float velocityY) {
        try {
            final float diffY = e2.getY() - e1.getY();
            final float diffX = e2.getX() - e1.getX();
            final float diffXAbs = Math.abs(diffX);
            if (diffXAbs > Math.abs(diffY) && diffXAbs > SWIPE_THRESHOLD && Math.abs(velocityX) > SWIPE_VELOCITY_THRESHOLD) {
                if (diffX > 0) swipeEvent.onSwipe(true);
                else swipeEvent.onSwipe(false);
                return true;
            }
        } catch (final Exception e) {
            Log.e("AWAISKING_APP", "", e);
        }
        return false;
    }
}
