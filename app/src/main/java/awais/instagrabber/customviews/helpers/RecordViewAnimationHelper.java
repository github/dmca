package awais.instagrabber.customviews.helpers;

import android.animation.AnimatorSet;
import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Handler;
import android.view.View;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;

import androidx.appcompat.widget.AppCompatImageView;
import androidx.vectordrawable.graphics.drawable.AnimatedVectorDrawableCompat;
import androidx.vectordrawable.graphics.drawable.AnimatorInflaterCompat;

import awais.instagrabber.R;
import awais.instagrabber.customviews.RecordButton;
import awais.instagrabber.customviews.RecordView.OnBasketAnimationEnd;

import static android.view.View.INVISIBLE;
import static android.view.View.VISIBLE;

public class RecordViewAnimationHelper {
    private static final String TAG = RecordViewAnimationHelper.class.getSimpleName();
    private final Context context;
    private final AnimatedVectorDrawableCompat animatedVectorDrawable;
    private final ImageView basketImg;
    private final ImageView smallBlinkingMic;
    private AlphaAnimation alphaAnimation;
    private OnBasketAnimationEnd onBasketAnimationEndListener;
    private boolean isBasketAnimating;
    private boolean isStartRecorded = false;
    private float micX = 0;
    private float micY = 0;
    private AnimatorSet micAnimation;
    private TranslateAnimation translateAnimation1, translateAnimation2;
    private Handler handler1, handler2;

    public RecordViewAnimationHelper(Context context, AppCompatImageView basketImg, AppCompatImageView smallBlinkingMic) {
        this.context = context;
        this.smallBlinkingMic = smallBlinkingMic;
        this.basketImg = basketImg;
        animatedVectorDrawable = AnimatedVectorDrawableCompat.create(context, R.drawable.recv_basket_animated);
    }

    @SuppressLint("RestrictedApi")
    public void animateBasket(float basketInitialY) {
        isBasketAnimating = true;

        clearAlphaAnimation(false);

        //save initial x,y values for mic icon
        if (micX == 0) {
            micX = smallBlinkingMic.getX();
            micY = smallBlinkingMic.getY();
        }

        micAnimation = (AnimatorSet) AnimatorInflaterCompat.loadAnimator(context, R.animator.delete_mic_animation);
        micAnimation.setTarget(smallBlinkingMic); // set the view you want to animate

        translateAnimation1 = new TranslateAnimation(0, 0, basketInitialY, basketInitialY - 90);
        translateAnimation1.setDuration(250);

        translateAnimation2 = new TranslateAnimation(0, 0, basketInitialY - 90, basketInitialY);
        translateAnimation2.setDuration(350);

        micAnimation.start();
        basketImg.setImageDrawable(animatedVectorDrawable);

        handler1 = new Handler();
        handler1.postDelayed(() -> {
            basketImg.setVisibility(VISIBLE);
            basketImg.startAnimation(translateAnimation1);
        }, 350);

        translateAnimation1.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {}

            @Override
            public void onAnimationEnd(Animation animation) {
                animatedVectorDrawable.start();
                handler2 = new Handler();
                handler2.postDelayed(() -> {
                    basketImg.startAnimation(translateAnimation2);
                    smallBlinkingMic.setVisibility(INVISIBLE);
                    basketImg.setVisibility(INVISIBLE);
                }, 450);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {}
        });

        translateAnimation2.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {}

            @Override
            public void onAnimationEnd(Animation animation) {
                basketImg.setVisibility(INVISIBLE);
                isBasketAnimating = false;
                //if the user pressed the record button while the animation is running
                // then do NOT call on Animation end
                if (onBasketAnimationEndListener != null && !isStartRecorded) {
                    onBasketAnimationEndListener.onAnimationEnd();
                }
            }

            @Override
            public void onAnimationRepeat(Animation animation) {}
        });
    }

    //if the user started a new Record while the Animation is running
    // then we want to stop the current animation and revert views back to default state
    public void resetBasketAnimation() {
        if (isBasketAnimating) {
            translateAnimation1.reset();
            translateAnimation1.cancel();
            translateAnimation2.reset();
            translateAnimation2.cancel();
            micAnimation.cancel();
            smallBlinkingMic.clearAnimation();
            basketImg.clearAnimation();
            if (handler1 != null) {
                handler1.removeCallbacksAndMessages(null);
            }
            if (handler2 != null) {
                handler2.removeCallbacksAndMessages(null);
            }
            basketImg.setVisibility(INVISIBLE);
            smallBlinkingMic.setX(micX);
            smallBlinkingMic.setY(micY);
            smallBlinkingMic.setVisibility(View.GONE);
            isBasketAnimating = false;
        }
    }

    public void clearAlphaAnimation(boolean hideView) {
        if (alphaAnimation != null) {
            alphaAnimation.cancel();
            alphaAnimation.reset();
        }
        smallBlinkingMic.clearAnimation();
        if (hideView) {
            smallBlinkingMic.setVisibility(View.GONE);
        }
    }

    public void animateSmallMicAlpha() {
        alphaAnimation = new AlphaAnimation(0.0f, 1.0f);
        alphaAnimation.setDuration(500);
        alphaAnimation.setRepeatMode(Animation.REVERSE);
        alphaAnimation.setRepeatCount(Animation.INFINITE);
        smallBlinkingMic.startAnimation(alphaAnimation);
    }

    public void moveRecordButtonAndSlideToCancelBack(final RecordButton recordBtn, View slideToCancelLayout, float initialX, float difX) {
        final ValueAnimator positionAnimator = ValueAnimator.ofFloat(recordBtn.getX(), initialX);
        positionAnimator.setInterpolator(new AccelerateDecelerateInterpolator());
        positionAnimator.addUpdateListener(animation -> {
            float x = (Float) animation.getAnimatedValue();
            recordBtn.setX(x);
        });
        recordBtn.stopScale();
        positionAnimator.setDuration(200);
        positionAnimator.start();

        // if the move event was not called ,then the difX will still 0 and there is no need to move it back
        if (difX != 0) {
            float x = initialX - difX;
            slideToCancelLayout.animate()
                               .x(x)
                               .setDuration(0)
                               .start();
        }
    }

    public void resetSmallMic() {
        smallBlinkingMic.setAlpha(1.0f);
        smallBlinkingMic.setScaleX(1.0f);
        smallBlinkingMic.setScaleY(1.0f);
    }

    public void setOnBasketAnimationEndListener(OnBasketAnimationEnd onBasketAnimationEndListener) {
        this.onBasketAnimationEndListener = onBasketAnimationEndListener;

    }

    public void onAnimationEnd() {
        if (onBasketAnimationEndListener != null) {
            onBasketAnimationEndListener.onAnimationEnd();
        }
    }

    //check if the user started a new Record by pressing the RecordButton
    public void setStartRecorded(boolean startRecorded) {
        isStartRecorded = startRecorded;
    }

}
