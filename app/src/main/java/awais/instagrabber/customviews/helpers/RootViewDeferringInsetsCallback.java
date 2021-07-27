package awais.instagrabber.customviews.helpers;/*
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

import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.graphics.Insets;
import androidx.core.view.OnApplyWindowInsetsListener;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsAnimationCompat;
import androidx.core.view.WindowInsetsCompat;

import java.util.List;

/**
 * A class which extends/implements both [WindowInsetsAnimationCompat.Callback] and
 * [View.OnApplyWindowInsetsListener], which should be set on the root view in your layout.
 * <p>
 * This class enables the root view is selectively defer handling any insets which match
 * [deferredInsetTypes], to enable better looking [WindowInsetsAnimationCompat]s.
 * <p>
 * An example is the following: when a [WindowInsetsAnimationCompat] is started, the system will dispatch
 * a [WindowInsetsCompat] instance which contains the end state of the animation. For the scenario of
 * the IME being animated in, that means that the insets contains the IME height. If the view's
 * [View.OnApplyWindowInsetsListener] simply always applied the combination of
 * [WindowInsetsCompat.Type.ime] and [WindowInsetsCompat.Type.systemBars] using padding, the viewport of any
 * child views would then be smaller. This results in us animating a smaller (padded-in) view into
 * a larger viewport. Visually, this results in the views looking clipped.
 * <p>
 * This class allows us to implement a different strategy for the above scenario, by selectively
 * deferring the [WindowInsetsCompat.Type.ime] insets until the [WindowInsetsAnimationCompat] is ended.
 * For the above example, you would create a [RootViewDeferringInsetsCallback] like so:
 * <p>
 * ```
 * val callback = RootViewDeferringInsetsCallback(
 * persistentInsetTypes = WindowInsetsCompat.Type.systemBars(),
 * deferredInsetTypes = WindowInsetsCompat.Type.ime()
 * )
 * ```
 * <p>
 * This class is not limited to just IME animations, and can work with any [WindowInsetsCompat.Type]s.
 */
public class RootViewDeferringInsetsCallback extends WindowInsetsAnimationCompat.Callback implements OnApplyWindowInsetsListener {

    private final int persistentInsetTypes;
    private final int deferredInsetTypes;
    @Nullable
    private View view = null;
    @Nullable
    private WindowInsetsCompat lastWindowInsets = null;
    private boolean deferredInsets = false;

    /**
     * @param persistentInsetTypes the bitmask of any inset types which should always be handled
     *                             through padding the attached view
     * @param deferredInsetTypes   the bitmask of insets types which should be deferred until after
     *                             any related [WindowInsetsAnimationCompat]s have ended
     */
    public RootViewDeferringInsetsCallback(final int persistentInsetTypes, final int deferredInsetTypes) {
        super(DISPATCH_MODE_CONTINUE_ON_SUBTREE);
        if ((persistentInsetTypes & deferredInsetTypes) != 0) {
            throw new IllegalArgumentException("persistentInsetTypes and deferredInsetTypes can not contain " +
                                                       "any of same WindowInsetsCompat.Type values");
        }
        this.persistentInsetTypes = persistentInsetTypes;
        this.deferredInsetTypes = deferredInsetTypes;
    }

    @Override
    public WindowInsetsCompat onApplyWindowInsets(@NonNull final View v, @NonNull final WindowInsetsCompat windowInsets) {
        // Store the view and insets for us in onEnd() below
        view = v;
        lastWindowInsets = windowInsets;

        final int types = deferredInsets
                          // When the deferred flag is enabled, we only use the systemBars() insets
                          ? persistentInsetTypes
                          // Otherwise we handle the combination of the the systemBars() and ime() insets
                          : persistentInsetTypes | deferredInsetTypes;

        // Finally we apply the resolved insets by setting them as padding
        final Insets typeInsets = windowInsets.getInsets(types);
        v.setPadding(typeInsets.left, typeInsets.top, typeInsets.right, typeInsets.bottom);

        // We return the new WindowInsetsCompat.CONSUMED to stop the insets being dispatched any
        // further into the view hierarchy. This replaces the deprecated
        // WindowInsetsCompat.consumeSystemWindowInsets() and related functions.
        return WindowInsetsCompat.CONSUMED;
    }

    @Override
    public void onPrepare(WindowInsetsAnimationCompat animation) {
        if ((animation.getTypeMask() & deferredInsetTypes) != 0) {
            // We defer the WindowInsetsCompat.Type.ime() insets if the IME is currently not visible.
            // This results in only the WindowInsetsCompat.Type.systemBars() being applied, allowing
            // the scrolling view to remain at it's larger size.
            deferredInsets = true;
        }
    }

    @NonNull
    @Override
    public WindowInsetsCompat onProgress(@NonNull final WindowInsetsCompat insets,
                                         @NonNull final List<WindowInsetsAnimationCompat> runningAnims) {
        // This is a no-op. We don't actually want to handle any WindowInsetsAnimations
        return insets;
    }

    @Override
    public void onEnd(@NonNull final WindowInsetsAnimationCompat animation) {
        if (deferredInsets && (animation.getTypeMask() & deferredInsetTypes) != 0) {
            // If we deferred the IME insets and an IME animation has finished, we need to reset
            // the flag
            deferredInsets = false;

            // And finally dispatch the deferred insets to the view now.
            // Ideally we would just call view.requestApplyInsets() and let the normal dispatch
            // cycle happen, but this happens too late resulting in a visual flicker.
            // Instead we manually dispatch the most recent WindowInsets to the view.
            if (lastWindowInsets != null && view != null) {
                ViewCompat.dispatchApplyWindowInsets(view, lastWindowInsets);
            }
        }
    }
}