/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package awais.instagrabber.customviews.helpers;

import android.os.CancellationSignal;
import android.util.Log;
import android.view.View;
import android.view.animation.LinearInterpolator;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsAnimationControlListenerCompat;
import androidx.core.view.WindowInsetsAnimationControllerCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.dynamicanimation.animation.FloatPropertyCompat;
import androidx.dynamicanimation.animation.SpringAnimation;
import androidx.dynamicanimation.animation.SpringForce;

import awais.instagrabber.utils.ViewUtils;

/**
 * A wrapper around the [WindowInsetsAnimationControllerCompat] APIs in AndroidX Core, to simplify
 * the implementation of common use-cases around the IME.
 * <p>
 * See [InsetsAnimationLinearLayout] and [InsetsAnimationTouchListener] for examples of how
 * to use this class.
 */
public class SimpleImeAnimationController {
    private static final String TAG = SimpleImeAnimationController.class.getSimpleName();
    /**
     * Scroll threshold for determining whether to animating to the end state, or to the start state.
     * Currently 15% of the total swipe distance distance
     */
    private static final float SCROLL_THRESHOLD = 0.15f;

    @Nullable
    private WindowInsetsAnimationControllerCompat insetsAnimationController = null;
    @Nullable
    private CancellationSignal pendingRequestCancellationSignal = null;
    @Nullable
    private OnRequestReadyListener pendingRequestOnReadyListener;
    /**
     * True if the IME was shown at the start of the current animation.
     */
    private boolean isImeShownAtStart = false;
    @Nullable
    private SpringAnimation currentSpringAnimation = null;
    private WindowInsetsAnimationControlListenerCompat fwdListener;

    /**
     * A LinearInterpolator instance we can re-use across listeners.
     */
    private final LinearInterpolator linearInterpolator = new LinearInterpolator();
    /* To take control of the an WindowInsetsAnimation, we need to pass in a listener to
       controlWindowInsetsAnimation() in startControlRequest(). The listener created here
       keeps track of the current WindowInsetsAnimationController and resets our state. */
    private final WindowInsetsAnimationControlListenerCompat animationControlListener = new WindowInsetsAnimationControlListenerCompat() {
        /**
         * Once the request is ready, call our [onRequestReady] function
         */
        @Override
        public void onReady(@NonNull final WindowInsetsAnimationControllerCompat controller, final int types) {
            onRequestReady(controller);
            if (fwdListener != null) {
                fwdListener.onReady(controller, types);
            }
        }

        /**
         * If the request is finished, we should reset our internal state
         */
        @Override
        public void onFinished(@NonNull final WindowInsetsAnimationControllerCompat controller) {
            reset();
            if (fwdListener != null) {
                fwdListener.onFinished(controller);
            }
        }

        /**
         * If the request is cancelled, we should reset our internal state
         */
        @Override
        public void onCancelled(@Nullable final WindowInsetsAnimationControllerCompat controller) {
            reset();
            if (fwdListener != null) {
                fwdListener.onCancelled(controller);
            }
        }
    };

    /**
     * Start a control request to the [view]s [android.view.WindowInsetsController]. This should
     * be called once the view is in a position to take control over the position of the IME.
     *
     * @param view                   The view which is triggering this request
     * @param onRequestReadyListener optional listener which will be called when the request is ready and
     *                               the animation can proceed
     */
    public void startControlRequest(@NonNull final View view,
                                    @Nullable final OnRequestReadyListener onRequestReadyListener) {
        if (isInsetAnimationInProgress()) {
            Log.w(TAG, "startControlRequest: Animation in progress. Can not start a new request to controlWindowInsetsAnimation()");
            return;
        }

        // Keep track of the IME insets, and the IME visibility, at the start of the request
        final WindowInsetsCompat rootWindowInsets = ViewCompat.getRootWindowInsets(view);
        if (rootWindowInsets != null) {
            isImeShownAtStart = rootWindowInsets.isVisible(WindowInsetsCompat.Type.ime());
        }

        // Create a cancellation signal, which we pass to controlWindowInsetsAnimation() below
        pendingRequestCancellationSignal = new CancellationSignal();
        // Keep reference to the onReady callback
        pendingRequestOnReadyListener = onRequestReadyListener;

        // Finally we make a controlWindowInsetsAnimation() request:
        final WindowInsetsControllerCompat windowInsetsController = ViewCompat.getWindowInsetsController(view);
        if (windowInsetsController != null) {
            windowInsetsController.controlWindowInsetsAnimation(
                    // We're only catering for IME animations in this listener
                    WindowInsetsCompat.Type.ime(),
                    // Animation duration. This is not used by the system, and is only passed to any
                    // WindowInsetsAnimation.Callback set on views. We pass in -1 to indicate that we're
                    // not starting a finite animation, and that this is completely controlled by
                    // the user's touch.
                    -1,
                    // The time interpolator used in calculating the animation progress. The fraction value
                    // we passed into setInsetsAndAlpha() which be passed into this interpolator before
                    // being used by the system to inset the IME. LinearInterpolator is a good type
                    // to use for scrolling gestures.
                    linearInterpolator,
                    // A cancellation signal, which allows us to cancel the request to control
                    pendingRequestCancellationSignal,
                    // The WindowInsetsAnimationControlListener
                    animationControlListener
            );
        }
    }

    /**
     * Start a control request to the [view]s [android.view.WindowInsetsController], similar to
     * [startControlRequest], but immediately fling to a finish using [velocityY] once ready.
     * <p>
     * This function is useful for fire-and-forget operations to animate the IME.
     *
     * @param view      The view which is triggering this request
     * @param velocityY the velocity of the touch gesture which caused this call
     */
    public void startAndFling(@NonNull final View view, final float velocityY) {
        startControlRequest(view, null);
        animateToFinish(velocityY);
    }

    /**
     * Update the inset position of the IME by the given [dy] value. This value will be coerced
     * into the hidden and shown inset values.
     * <p>
     * This function should only be called if [isInsetAnimationInProgress] returns true.
     *
     * @return the amount of [dy] consumed by the inset animation, in pixels
     */
    public int insetBy(final int dy) {
        if (insetsAnimationController == null) {
            throw new IllegalStateException("Current WindowInsetsAnimationController is null." +
                                                    "This should only be called if isAnimationInProgress() returns true");
        }
        final WindowInsetsAnimationControllerCompat controller = insetsAnimationController;

        // Call updateInsetTo() with the new inset value
        return insetTo(controller.getCurrentInsets().bottom - dy);
    }

    /**
     * Update the inset position of the IME to be the given [inset] value. This value will be
     * coerced into the hidden and shown inset values.
     * <p>
     * This function should only be called if [isInsetAnimationInProgress] returns true.
     *
     * @return the distance moved by the inset animation, in pixels
     */
    public int insetTo(final int inset) {
        if (insetsAnimationController == null) {
            throw new IllegalStateException("Current WindowInsetsAnimationController is null." +
                                                    "This should only be called if isAnimationInProgress() returns true");
        }
        final WindowInsetsAnimationControllerCompat controller = insetsAnimationController;

        final int hiddenBottom = controller.getHiddenStateInsets().bottom;
        final int shownBottom = controller.getShownStateInsets().bottom;
        final int startBottom = isImeShownAtStart ? shownBottom : hiddenBottom;
        final int endBottom = isImeShownAtStart ? hiddenBottom : shownBottom;

        // We coerce the given inset within the limits of the hidden and shown insets
        final int coercedBottom = coerceIn(inset, hiddenBottom, shownBottom);

        final int consumedDy = controller.getCurrentInsets().bottom - coercedBottom;

        // Finally update the insets in the WindowInsetsAnimationController using
        // setInsetsAndAlpha().
        controller.setInsetsAndAlpha(
                // Here we update the animating insets. This is what controls where the IME is displayed.
                // It is also passed through to views via their WindowInsetsAnimation.Callback.
                Insets.of(0, 0, 0, coercedBottom),
                // This controls the alpha value. We don't want to alter the alpha so use 1f
                1f,
                // Finally we calculate the animation progress fraction. This value is passed through
                // to any WindowInsetsAnimation.Callbacks, but it is not used by the system.
                (coercedBottom - startBottom) / (float) (endBottom - startBottom)
        );

        return consumedDy;
    }

    /**
     * Return `true` if an inset animation is in progress.
     */
    public boolean isInsetAnimationInProgress() {
        return insetsAnimationController != null;
    }

    /**
     * Return `true` if an inset animation is currently finishing.
     */
    public boolean isInsetAnimationFinishing() {
        return currentSpringAnimation != null;
    }

    /**
     * Return `true` if a request to control an inset animation is in progress.
     */
    public boolean isInsetAnimationRequestPending() {
        return pendingRequestCancellationSignal != null;
    }

    /**
     * Cancel the current [WindowInsetsAnimationControllerCompat]. We immediately finish
     * the animation, reverting back to the state at the start of the gesture.
     */
    public void cancel() {
        if (insetsAnimationController != null) {
            insetsAnimationController.finish(isImeShownAtStart);
        }
        if (pendingRequestCancellationSignal != null) {
            pendingRequestCancellationSignal.cancel();
        }
        if (currentSpringAnimation != null) {
            // Cancel the current spring animation
            currentSpringAnimation.cancel();
        }
        reset();
    }

    /**
     * Finish the current [WindowInsetsAnimationControllerCompat] immediately.
     */
    public void finish() {
        final WindowInsetsAnimationControllerCompat controller = insetsAnimationController;

        if (controller == null) {
            // If we don't currently have a controller, cancel any pending request and return
            if (pendingRequestCancellationSignal != null) {
                pendingRequestCancellationSignal.cancel();
            }
            return;
        }

        final int current = controller.getCurrentInsets().bottom;
        final int shown = controller.getShownStateInsets().bottom;
        final int hidden = controller.getHiddenStateInsets().bottom;

        // The current inset matches either the shown/hidden inset, finish() immediately
        if (current == shown) {
            controller.finish(true);
        } else if (current == hidden) {
            controller.finish(false);
        } else {
            // Otherwise, we'll look at the current position...
            if (controller.getCurrentFraction() >= SCROLL_THRESHOLD) {
                // If the IME is past the 'threshold' we snap to the toggled state
                controller.finish(!isImeShownAtStart);
            } else {
                // ...otherwise, we snap back to the original visibility
                controller.finish(isImeShownAtStart);
            }
        }
    }

    /**
     * Finish the current [WindowInsetsAnimationControllerCompat]. We finish the animation,
     * animating to the end state if necessary.
     *
     * @param velocityY the velocity of the touch gesture which caused this call to [animateToFinish].
     *                  Can be `null` if velocity is not available.
     */
    public void animateToFinish(@Nullable final Float velocityY) {
        final WindowInsetsAnimationControllerCompat controller = insetsAnimationController;

        if (controller == null) {
            // If we don't currently have a controller, cancel any pending request and return
            if (pendingRequestCancellationSignal != null) {
                pendingRequestCancellationSignal.cancel();
            }
            return;
        }

        final int current = controller.getCurrentInsets().bottom;
        final int shown = controller.getShownStateInsets().bottom;
        final int hidden = controller.getHiddenStateInsets().bottom;

        if (velocityY != null) {
            // If we have a velocity, we can use it's direction to determine
            // the visibility. Upwards == visible
            animateImeToVisibility(velocityY > 0, velocityY);
        } else if (current == shown) {
            // The current inset matches either the shown/hidden inset, finish() immediately
            controller.finish(true);
        } else if (current == hidden) {
            controller.finish(false);
        } else {
            // Otherwise, we'll look at the current position...
            if (controller.getCurrentFraction() >= SCROLL_THRESHOLD) {
                // If the IME is past the 'threshold' we animate it to the toggled state
                animateImeToVisibility(!isImeShownAtStart, null);
            } else {
                // ...otherwise, we animate it back to the original visibility
                animateImeToVisibility(isImeShownAtStart, null);
            }
        }
    }

    private void onRequestReady(@NonNull final WindowInsetsAnimationControllerCompat controller) {
        // The request is ready, so clear out the pending cancellation signal
        pendingRequestCancellationSignal = null;
        // Store the current WindowInsetsAnimationController
        insetsAnimationController = controller;

        // Call any pending callback
        if (pendingRequestOnReadyListener != null) {
            pendingRequestOnReadyListener.onRequestReady(controller);
        }
        pendingRequestOnReadyListener = null;
    }

    /**
     * Resets all of our internal state.
     */
    private void reset() {
        // Clear all of our internal state
        insetsAnimationController = null;
        pendingRequestCancellationSignal = null;
        isImeShownAtStart = false;
        if (currentSpringAnimation != null) {
            currentSpringAnimation.cancel();
        }
        currentSpringAnimation = null;
        pendingRequestOnReadyListener = null;
    }

    /**
     * Animate the IME to a given visibility.
     *
     * @param visible   `true` to animate the IME to it's fully shown state, `false` to it's
     *                  fully hidden state.
     * @param velocityY the velocity of the touch gesture which caused this call. Can be `null`
     *                  if velocity is not available.
     */
    private void animateImeToVisibility(final boolean visible, @Nullable final Float velocityY) {
        if (insetsAnimationController == null) {
            throw new IllegalStateException("Controller should not be null");
        }
        final WindowInsetsAnimationControllerCompat controller = insetsAnimationController;

        final FloatPropertyCompat<Object> property = new FloatPropertyCompat<Object>("property") {
            @Override
            public float getValue(final Object object) {
                return controller.getCurrentInsets().bottom;
            }

            @Override
            public void setValue(final Object object, final float value) {
                if (insetsAnimationController == null) {
                    return;
                }
                insetTo((int) value);
            }
        };
        final float finalPosition = visible ? controller.getShownStateInsets().bottom
                                            : controller.getHiddenStateInsets().bottom;
        final SpringForce force = new SpringForce(finalPosition)
                // Tweak the damping value, to remove any bounciness.
                .setDampingRatio(SpringForce.DAMPING_RATIO_NO_BOUNCY)
                // The stiffness value controls the strength of the spring animation, which
                // controls the speed. Medium (the default) is a good value, but feel free to
                // play around with this value.
                .setStiffness(SpringForce.STIFFNESS_MEDIUM);
        ViewUtils.springAnimationOf(this, property, finalPosition)
                 .setSpring(force)
                 .setStartVelocity(velocityY != null ? velocityY : 0)
                 .addEndListener((animation, canceled, value, velocity) -> {
                     if (animation == currentSpringAnimation) {
                         currentSpringAnimation = null;
                     }
                     // Once the animation has ended, finish the controller
                     finish();
                 }).start();
    }

    private int coerceIn(final int v, final int min, final int max) {
        if (v >= min && v <= max) {
            return v;
        }
        if (v < min) {
            return min;
        }
        return max;
    }

    public void setAnimationControlListener(final WindowInsetsAnimationControlListenerCompat listener) {
        fwdListener = listener;
    }

    public interface OnRequestReadyListener {
        void onRequestReady(WindowInsetsAnimationControllerCompat windowInsetsAnimationControllerCompat);
    }
}
