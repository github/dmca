package awais.instagrabber.animations;

import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.view.View;
import android.view.animation.AccelerateDecelerateInterpolator;

public class ScaleAnimation {

    private final View view;

    public ScaleAnimation(View view) {
        this.view = view;
    }


    public void start() {
        AnimatorSet set = new AnimatorSet();
        ObjectAnimator scaleY = ObjectAnimator.ofFloat(view, "scaleY", 2.0f);

        ObjectAnimator scaleX = ObjectAnimator.ofFloat(view, "scaleX", 2.0f);
        set.setDuration(150);
        set.setInterpolator(new AccelerateDecelerateInterpolator());
        set.playTogether(scaleY, scaleX);
        set.start();
    }

    public void stop() {
        AnimatorSet set = new AnimatorSet();
        ObjectAnimator scaleY = ObjectAnimator.ofFloat(view, "scaleY", 1.0f);
        //        scaleY.setDuration(250);
        //        scaleY.setInterpolator(new DecelerateInterpolator());


        ObjectAnimator scaleX = ObjectAnimator.ofFloat(view, "scaleX", 1.0f);
        //        scaleX.setDuration(250);
        //        scaleX.setInterpolator(new DecelerateInterpolator());


        set.setDuration(150);
        set.setInterpolator(new AccelerateDecelerateInterpolator());
        set.playTogether(scaleY, scaleX);
        set.start();
    }
}
