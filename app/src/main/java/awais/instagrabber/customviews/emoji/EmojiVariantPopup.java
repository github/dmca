package awais.instagrabber.customviews.emoji;

/*
 * Copyright (C) 2016 - Niklas Baudy, Ruben Gees, Mario Đanić and contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.drawable.BitmapDrawable;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.PopupWindow;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.customviews.emoji.EmojiPicker.OnEmojiClickListener;
import awais.instagrabber.databinding.ItemEmojiGridBinding;
import awais.instagrabber.databinding.LayoutEmojiVariantPopupBinding;
import awais.instagrabber.utils.AppExecutors;

import static android.view.View.MeasureSpec.makeMeasureSpec;

public final class EmojiVariantPopup {
    private static final int DO_NOT_UPDATE_FLAG = -1;

    private final View rootView;
    private final OnEmojiClickListener listener;

    private PopupWindow popupWindow;
    private View rootImageView;
    private final EmojiVariantManager emojiVariantManager;
    private final AppExecutors appExecutors;

    public EmojiVariantPopup(@NonNull final View rootView,
                             final OnEmojiClickListener listener) {
        this.rootView = rootView;
        this.listener = listener;
        emojiVariantManager = EmojiVariantManager.getInstance();
        appExecutors = AppExecutors.getInstance();
    }

    public void show(@NonNull final View view, @NonNull final Emoji emoji) {
        dismiss();

        rootImageView = view;

        final View content = initView(view.getContext(), emoji, view.getWidth());

        popupWindow = new PopupWindow(content, WindowManager.LayoutParams.WRAP_CONTENT, WindowManager.LayoutParams.WRAP_CONTENT);
        popupWindow.setFocusable(true);
        popupWindow.setOutsideTouchable(true);
        popupWindow.setInputMethodMode(PopupWindow.INPUT_METHOD_NOT_NEEDED);
        popupWindow.setBackgroundDrawable(new BitmapDrawable(view.getContext().getResources(), (Bitmap) null));

        content.measure(makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED), makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED));

        final Point location = locationOnScreen(view);
        final Point desiredLocation = new Point(
                location.x - content.getMeasuredWidth() / 2 + view.getWidth() / 2,
                location.y - content.getMeasuredHeight()
        );

        popupWindow.showAtLocation(rootView, Gravity.NO_GRAVITY, desiredLocation.x, desiredLocation.y);
        rootImageView.getParent().requestDisallowInterceptTouchEvent(true);
        fixPopupLocation(popupWindow, desiredLocation);
    }

    public void dismiss() {
        rootImageView = null;

        if (popupWindow != null) {
            popupWindow.dismiss();
            popupWindow = null;
        }
    }

    private View initView(@NonNull final Context context, @NonNull final Emoji emoji, final int width) {
        final LayoutInflater layoutInflater = LayoutInflater.from(context);
        final LayoutEmojiVariantPopupBinding binding = LayoutEmojiVariantPopupBinding.inflate(layoutInflater, null, false);
        final List<Emoji> variants = new ArrayList<>(emoji.getVariants());
        // Add parent at start of list
        // variants.add(0, emoji);
        for (final Emoji variant : variants) {
            final ItemEmojiGridBinding itemBinding = ItemEmojiGridBinding.inflate(layoutInflater, binding.container, false);
            final ViewGroup.MarginLayoutParams layoutParams = (ViewGroup.MarginLayoutParams) itemBinding.image.getLayoutParams();
            // Use the same size for Emojis as in the picker.
            layoutParams.width = width;
            itemBinding.image.setImageDrawable(variant.getDrawable());
            itemBinding.image.setOnClickListener(view -> {
                if (listener != null) {
                    if (!variant.getUnicode().equals(emojiVariantManager.getVariant(emoji.getUnicode()))) {
                        emojiVariantManager.setVariant(emoji.getUnicode(), variant.getUnicode());
                    }
                    listener.onClick(view, variant);
                }
                dismiss();
            });
            binding.container.addView(itemBinding.getRoot());
        }
        return binding.getRoot();
    }

    @NonNull
    private Point locationOnScreen(@NonNull final View view) {
        final int[] location = new int[2];
        view.getLocationOnScreen(location);
        return new Point(location[0], location[1]);
    }

    private void fixPopupLocation(@NonNull final PopupWindow popupWindow, @NonNull final Point desiredLocation) {
        popupWindow.getContentView().post(() -> {
            final Point actualLocation = locationOnScreen(popupWindow.getContentView());

            if (!(actualLocation.x == desiredLocation.x && actualLocation.y == desiredLocation.y)) {
                final int differenceX = actualLocation.x - desiredLocation.x;
                final int differenceY = actualLocation.y - desiredLocation.y;

                final int fixedOffsetX;
                final int fixedOffsetY;

                if (actualLocation.x > desiredLocation.x) {
                    fixedOffsetX = desiredLocation.x - differenceX;
                } else {
                    fixedOffsetX = desiredLocation.x + differenceX;
                }

                if (actualLocation.y > desiredLocation.y) {
                    fixedOffsetY = desiredLocation.y - differenceY;
                } else {
                    fixedOffsetY = desiredLocation.y + differenceY;
                }

                popupWindow.update(fixedOffsetX, fixedOffsetY, DO_NOT_UPDATE_FLAG, DO_NOT_UPDATE_FLAG);
            }
        });
    }
}

