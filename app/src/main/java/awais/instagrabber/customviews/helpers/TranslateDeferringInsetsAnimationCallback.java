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
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsAnimationCompat;
import androidx.core.view.WindowInsetsCompat;

import java.util.List;

/**
 * A [WindowInsetsAnimationCompat.Callback] which will translate/move the given view during any
 * inset animations of the given inset type.
 * <p>
 * This class works in tandem with [RootViewDeferringInsetsCallback] to support the deferring of
 * certain [WindowInsetsCompat.Type] values during a [WindowInsetsAnimationCompat], provided in
 * [deferredInsetTypes]. The values passed into this constructor should match those which
 * the [RootViewDeferringInsetsCallback] is created with.
 */
public class TranslateDeferringInsetsAnimationCallback extends WindowInsetsAnimationCompat.Callback {
    private final View view;
    private final int persistentInsetTypes;
    private final int deferredInsetTypes;

    private boolean shouldTranslate = true;
    private int kbHeight;

    public TranslateDeferringInsetsAnimationCallback(final View view,
                                                     final int persistentInsetTypes,
                                                     final int deferredInsetTypes) {
        this(view, persistentInsetTypes, deferredInsetTypes, DISPATCH_MODE_STOP);
    }

    /**
     * @param view                 the view to translate from it's start to end state
     * @param persistentInsetTypes the bitmask of any inset types which were handled as part of the
     *                             layout
     * @param deferredInsetTypes   the bitmask of insets types which should be deferred until after
     *                             any [WindowInsetsAnimationCompat]s have ended
     * @param dispatchMode         The dispatch mode for this callback.
     *                             See [WindowInsetsAnimationCompat.Callback.getDispatchMode].
     */
    public TranslateDeferringInsetsAnimationCallback(final View view,
                                                     final int persistentInsetTypes,
                                                     final int deferredInsetTypes,
                                                     final int dispatchMode) {
        super(dispatchMode);
        if ((persistentInsetTypes & deferredInsetTypes) != 0) {
            throw new IllegalArgumentException("persistentInsetTypes and deferredInsetTypes can not contain " +
                                                       "any of same WindowInsetsCompat.Type values");
        }
        this.view = view;
        this.persistentInsetTypes = persistentInsetTypes;
        this.deferredInsetTypes = deferredInsetTypes;
    }

    @NonNull
    @Override
    public WindowInsetsCompat onProgress(@NonNull final WindowInsetsCompat insets,
                                         @NonNull final List<WindowInsetsAnimationCompat> runningAnimations) {
        // onProgress() is called when any of the running animations progress...

        // First we get the insets which are potentially deferred
        final Insets typesInset = insets.getInsets(deferredInsetTypes);
        // Then we get the persistent inset types which are applied as padding during layout
        final Insets otherInset = insets.getInsets(persistentInsetTypes);

        // Now that we subtract the two insets, to calculate the difference. We also coerce
        // the insets to be >= 0, to make sure we don't use negative insets.
        final Insets subtract = Insets.subtract(typesInset, otherInset);
        final Insets diff = Insets.max(subtract, Insets.NONE);

        // The resulting `diff` insets contain the values for us to apply as a translation
        // to the view
        view.setTranslationX(diff.left - diff.right);
        view.setTranslationY(shouldTranslate ? diff.top - diff.bottom : -kbHeight);

        return insets;
    }

    @Override
    public void onEnd(@NonNull final WindowInsetsAnimationCompat animation) {
        try {
            final WindowInsetsCompat rootWindowInsets = ViewCompat.getRootWindowInsets(view);
            if (kbHeight == 0) {
                if (rootWindowInsets == null) return;
                final Insets imeInsets = rootWindowInsets.getInsets(WindowInsetsCompat.Type.ime());
                final Insets navBarInsets = rootWindowInsets.getInsets(WindowInsetsCompat.Type.navigationBars());
                kbHeight = imeInsets.bottom - navBarInsets.bottom;
            }
            // Once the animation has ended, reset the translation values
            view.setTranslationX(0f);
            final boolean visible = rootWindowInsets != null && rootWindowInsets.isVisible(WindowInsetsCompat.Type.ime());
            float translationY = 0;
            if (!shouldTranslate) {
                translationY = -kbHeight;
                if (visible) {
                    translationY = 0;
                }
            }
            view.setTranslationY(translationY);
        } finally {
            shouldTranslate = true;
        }
    }

    public void setShouldTranslate(final boolean shouldTranslate) {
        this.shouldTranslate = shouldTranslate;
    }
}
