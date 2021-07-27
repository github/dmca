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

import android.view.View;

import androidx.annotation.NonNull;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsAnimationCompat;
import androidx.core.view.WindowInsetsCompat;

import java.util.List;

/**
 * A [WindowInsetsAnimationCompat.Callback] which will request and clear focus on the given view,
 * depending on the [WindowInsetsCompat.Type.ime] visibility state when an IME
 * [WindowInsetsAnimationCompat] has finished.
 * <p>
 * This is primarily used when animating the [WindowInsetsCompat.Type.ime], so that the
 * appropriate view is focused for accepting input from the IME.
 */
public class ControlFocusInsetsAnimationCallback extends WindowInsetsAnimationCompat.Callback {

    private final View view;

    public ControlFocusInsetsAnimationCallback(@NonNull final View view) {
        this(view, DISPATCH_MODE_STOP);
    }

    /**
     * @param view         the view to request/clear focus
     * @param dispatchMode The dispatch mode for this callback.
     * @see WindowInsetsAnimationCompat.Callback.DispatchMode
     */
    public ControlFocusInsetsAnimationCallback(@NonNull final View view, final int dispatchMode) {
        super(dispatchMode);
        this.view = view;
    }

    @NonNull
    @Override
    public WindowInsetsCompat onProgress(@NonNull final WindowInsetsCompat insets,
                                         @NonNull final List<WindowInsetsAnimationCompat> runningAnimations) {
        // no-op and return the insets
        return insets;
    }

    @Override
    public void onEnd(final WindowInsetsAnimationCompat animation) {
        if ((animation.getTypeMask() & WindowInsetsCompat.Type.ime()) != 0) {
            // The animation has now finished, so we can check the view's focus state.
            // We post the check because the rootWindowInsets has not yet been updated, but will
            // be in the next message traversal
            view.post(this::checkFocus);
        }
    }

    private void checkFocus() {
        final WindowInsetsCompat rootWindowInsets = ViewCompat.getRootWindowInsets(view);
        boolean imeVisible = false;
        if (rootWindowInsets != null) {
            imeVisible = rootWindowInsets.isVisible(WindowInsetsCompat.Type.ime());
        }
        if (imeVisible && view.getRootView().findFocus() == null) {
            // If the IME will be visible, and there is not a currently focused view in
            // the hierarchy, request focus on our view
            view.requestFocus();
        } else if (!imeVisible && view.isFocused()) {
            // If the IME will not be visible and our view is currently focused, clear the focus
            view.clearFocus();
        }
    }
}
