package awais.instagrabber.customviews.helpers;


import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.view.HapticFeedbackConstants;
import android.view.MotionEvent;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.appcompat.content.res.AppCompatResources;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.utils.Utils;

/**
 * Thanks to https://github.com/izjumovfs/SwipeToReply/blob/master/swipetoreply/src/main/java/com/capybaralabs/swipetoreply/SwipeController.java
 */
public class SwipeAndRestoreItemTouchHelperCallback extends ItemTouchHelper.Callback {
    private static final String TAG = "SwipeRestoreCallback";

    private final float swipeThreshold;
    private final float swipeAutoCancelThreshold;
    private final OnSwipeListener onSwipeListener;
    private final Drawable replyIcon;
    // private final Drawable replyIconBackground;
    private final int replyIconShowThreshold;
    private final float replyIconMaxTranslation;
    private final Rect replyIconBounds = new Rect();
    private final float replyIconXOffset;
    private final int replyIconSize;

    private boolean mSwipeBack = false;
    private boolean hasVibrated;

    public SwipeAndRestoreItemTouchHelperCallback(final Context context, final OnSwipeListener onSwipeListener) {
        this.onSwipeListener = onSwipeListener;
        swipeThreshold = Utils.displayMetrics.widthPixels * 0.25f;
        swipeAutoCancelThreshold = swipeThreshold + Utils.convertDpToPx(5);
        replyIcon = AppCompatResources.getDrawable(context, R.drawable.ic_round_reply_24);
        if (replyIcon == null) {
            throw new IllegalArgumentException("reply icon is null");
        }
        replyIcon.setTint(context.getResources().getColor(R.color.white)); //todo need to update according to theme
        replyIconShowThreshold = Utils.convertDpToPx(24);
        replyIconMaxTranslation = swipeThreshold - replyIconShowThreshold;
        // Log.d(TAG, "replyIconShowThreshold: " + replyIconShowThreshold + ", swipeThreshold: " + swipeThreshold);
        replyIconSize = replyIconShowThreshold; // Utils.convertDpToPx(24);
        replyIconXOffset = swipeThreshold * 0.25f /*Utils.convertDpToPx(20)*/;
    }

    @Override
    public int getMovementFlags(@NonNull RecyclerView recyclerView, @NonNull RecyclerView.ViewHolder viewHolder) {
        if (!(viewHolder instanceof SwipeableViewHolder)) {
            return makeMovementFlags(ItemTouchHelper.ACTION_STATE_IDLE, ItemTouchHelper.ACTION_STATE_IDLE);
        }
        return makeMovementFlags(ItemTouchHelper.ACTION_STATE_IDLE, ((SwipeableViewHolder) viewHolder).getSwipeDirection());
    }

    @Override
    public boolean onMove(@NonNull RecyclerView recyclerView,
                          @NonNull RecyclerView.ViewHolder viewHolder,
                          @NonNull RecyclerView.ViewHolder viewHolder1) {
        return false;
    }

    @Override
    public void onSwiped(@NonNull RecyclerView.ViewHolder viewHolder, int i) {}

    @Override
    public int convertToAbsoluteDirection(int flags, int layoutDirection) {
        if (mSwipeBack) {
            mSwipeBack = false;
            return 0;
        }
        return super.convertToAbsoluteDirection(flags, layoutDirection);
    }

    @Override
    public void onChildDraw(@NonNull Canvas c,
                            @NonNull RecyclerView recyclerView,
                            @NonNull RecyclerView.ViewHolder viewHolder,
                            float dX,
                            float dY,
                            int actionState,
                            boolean isCurrentlyActive) {
        if (actionState == ItemTouchHelper.ACTION_STATE_SWIPE) {
            setTouchListener(recyclerView, viewHolder);
        }
        super.onChildDraw(c, recyclerView, viewHolder, dX, dY, actionState, isCurrentlyActive);
        drawReplyButton(c, viewHolder);
    }

    @SuppressLint("ClickableViewAccessibility")
    private void setTouchListener(RecyclerView recyclerView, final RecyclerView.ViewHolder viewHolder) {
        recyclerView.setOnTouchListener((v, event) -> {
            if (event.getAction() == MotionEvent.ACTION_MOVE) {
                if (Math.abs(viewHolder.itemView.getTranslationX()) >= swipeAutoCancelThreshold) {
                    if (!hasVibrated) {
                        viewHolder.itemView.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,
                                                                  HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        hasVibrated = true;
                    }
                    //     MotionEvent cancelEvent = MotionEvent.obtain(event);
                    //     cancelEvent.setAction(MotionEvent.ACTION_CANCEL);
                    //     recyclerView.dispatchTouchEvent(cancelEvent);
                    //     cancelEvent.recycle();
                }
            }
            mSwipeBack = event.getAction() == MotionEvent.ACTION_CANCEL || event.getAction() == MotionEvent.ACTION_UP;
            if (mSwipeBack) {
                hasVibrated = false;
                if (Math.abs(viewHolder.itemView.getTranslationX()) >= swipeThreshold) {
                    if (onSwipeListener != null) {
                        onSwipeListener.onSwipe(viewHolder.getBindingAdapterPosition(), viewHolder);
                    }
                }
            }
            return false;
        });
    }

    public interface SwipeableViewHolder {
        int getSwipeDirection();
    }

    public interface OnSwipeListener {
        void onSwipe(final int adapterPosition, final RecyclerView.ViewHolder viewHolder);
    }

    private void drawReplyButton(Canvas canvas, final RecyclerView.ViewHolder viewHolder) {
        if (!(viewHolder instanceof SwipeableViewHolder)) return;
        final int swipeDirection = ((SwipeableViewHolder) viewHolder).getSwipeDirection();
        if (swipeDirection != ItemTouchHelper.START && swipeDirection != ItemTouchHelper.END) return;
        final View view = viewHolder.itemView;
        float translationX = view.getTranslationX();
        boolean show = false;
        float progress;
        final float translationXAbs = Math.abs(translationX);
        if (translationXAbs >= replyIconShowThreshold) {
            show = true;
        }
        if (show) {
            // replyIconShowThreshold -> swipeThreshold <=> progress 0 -> 1
            final float replyIconTranslation = translationXAbs - replyIconShowThreshold;
            progress = replyIconTranslation / replyIconMaxTranslation;
            if (progress > 1) {
                progress = 1f;
            }
            if (progress < 0) {
                progress = 0;
            }
            // Log.d(TAG, /*"translationX: " + translationX + ",  replyIconTranslation: " + replyIconTranslation +*/ "progress: " + progress);
        } else {
            progress = 0f;
            // Log.d(TAG, /*"translationX: " + translationX + ",  replyIconTranslation: " + 0 +*/ "progress: " + progress);
        }
        if (progress > 0) {
            // calculate the reply icon y position, then offset top, bottom with icon size
            final int y = view.getTop() + (view.getMeasuredHeight() / 2);
            final int tempIconSize = (int) (replyIconSize * progress);
            final int tempIconSizeHalf = tempIconSize / 2;
            final int xOffset = (int) (replyIconXOffset * progress);
            final int left;
            if (swipeDirection == ItemTouchHelper.END) {
                // draw arrow of left side
                left = xOffset;
            } else {
                // draw arrow of right side
                left = view.getMeasuredWidth() - xOffset - tempIconSize;
            }
            final int right = tempIconSize + left;
            replyIconBounds.set(left, y - tempIconSizeHalf, right, y + tempIconSizeHalf);
            replyIcon.setBounds(replyIconBounds);
            replyIcon.draw(canvas);
        }
    }

}
