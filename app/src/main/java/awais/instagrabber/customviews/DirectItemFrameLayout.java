package awais.instagrabber.customviews;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class DirectItemFrameLayout extends FrameLayout {
    private static final String TAG = DirectItemFrameLayout.class.getSimpleName();

    private boolean longPressed = false;
    private float touchX;
    private float touchY;
    private OnItemLongClickListener onItemLongClickListener;
    private int touchSlop;

    private final Handler handler = new Handler();
    private final Runnable longPressRunnable = () -> {
        longPressed = true;
        if (onItemLongClickListener != null) {
            onItemLongClickListener.onLongClick(this, touchX, touchY);
        }
    };
    private final Runnable longPressStartRunnable = () -> {
        if (onItemLongClickListener != null) {
            onItemLongClickListener.onLongClickStart(this);
        }
    };

    public DirectItemFrameLayout(@NonNull final Context context) {
        super(context);
        init(context);
    }

    public DirectItemFrameLayout(@NonNull final Context context, @Nullable final AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public DirectItemFrameLayout(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    public DirectItemFrameLayout(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr, final int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(context);
    }

    private void init(final Context context) {
        ViewConfiguration vc = ViewConfiguration.get(context);
        touchSlop = vc.getScaledTouchSlop();
    }

    public void setOnItemLongClickListener(final OnItemLongClickListener onItemLongClickListener) {
        this.onItemLongClickListener = onItemLongClickListener;
    }

    @Override
    public boolean dispatchTouchEvent(final MotionEvent ev) {
        switch (ev.getAction()) {
            case MotionEvent.ACTION_DOWN:
                longPressed = false;
                handler.postDelayed(longPressRunnable, ViewConfiguration.getLongPressTimeout());
                handler.postDelayed(longPressStartRunnable, ViewConfiguration.getTapTimeout());
                touchX = ev.getRawX();
                touchY = ev.getRawY();
                break;
            case MotionEvent.ACTION_MOVE:
                final float diffX = touchX - ev.getRawX();
                final float diffXAbs = Math.abs(diffX);
                final boolean isMoved = diffXAbs > touchSlop || Math.abs(touchY - ev.getRawY()) > touchSlop;
                if (longPressed || isMoved) {
                    handler.removeCallbacks(longPressStartRunnable);
                    handler.removeCallbacks(longPressRunnable);
                    if (!longPressed) {
                        if (onItemLongClickListener != null) {
                            onItemLongClickListener.onLongClickCancel(this);
                        }
                    }
                    // if (diffXAbs > touchSlop) {
                    //     setTranslationX(-diffX);
                    // }
                }
                break;
            case MotionEvent.ACTION_UP:
                handler.removeCallbacks(longPressRunnable);
                handler.removeCallbacks(longPressStartRunnable);
                if (longPressed) {
                    return true;
                }
                if (onItemLongClickListener != null) {
                    onItemLongClickListener.onLongClickCancel(this);
                }
                break;
            case MotionEvent.ACTION_CANCEL:
                handler.removeCallbacks(longPressRunnable);
                handler.removeCallbacks(longPressStartRunnable);
                if (onItemLongClickListener != null) {
                    onItemLongClickListener.onLongClickCancel(this);
                }
                break;
        }
        final boolean dispatchTouchEvent = super.dispatchTouchEvent(ev);
        if (ev.getAction() == MotionEvent.ACTION_DOWN && !dispatchTouchEvent) {
            return true;
        }
        return dispatchTouchEvent;
    }

    public interface OnItemLongClickListener {
        void onLongClickStart(View view);

        void onLongClickCancel(View view);

        void onLongClick(View view, float x, float y);
    }
}
