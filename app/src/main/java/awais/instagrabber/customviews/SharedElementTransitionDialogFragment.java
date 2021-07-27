package awais.instagrabber.customviews;

import android.animation.Animator;
import android.graphics.Rect;
import android.os.Handler;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.animation.AccelerateDecelerateInterpolator;

import androidx.annotation.NonNull;
import androidx.fragment.app.DialogFragment;
import androidx.transition.ChangeBounds;
import androidx.transition.ChangeTransform;
import androidx.transition.Transition;
import androidx.transition.TransitionListenerAdapter;
import androidx.transition.TransitionManager;
import androidx.transition.TransitionSet;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import awais.instagrabber.utils.Utils;

public abstract class SharedElementTransitionDialogFragment extends DialogFragment {
    // private static final String TAG = "SETDialogFragment";
    private static final int DURATION = 200;

    private final Map<Integer, View> startViews = new HashMap<>();
    private final Map<Integer, View> destViews = new HashMap<>();
    private final Map<Integer, ViewBounds> viewBoundsMap = new HashMap<>();
    private final List<Animator> additionalAnimators = new ArrayList<>();
    private final Handler initialBoundsHandler = new Handler();

    private boolean startCalled = false;
    private boolean startInitiated = false;
    private int boundsCalculatedCount = 0;

    protected int getAnimationDuration() {
        return DURATION;
    }

    public void addSharedElement(@NonNull final View startView, @NonNull final View destView) {
        final int key = destView.hashCode();
        startViews.put(key, startView);
        destViews.put(key, destView);
        setupInitialBounds(startView, destView);
        // final View view = getView();
        // if (view == null) return;
        // view.post(() -> {});
    }

    public void startPostponedEnterTransition() {
        startCalled = true;
        if (startInitiated) return;
        if (boundsCalculatedCount < startViews.size()) return;
        startInitiated = true;
        final Set<Integer> keySet = startViews.keySet();
        final View view = getView();
        if (!(view instanceof ViewGroup)) return;
        final TransitionSet transitionSet = new TransitionSet()
                .setDuration(DURATION)
                .setInterpolator(new AccelerateDecelerateInterpolator())
                .addTransition(new ChangeBounds())
                .addTransition(new ChangeTransform())
                .addListener(new TransitionListenerAdapter() {
                    @Override
                    public void onTransitionStart(@NonNull final Transition transition) {
                        for (Animator animator : additionalAnimators) {
                            animator.start();
                        }
                    }

                    @Override
                    public void onTransitionEnd(@NonNull final Transition transition) {
                        for (final Integer key : keySet) {
                            final View startView = startViews.get(key);
                            final View destView = destViews.get(key);
                            final ViewBounds viewBounds = viewBoundsMap.get(key);
                            if (startView == null || destView == null || viewBounds == null) return;
                            onEndSharedElementAnimation(startView, destView, viewBounds);
                        }
                    }
                });
        view.post(() -> {
            TransitionManager.beginDelayedTransition((ViewGroup) view, transitionSet);
            for (final Integer key : keySet) {
                final View startView = startViews.get(key);
                final View destView = destViews.get(key);
                final ViewBounds viewBounds = viewBoundsMap.get(key);
                if (startView == null || destView == null || viewBounds == null) return;
                onBeforeSharedElementAnimation(startView, destView, viewBounds);
                setDestBounds(key);
            }
        });
    }

    private void setDestBounds(final int key) {
        final View startView = startViews.get(key);
        if (startView == null) return;
        final View destView = destViews.get(key);
        if (destView == null) return;
        final ViewBounds viewBounds = viewBoundsMap.get(key);
        if (viewBounds == null) return;
        destView.setX((int) viewBounds.getDestX());
        destView.setY((int) viewBounds.getDestY());
        destView.setTranslationX(0);
        destView.setTranslationY(0);
        final ViewGroup.LayoutParams layoutParams = destView.getLayoutParams();
        layoutParams.height = viewBounds.getDestHeight();
        layoutParams.width = viewBounds.getDestWidth();
        destView.requestLayout();
    }

    protected void onBeforeSharedElementAnimation(@NonNull final View startView,
                                                  @NonNull final View destView,
                                                  @NonNull final ViewBounds viewBounds) {}

    protected void onEndSharedElementAnimation(@NonNull final View startView,
                                               @NonNull final View destView,
                                               @NonNull final ViewBounds viewBounds) {}

    private void setupInitialBounds(@NonNull final View startView, @NonNull final View destView) {
        final ViewTreeObserver.OnPreDrawListener preDrawListener = new ViewTreeObserver.OnPreDrawListener() {
            private boolean firstPassDone;

            @Override
            public boolean onPreDraw() {
                destView.getViewTreeObserver().removeOnPreDrawListener(this);
                if (!firstPassDone) {
                    getViewBounds(startView, destView, this);
                    firstPassDone = true;
                    return false;
                }
                final int[] location = new int[2];
                startView.getLocationOnScreen(location);
                final int initX = location[0];
                final int initY = location[1];
                destView.setX(initX);
                destView.setY(initY - Utils.getStatusBarHeight(getContext()));
                boundsCalculatedCount++;
                if (startCalled) {
                    startPostponedEnterTransition();
                }
                return true;
            }
        };
        destView.getViewTreeObserver().addOnPreDrawListener(preDrawListener);
    }

    private void getViewBounds(@NonNull final View startView,
                               @NonNull final View destView,
                               @NonNull final ViewTreeObserver.OnPreDrawListener preDrawListener) {
        final ViewBounds viewBounds = new ViewBounds();
        viewBounds.setDestWidth(destView.getWidth());
        viewBounds.setDestHeight(destView.getHeight());
        viewBounds.setDestX(destView.getX());
        viewBounds.setDestY(destView.getY());

        final Rect destBounds = new Rect();
        destView.getDrawingRect(destBounds);
        viewBounds.setDestBounds(destBounds);

        final ViewGroup.LayoutParams layoutParams = destView.getLayoutParams();

        final Rect startBounds = new Rect();
        startView.getDrawingRect(startBounds);
        viewBounds.setStartBounds(startBounds);

        final int key = destView.hashCode();
        viewBoundsMap.put(key, viewBounds);

        layoutParams.height = startView.getHeight();
        layoutParams.width = startView.getWidth();

        destView.getViewTreeObserver().addOnPreDrawListener(preDrawListener);
        destView.requestLayout();
    }

    // private void animateBounds(@NonNull final View startView,
    //                            @NonNull final View destView,
    //                            @NonNull final ViewBounds viewBounds) {
    //     final ValueAnimator heightAnimator = ObjectAnimator.ofInt(startView.getHeight(), viewBounds.getDestHeight());
    //     final ValueAnimator widthAnimator = ObjectAnimator.ofInt(startView.getWidth(), viewBounds.getDestWidth());
    //     heightAnimator.setDuration(DURATION);
    //     widthAnimator.setDuration(DURATION);
    //     additionalAnimators.add(heightAnimator);
    //     additionalAnimators.add(widthAnimator);
    //     heightAnimator.addUpdateListener(animation -> {
    //         ViewGroup.LayoutParams params = destView.getLayoutParams();
    //         params.height = (int) animation.getAnimatedValue();
    //         destView.requestLayout();
    //     });
    //     widthAnimator.addUpdateListener(animation -> {
    //         ViewGroup.LayoutParams params = destView.getLayoutParams();
    //         params.width = (int) animation.getAnimatedValue();
    //         destView.requestLayout();
    //     });
    //     onBeforeSharedElementAnimation(startView, destView, viewBounds);
    //     final float destX = viewBounds.getDestX();
    //     final float destY = viewBounds.getDestY();
    //     final AnimatorSet animatorSet = new AnimatorSet();
    //     animatorSet.addListener(new AnimatorListenerAdapter() {
    //         @Override
    //         public void onAnimationEnd(final Animator animation) {
    //             animationEnded(startView, destView, viewBounds);
    //         }
    //     });
    //
    //     destView.animate()
    //             .x(destX)
    //             .y(destY)
    //             .setDuration(DURATION)
    //             .withStartAction(() -> {
    //                 if (!additionalAnimatorsStarted && additionalAnimators.size() > 0) {
    //                     additionalAnimatorsStarted = true;
    //                     animatorSet.playTogether(additionalAnimators);
    //                     animatorSet.start();
    //                 }
    //             })
    //             .withEndAction(() -> animationEnded(startView, destView, viewBounds))
    //             .start();
    // }

    // private int endCount = 0;
    // private void animationEnded(final View startView, final View destView, final ViewBounds viewBounds) {
    //     ++endCount;
    //     if (endCount != startViews.size() + 1) return;
    //     onEndSharedElementAnimation(startView, destView, viewBounds);
    // }

    protected void addAnimator(@NonNull final Animator animator) {
        additionalAnimators.add(animator);
    }

    protected static class ViewBounds {
        private float destY;
        private float destX;
        private int destHeight;
        private int destWidth;
        private Rect startBounds;
        private Rect destBounds;

        public ViewBounds() {}

        public float getDestY() {
            return destY;
        }

        public void setDestY(final float destY) {
            this.destY = destY;
        }

        public float getDestX() {
            return destX;
        }

        public void setDestX(final float destX) {
            this.destX = destX;
        }

        public int getDestHeight() {
            return destHeight;
        }

        public void setDestHeight(final int destHeight) {
            this.destHeight = destHeight;
        }

        public int getDestWidth() {
            return destWidth;
        }

        public void setDestWidth(final int destWidth) {
            this.destWidth = destWidth;
        }

        public Rect getStartBounds() {
            return startBounds;
        }

        public void setStartBounds(final Rect startBounds) {
            this.startBounds = startBounds;
        }

        public Rect getDestBounds() {
            return destBounds;
        }

        public void setDestBounds(final Rect destBounds) {
            this.destBounds = destBounds;
        }
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        startViews.clear();
        destViews.clear();
        viewBoundsMap.clear();
        additionalAnimators.clear();
    }
}
