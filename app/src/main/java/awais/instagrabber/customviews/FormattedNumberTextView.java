package awais.instagrabber.customviews;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatTextView;
import androidx.transition.ChangeBounds;
import androidx.transition.Transition;
import androidx.transition.TransitionManager;
import androidx.transition.TransitionSet;

import java.time.Duration;

import awais.instagrabber.customviews.helpers.ChangeText;
import awais.instagrabber.utils.NumberUtils;

public class FormattedNumberTextView extends AppCompatTextView {
    private static final String TAG = FormattedNumberTextView.class.getSimpleName();
    private static final Transition TRANSITION;

    private long number = Long.MIN_VALUE;
    private boolean showAbbreviation = true;
    private boolean animateChanges = false;
    private boolean toggleOnClick = true;
    private boolean autoToggleToAbbreviation = true;
    private long autoToggleTimeoutMs = Duration.ofSeconds(2).toMillis();
    private boolean initDone = false;

    static {
        final TransitionSet transitionSet = new TransitionSet();
        final ChangeText changeText = new ChangeText().setChangeBehavior(ChangeText.CHANGE_BEHAVIOR_OUT_IN);
        transitionSet.addTransition(changeText).addTransition(new ChangeBounds());
        TRANSITION = transitionSet;
    }


    public FormattedNumberTextView(@NonNull final Context context) {
        super(context);
        init();
    }

    public FormattedNumberTextView(@NonNull final Context context, @Nullable final AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public FormattedNumberTextView(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        if (initDone) return;
        setupClickToggle();
        initDone = true;
    }

    private void setupClickToggle() {
        setOnClickListener(null);
    }

    private OnClickListener getWrappedClickListener(@Nullable final OnClickListener l) {
        if (!toggleOnClick) {
            return l;
        }
        return v -> {
            toggleAbbreviation();
            if (l != null) {
                l.onClick(this);
            }
        };
    }

    public void setNumber(final long number) {
        if (this.number == number) return;
        this.number = number;
        format();
    }

    public void clearNumber() {
        if (number == Long.MIN_VALUE) return;
        number = Long.MIN_VALUE;
        format();
    }

    public void setShowAbbreviation(final boolean showAbbreviation) {
        if (this.showAbbreviation && showAbbreviation) return;
        this.showAbbreviation = showAbbreviation;
        format();
    }

    public boolean isShowAbbreviation() {
        return showAbbreviation;
    }

    private void toggleAbbreviation() {
        if (number == Long.MIN_VALUE) return;
        setShowAbbreviation(!showAbbreviation);
    }

    public void setToggleOnClick(final boolean toggleOnClick) {
        this.toggleOnClick = toggleOnClick;
    }

    public boolean isToggleOnClick() {
        return toggleOnClick;
    }

    public void setAutoToggleToAbbreviation(final boolean autoToggleToAbbreviation) {
        this.autoToggleToAbbreviation = autoToggleToAbbreviation;
    }

    public boolean isAutoToggleToAbbreviation() {
        return autoToggleToAbbreviation;
    }

    public void setAutoToggleTimeoutMs(final long autoToggleTimeoutMs) {
        this.autoToggleTimeoutMs = autoToggleTimeoutMs;
    }

    public long getAutoToggleTimeoutMs() {
        return autoToggleTimeoutMs;
    }

    public void setAnimateChanges(final boolean animateChanges) {
        this.animateChanges = animateChanges;
    }

    public boolean isAnimateChanges() {
        return animateChanges;
    }

    @Override
    public void setOnClickListener(@Nullable final OnClickListener l) {
        super.setOnClickListener(getWrappedClickListener(l));
    }

    private void format() {
        post(() -> {
            if (animateChanges) {
                try {
                    TransitionManager.beginDelayedTransition((ViewGroup) getParent(), TRANSITION);
                } catch (Exception e) {
                    Log.e(TAG, "format: ", e);
                }
            }
            if (number == Long.MIN_VALUE) {
                setText(null);
                return;
            }
            if (showAbbreviation) {
                setText(NumberUtils.abbreviate(number));
                return;
            }
            setText(String.valueOf(number));
            if (autoToggleToAbbreviation) {
                getHandler().postDelayed(() -> setShowAbbreviation(true), autoToggleTimeoutMs);
            }
        });
    }
}
