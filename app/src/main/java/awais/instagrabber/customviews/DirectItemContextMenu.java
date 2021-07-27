package awais.instagrabber.customviews;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.TimeInterpolator;
import android.animation.ValueAnimator;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Point;
import android.graphics.Rect;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.widget.ImageView;
import android.widget.PopupWindow;

import androidx.annotation.IdRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;
import androidx.appcompat.widget.AppCompatEditText;
import androidx.appcompat.widget.AppCompatImageView;
import androidx.appcompat.widget.AppCompatTextView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.util.Pair;

import java.util.List;
import java.util.function.Function;

import awais.instagrabber.R;
import awais.instagrabber.animations.RoundedRectRevealOutlineProvider;
import awais.instagrabber.customviews.emoji.Emoji;
import awais.instagrabber.customviews.emoji.ReactionsManager;
import awais.instagrabber.databinding.LayoutDirectItemOptionsBinding;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;

import static android.view.View.MeasureSpec.makeMeasureSpec;

public class DirectItemContextMenu extends PopupWindow {
    private static final String TAG = DirectItemContextMenu.class.getSimpleName();
    private static final int DO_NOT_UPDATE_FLAG = -1;
    private static final int DURATION = 300;

    private final Context context;
    private final boolean showReactions;
    private final ReactionsManager reactionsManager;
    private final int emojiSize;
    private final int emojiMargin;
    private final int emojiMarginHalf;
    private final Rect startRect = new Rect();
    private final Rect endRect = new Rect();
    private final TimeInterpolator revealInterpolator = new AccelerateDecelerateInterpolator();
    private final AnimatorListenerAdapter exitAnimationListener;
    private final TypedValue selectableItemBackgroundBorderless;
    private final TypedValue selectableItemBackground;
    private final int dividerHeight;
    private final int optionHeight;
    private final int optionPadding;
    private final int addAdjust;
    private final boolean hasOptions;
    private final List<MenuItem> options;
    private final int widthWithoutReactions;

    private AnimatorSet openCloseAnimator;
    private Point location;
    private Point point;
    private OnReactionClickListener onReactionClickListener;
    private OnOptionSelectListener onOptionSelectListener;
    private OnAddReactionClickListener onAddReactionListener;

    public DirectItemContextMenu(@NonNull final Context context, final boolean showReactions, final List<MenuItem> options) {
        super(context);
        this.context = context;
        this.showReactions = showReactions;
        this.options = options;
        if (!showReactions && (options == null || options.isEmpty())) {
            throw new IllegalArgumentException("showReactions is set false and options are empty");
        }
        reactionsManager = ReactionsManager.getInstance();
        final Resources resources = context.getResources();
        emojiSize = resources.getDimensionPixelSize(R.dimen.reaction_picker_emoji_size);
        emojiMargin = resources.getDimensionPixelSize(R.dimen.reaction_picker_emoji_margin);
        emojiMarginHalf = emojiMargin / 2;
        addAdjust = resources.getDimensionPixelSize(R.dimen.reaction_picker_add_padding_adjustment);
        dividerHeight = resources.getDimensionPixelSize(R.dimen.horizontal_divider_height);
        optionHeight = resources.getDimensionPixelSize(R.dimen.reaction_picker_option_height);
        optionPadding = resources.getDimensionPixelSize(R.dimen.dm_message_card_radius);
        widthWithoutReactions = resources.getDimensionPixelSize(R.dimen.dm_item_context_min_width);
        exitAnimationListener = new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(final Animator animation) {
                openCloseAnimator = null;
                point = null;
                getContentView().post(DirectItemContextMenu.super::dismiss);
            }
        };
        selectableItemBackgroundBorderless = new TypedValue();
        context.getTheme().resolveAttribute(android.R.attr.selectableItemBackgroundBorderless, selectableItemBackgroundBorderless, true);
        selectableItemBackground = new TypedValue();
        context.getTheme().resolveAttribute(android.R.attr.selectableItemBackground, selectableItemBackground, true);
        hasOptions = options != null && !options.isEmpty();
    }

    public void show(@NonNull View rootView, @NonNull final Point location) {
        final View content = createContentView();
        content.measure(makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED), makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED));
        setup(content);
        // rootView.getParent().requestDisallowInterceptTouchEvent(true);
        // final Point correctedLocation = new Point(location.x, location.y - emojiSize * 2);
        this.location = location;
        showAtLocation(rootView, Gravity.TOP | Gravity.START, location.x, location.y);
        // fixPopupLocation(popupWindow, correctedLocation);
        animateOpen();
    }

    private void setup(final View content) {
        setContentView(content);
        setWindowLayoutMode(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        setFocusable(true);
        setOutsideTouchable(true);
        setInputMethodMode(PopupWindow.INPUT_METHOD_NOT_NEEDED);
        setBackgroundDrawable(null);
    }

    public void setOnOptionSelectListener(final OnOptionSelectListener onOptionSelectListener) {
        this.onOptionSelectListener = onOptionSelectListener;
    }

    public void setOnReactionClickListener(final OnReactionClickListener onReactionClickListener) {
        this.onReactionClickListener = onReactionClickListener;
    }

    public void setOnAddReactionListener(final OnAddReactionClickListener onAddReactionListener) {
        this.onAddReactionListener = onAddReactionListener;
    }

    private void animateOpen() {
        final View contentView = getContentView();
        contentView.setVisibility(View.INVISIBLE);
        contentView.post(() -> {
            final AnimatorSet openAnim = new AnimatorSet();
            // Rectangular reveal.
            final ValueAnimator revealAnim = createOpenCloseOutlineProvider().createRevealAnimator(contentView, false);
            revealAnim.setDuration(DURATION);
            revealAnim.setInterpolator(revealInterpolator);

            ValueAnimator fadeIn = ValueAnimator.ofFloat(0, 1);
            fadeIn.setDuration(DURATION);
            fadeIn.setInterpolator(revealInterpolator);
            fadeIn.addUpdateListener(anim -> {
                float alpha = (float) anim.getAnimatedValue();
                contentView.setAlpha(revealAnim.isStarted() ? alpha : 0);
            });
            openAnim.play(fadeIn);
            openAnim.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    contentView.setAlpha(1f);
                    openCloseAnimator = null;
                }
            });

            openCloseAnimator = openAnim;
            openAnim.playSequentially(revealAnim);
            contentView.setVisibility(View.VISIBLE);
            openAnim.start();
        });
    }

    protected void animateClose() {
        endRect.setEmpty();
        if (openCloseAnimator != null) {
            openCloseAnimator.cancel();
        }
        final View contentView = getContentView();
        final AnimatorSet closeAnim = new AnimatorSet();
        // Rectangular reveal (reversed).
        final ValueAnimator revealAnim = createOpenCloseOutlineProvider().createRevealAnimator(contentView, true);
        revealAnim.setDuration(DURATION);
        revealAnim.setInterpolator(revealInterpolator);
        closeAnim.play(revealAnim);

        ValueAnimator fadeOut = ValueAnimator.ofFloat(contentView.getAlpha(), 0);
        fadeOut.setDuration(DURATION);
        fadeOut.setInterpolator(revealInterpolator);
        fadeOut.addUpdateListener(anim -> {
            float alpha = (float) anim.getAnimatedValue();
            contentView.setAlpha(revealAnim.isStarted() ? alpha : contentView.getAlpha());
        });
        closeAnim.playTogether(fadeOut);
        closeAnim.addListener(exitAnimationListener);
        openCloseAnimator = closeAnim;
        closeAnim.start();
    }

    private RoundedRectRevealOutlineProvider createOpenCloseOutlineProvider() {
        final View contentView = getContentView();
        final int radius = context.getResources().getDimensionPixelSize(R.dimen.dm_message_card_radius_small);
        // Log.d(TAG, "createOpenCloseOutlineProvider: " + locationOnScreen(contentView) + " " + contentView.getMeasuredWidth() + " " + contentView
        //         .getMeasuredHeight());
        if (point == null) {
            point = locationOnScreen(contentView);
        }
        final int left = location.x - point.x;
        final int top = location.y - point.y;
        startRect.set(left, top, left, top);
        endRect.set(0, 0, contentView.getMeasuredWidth(), contentView.getMeasuredHeight());
        return new RoundedRectRevealOutlineProvider(radius, radius, startRect, endRect);
    }

    public void dismiss() {
        animateClose();
    }

    private View createContentView() {
        final LayoutInflater layoutInflater = LayoutInflater.from(context);
        final LayoutDirectItemOptionsBinding binding = LayoutDirectItemOptionsBinding.inflate(layoutInflater, null, false);
        Pair<View, View> firstLastEmojiView = null;
        if (showReactions) {
            firstLastEmojiView = addReactions(layoutInflater, binding.container);
        }
        if (hasOptions) {
            View divider = null;
            if (showReactions) {
                if (firstLastEmojiView == null) {
                    throw new IllegalStateException("firstLastEmojiView is null even though reactions were added");
                }
                // add divider if reactions were added
                divider = addDivider(binding.container,
                                     firstLastEmojiView.first.getId(),
                                     firstLastEmojiView.first.getId(),
                                     firstLastEmojiView.second.getId());
                ((ConstraintLayout.LayoutParams) firstLastEmojiView.first.getLayoutParams()).bottomToTop = divider.getId();
            }
            addOptions(layoutInflater, binding.container, divider);
        }
        return binding.getRoot();
    }

    private Pair<View, View> addReactions(final LayoutInflater layoutInflater, final ConstraintLayout container) {
        final List<Emoji> reactions = reactionsManager.getReactions();
        AppCompatImageView prevSquareImageView = null;
        View firstImageView = null;
        View lastImageView = null;
        for (int i = 0; i < reactions.size(); i++) {
            final Emoji reaction = reactions.get(i);
            final AppCompatImageView imageView = getEmojiImageView();
            final ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) imageView.getLayoutParams();
            if (i == 0 && !hasOptions) {
                // only connect bottom to parent bottom if there are no options
                layoutParams.bottomToBottom = ConstraintLayout.LayoutParams.PARENT_ID;
            }
            if (i == 0) {
                layoutParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
                layoutParams.startToStart = ConstraintLayout.LayoutParams.PARENT_ID;
                firstImageView = imageView;
                layoutParams.setMargins(emojiMargin, emojiMargin, emojiMarginHalf, emojiMargin);
            } else {
                layoutParams.startToEnd = prevSquareImageView.getId();
                final ConstraintLayout.LayoutParams prevViewLayoutParams = (ConstraintLayout.LayoutParams) prevSquareImageView.getLayoutParams();
                prevViewLayoutParams.endToStart = imageView.getId();
                // always connect the other image view's top and bottom to the first image view top and bottom
                layoutParams.topToTop = firstImageView.getId();
                layoutParams.bottomToBottom = firstImageView.getId();
                layoutParams.setMargins(emojiMarginHalf, emojiMargin, emojiMarginHalf, emojiMargin);
            }
            imageView.setImageDrawable(reaction.getDrawable());
            imageView.setOnClickListener(view -> {
                if (onReactionClickListener != null) {
                    onReactionClickListener.onClick(reaction);
                }
                dismiss();
            });
            container.addView(imageView);
            prevSquareImageView = imageView;
        }
        // add the + icon
        if (prevSquareImageView != null) {
            final AppCompatImageView imageView = getEmojiImageView();
            final ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) imageView.getLayoutParams();
            layoutParams.topToTop = firstImageView.getId();
            layoutParams.bottomToBottom = firstImageView.getId();
            layoutParams.startToEnd = prevSquareImageView.getId();
            final ConstraintLayout.LayoutParams prevViewLayoutParams = (ConstraintLayout.LayoutParams) prevSquareImageView.getLayoutParams();
            prevViewLayoutParams.endToStart = imageView.getId();
            layoutParams.endToEnd = ConstraintLayout.LayoutParams.PARENT_ID;
            layoutParams.setMargins(emojiMarginHalf - addAdjust, emojiMargin - addAdjust, emojiMargin - addAdjust, emojiMargin - addAdjust);
            imageView.setImageResource(R.drawable.ic_add);
            imageView.setOnClickListener(view -> {
                if (onAddReactionListener != null) {
                    onAddReactionListener.onAdd();
                }
                dismiss();
            });
            lastImageView = imageView;
            container.addView(imageView);
        }
        return new Pair<>(firstImageView, lastImageView);
    }

    @NonNull
    private AppCompatImageView getEmojiImageView() {
        final AppCompatImageView imageView = new AppCompatImageView(context);
        final ConstraintLayout.LayoutParams layoutParams = new ConstraintLayout.LayoutParams(emojiSize, emojiSize);
        imageView.setBackgroundResource(selectableItemBackgroundBorderless.resourceId);
        imageView.setScaleType(ImageView.ScaleType.FIT_CENTER);
        imageView.setId(SquareImageView.generateViewId());
        imageView.setLayoutParams(layoutParams);
        return imageView;
    }

    private void addOptions(final LayoutInflater layoutInflater,
                            final ConstraintLayout container,
                            @Nullable final View divider) {
        View prevOptionView = null;
        if (!showReactions) {
            container.getLayoutParams().width = widthWithoutReactions;
        }
        for (int i = 0; i < options.size(); i++) {
            final MenuItem menuItem = options.get(i);
            final AppCompatTextView textView = getTextView();
            final ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) textView.getLayoutParams();
            layoutParams.startToStart = ConstraintLayout.LayoutParams.PARENT_ID;
            layoutParams.endToEnd = ConstraintLayout.LayoutParams.PARENT_ID;
            if (i == 0) {
                if (divider != null) {
                    layoutParams.topToBottom = divider.getId();
                    ((ConstraintLayout.LayoutParams) divider.getLayoutParams()).bottomToTop = textView.getId();
                } else {
                    // if divider is null mean reactions were not added, so connect top to top of parent
                    layoutParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
                    layoutParams.topMargin = emojiMargin; // material design spec (https://material.io/components/menus#specs)
                }
            } else {
                layoutParams.topToBottom = prevOptionView.getId();
                final ConstraintLayout.LayoutParams prevLayoutParams = (ConstraintLayout.LayoutParams) prevOptionView.getLayoutParams();
                prevLayoutParams.bottomToTop = textView.getId();
            }
            if (i == options.size() - 1) {
                layoutParams.bottomToBottom = ConstraintLayout.LayoutParams.PARENT_ID;
                layoutParams.bottomMargin = emojiMargin; // material design spec (https://material.io/components/menus#specs)
            }
            textView.setText(context.getString(menuItem.getTitleRes()));
            textView.setOnClickListener(v -> {
                if (onOptionSelectListener != null) {
                    onOptionSelectListener.onSelect(menuItem.getItemId(), menuItem.getCallback());
                }
                dismiss();
            });
            container.addView(textView);
            prevOptionView = textView;
        }
    }

    private AppCompatTextView getTextView() {
        final AppCompatTextView textView = new AppCompatTextView(context);
        textView.setId(AppCompatEditText.generateViewId());
        textView.setBackgroundResource(selectableItemBackground.resourceId);
        textView.setGravity(Gravity.CENTER_VERTICAL);
        textView.setPaddingRelative(optionPadding, 0, optionPadding, 0);
        textView.setTextAppearance(context, R.style.TextAppearance_MaterialComponents_Body1);
        final ConstraintLayout.LayoutParams layoutParams = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_CONSTRAINT,
                                                                                             optionHeight);
        textView.setLayoutParams(layoutParams);
        return textView;
    }

    private View addDivider(final ConstraintLayout container,
                            final int topViewId,
                            final int startViewId,
                            final int endViewId) {
        final View dividerView = new View(context);
        dividerView.setId(View.generateViewId());
        dividerView.setBackgroundResource(R.drawable.pref_list_divider_material);
        final ConstraintLayout.LayoutParams layoutParams = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_CONSTRAINT,
                                                                                             dividerHeight);
        layoutParams.topToBottom = topViewId;
        layoutParams.startToStart = startViewId;
        layoutParams.endToEnd = endViewId;
        dividerView.setLayoutParams(layoutParams);
        container.addView(dividerView);
        return dividerView;
    }

    @NonNull
    private Point locationOnScreen(@NonNull final View view) {
        final int[] location = new int[2];
        view.getLocationOnScreen(location);
        return new Point(location[0], location[1]);
    }

    public static class MenuItem {
        @IdRes
        private final int itemId;
        @StringRes
        private final int titleRes;

        /**
         * Callback function
         */
        private final Function<DirectItem, Void> callback;

        public MenuItem(@IdRes final int itemId, @StringRes final int titleRes) {
            this(itemId, titleRes, null);
        }

        public MenuItem(@IdRes final int itemId, @StringRes final int titleRes, @Nullable final Function<DirectItem, Void> callback) {
            this.itemId = itemId;
            this.titleRes = titleRes;
            this.callback = callback;
        }

        public int getItemId() {
            return itemId;
        }

        public int getTitleRes() {
            return titleRes;
        }

        public Function<DirectItem, Void> getCallback() {
            return callback;
        }
    }

    public interface OnOptionSelectListener {
        void onSelect(int itemId, @Nullable Function<DirectItem, Void> callback);
    }

    public interface OnReactionClickListener {
        void onClick(Emoji emoji);
    }

    public interface OnAddReactionClickListener {
        void onAdd();
    }

    // @NonNull
    // private Rect getGlobalVisibleRect(@NonNull final View view) {
    //     final Rect rect = new Rect();
    //     view.getGlobalVisibleRect(rect);
    //     return rect;
    // }

    // private void fixPopupLocation(@NonNull final PopupWindow popupWindow, @NonNull final Point desiredLocation) {
    //     popupWindow.getContentView().post(() -> {
    //         final Point actualLocation = locationOnScreen(popupWindow.getContentView());
    //
    //         if (!(actualLocation.x == desiredLocation.x && actualLocation.y == desiredLocation.y)) {
    //             final int differenceX = actualLocation.x - desiredLocation.x;
    //             final int differenceY = actualLocation.y - desiredLocation.y;
    //
    //             final int fixedOffsetX;
    //             final int fixedOffsetY;
    //
    //             if (actualLocation.x > desiredLocation.x) {
    //                 fixedOffsetX = desiredLocation.x - differenceX;
    //             } else {
    //                 fixedOffsetX = desiredLocation.x + differenceX;
    //             }
    //
    //             if (actualLocation.y > desiredLocation.y) {
    //                 fixedOffsetY = desiredLocation.y - differenceY;
    //             } else {
    //                 fixedOffsetY = desiredLocation.y + differenceY;
    //             }
    //
    //             popupWindow.update(fixedOffsetX, fixedOffsetY, DO_NOT_UPDATE_FLAG, DO_NOT_UPDATE_FLAG);
    //         }
    //     });
    // }
}

