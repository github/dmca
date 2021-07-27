package awais.instagrabber.animations;

import android.view.View;
import android.view.animation.Animation;
import android.view.animation.Transformation;

public class ResizeAnimation extends Animation {
    private static final String TAG = "ResizeAnimation";

    final View view;
    final int startHeight;
    final int targetHeight;
    final int startWidth;
    final int targetWidth;

    public ResizeAnimation(final View view,
                           final int startHeight,
                           final int startWidth,
                           final int targetHeight,
                           final int targetWidth) {
        this.view = view;
        this.startHeight = startHeight;
        this.targetHeight = targetHeight;
        this.startWidth = startWidth;
        this.targetWidth = targetWidth;
    }

    @Override
    protected void applyTransformation(final float interpolatedTime, final Transformation t) {
        // Log.d(TAG, "applyTransformation: interpolatedTime: " + interpolatedTime);
        view.getLayoutParams().height = (int) (startHeight + (targetHeight - startHeight) * interpolatedTime);
        view.getLayoutParams().width = (int) (startWidth + (targetWidth - startWidth) * interpolatedTime);
        view.requestLayout();
    }

    @Override
    public void initialize(final int width, final int height, final int parentWidth, final int parentHeight) {
        super.initialize(width, height, parentWidth, parentHeight);
    }

    @Override
    public boolean willChangeBounds() {
        return true;
    }
}
