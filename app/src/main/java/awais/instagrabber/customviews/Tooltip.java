package awais.instagrabber.customviews;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.content.Context;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewPropertyAnimator;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.AppCompatTextView;

import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.utils.ViewUtils;

public class Tooltip extends AppCompatTextView {

    private View anchor;
    private ViewPropertyAnimator animator;
    private boolean showing;

    private final AppExecutors appExecutors = AppExecutors.getInstance();
    private final Runnable dismissRunnable = () -> {
        animator = animate().alpha(0).setListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                setVisibility(View.GONE);
            }
        }).setDuration(300);
        animator.start();
    };

    public Tooltip(@NonNull Context context, @NonNull ViewGroup parentView, int backgroundColor, int textColor) {
        super(context);
        setBackgroundDrawable(ViewUtils.createRoundRectDrawable(Utils.convertDpToPx(3), backgroundColor));
        setTextColor(textColor);
        setTextSize(TypedValue.COMPLEX_UNIT_DIP, 14);
        setPadding(Utils.convertDpToPx(8), Utils.convertDpToPx(7), Utils.convertDpToPx(8), Utils.convertDpToPx(7));
        setGravity(Gravity.CENTER_VERTICAL);
        parentView.addView(this, ViewUtils.createFrame(
                ViewUtils.WRAP_CONTENT, ViewUtils.WRAP_CONTENT, Gravity.START | Gravity.TOP, 5, 0, 5, 3));
        setVisibility(GONE);
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        updateTooltipPosition();
    }

    private void updateTooltipPosition() {
        if (anchor == null) {
            return;
        }
        int top = 0;
        int left = 0;

        View containerView = (View) getParent();
        View view = anchor;

        while (view != containerView) {
            top += view.getTop();
            left += view.getLeft();
            view = (View) view.getParent();
        }
        int x = left + anchor.getWidth() / 2 - getMeasuredWidth() / 2;
        if (x < 0) {
            x = 0;
        } else if (x + getMeasuredWidth() > containerView.getMeasuredWidth()) {
            x = containerView.getMeasuredWidth() - getMeasuredWidth() - Utils.convertDpToPx(16);
        }
        setTranslationX(x);

        int y = top - getMeasuredHeight();
        setTranslationY(y);
    }

    public void show(View anchor) {
        if (anchor == null) {
            return;
        }
        this.anchor = anchor;
        updateTooltipPosition();
        showing = true;

        appExecutors.mainThread().cancel(dismissRunnable);
        appExecutors.mainThread().execute(dismissRunnable, 2000);
        if (animator != null) {
            animator.setListener(null);
            animator.cancel();
            animator = null;
        }
        if (getVisibility() != VISIBLE) {
            setAlpha(0f);
            setVisibility(VISIBLE);
            animator = animate().setDuration(300).alpha(1f).setListener(null);
            animator.start();
        }
    }

    public void hide() {
        if (showing) {
            if (animator != null) {
                animator.setListener(null);
                animator.cancel();
                animator = null;
            }

            appExecutors.mainThread().cancel(dismissRunnable);
            dismissRunnable.run();
        }
        showing = false;
    }
}
