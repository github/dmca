package awais.instagrabber.customviews;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import awais.instagrabber.R;

public class ChatMessageLayout extends FrameLayout {

    private FrameLayout viewPartMain;
    private View viewPartInfo;
    private TypedArray a;

    private int viewPartInfoWidth;
    private int viewPartInfoHeight;

    // private boolean withGroupHeader = false;

    public ChatMessageLayout(@NonNull final Context context) {
        super(context);
    }

    public ChatMessageLayout(@NonNull final Context context, @Nullable final AttributeSet attrs) {
        super(context, attrs);
        a = context.obtainStyledAttributes(attrs, R.styleable.ChatMessageLayout, 0, 0);
    }

    public ChatMessageLayout(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        a = context.obtainStyledAttributes(attrs, R.styleable.ChatMessageLayout, defStyleAttr, 0);
    }

    public ChatMessageLayout(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr, final int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        a = context.obtainStyledAttributes(attrs, R.styleable.ChatMessageLayout, defStyleAttr, defStyleRes);
    }

    // public void setWithGroupHeader(boolean withGroupHeader) {
    //     this.withGroupHeader = withGroupHeader;
    // }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        try {
            viewPartMain = findViewById(a.getResourceId(R.styleable.ChatMessageLayout_viewPartMain, -1));
            viewPartInfo = findViewById(a.getResourceId(R.styleable.ChatMessageLayout_viewPartInfo, -1));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightSize;
        // heightSize = MeasureSpec.getSize(heightMeasureSpec);

        if (viewPartMain == null || viewPartInfo == null || widthSize <= 0) {
            return;
        }

        final View firstChild = viewPartMain.getChildAt(0);
        if (firstChild == null) return;

        final int firstChildId = firstChild.getId();
        if (firstChildId == R.id.reel_share_container) return;

        int availableWidth = widthSize - getPaddingLeft() - getPaddingRight();
        // int availableHeight = heightSize - getPaddingTop() - getPaddingBottom();

        final LayoutParams viewPartMainLayoutParams = (LayoutParams) viewPartMain.getLayoutParams();
        final int viewPartMainWidth = viewPartMain.getMeasuredWidth() + viewPartMainLayoutParams.leftMargin + viewPartMainLayoutParams.rightMargin;
        final int viewPartMainHeight = viewPartMain.getMeasuredHeight() + viewPartMainLayoutParams.topMargin + viewPartMainLayoutParams.bottomMargin;

        final LayoutParams viewPartInfoLayoutParams = (LayoutParams) viewPartInfo.getLayoutParams();
        viewPartInfoWidth = viewPartInfo.getMeasuredWidth() + viewPartInfoLayoutParams.leftMargin + viewPartInfoLayoutParams.rightMargin;
        viewPartInfoHeight = viewPartInfo.getMeasuredHeight() + viewPartInfoLayoutParams.topMargin + viewPartInfoLayoutParams.bottomMargin;

        widthSize = getPaddingLeft() + getPaddingRight();
        heightSize = getPaddingTop() + getPaddingBottom();
        if (firstChildId == R.id.media_container) {
            widthSize += viewPartMainWidth;
            heightSize += viewPartMainHeight;
        } else if (firstChildId == R.id.raven_media_container || firstChildId == R.id.profile_container || firstChildId == R.id.voice_media
                || firstChildId == R.id.story_container || firstChildId == R.id.media_share_container || firstChildId == R.id.link_container
                || firstChildId == R.id.ivAnimatedMessage) {
            widthSize += viewPartMainWidth;
            heightSize += viewPartMainHeight + viewPartInfoHeight;
        } else {
            int viewPartMainLineCount = 1;
            float viewPartMainLastLineWidth = 0;
            if (firstChild instanceof TextView) {
                viewPartMainLineCount = ((TextView) firstChild).getLineCount();
                viewPartMainLastLineWidth = viewPartMainLineCount > 0
                                            ? ((TextView) firstChild).getLayout().getLineWidth(viewPartMainLineCount - 1)
                                            : 0;
                // also include start left padding
                viewPartMainLastLineWidth += firstChild.getPaddingLeft();
            }

            final float lastLineWithInfoWidth = viewPartMainLastLineWidth + viewPartInfoWidth;
            if (viewPartMainLineCount > 1 && lastLineWithInfoWidth <= viewPartMain.getMeasuredWidth()) {
                widthSize += viewPartMainWidth;
                heightSize += viewPartMainHeight;
            } else if (viewPartMainLineCount > 1 && (lastLineWithInfoWidth > availableWidth)) {
                widthSize += viewPartMainWidth;
                heightSize += viewPartMainHeight + viewPartInfoHeight;
            } else if (viewPartMainLineCount == 1 && (viewPartMainWidth + viewPartInfoWidth > availableWidth)) {
                widthSize += viewPartMain.getMeasuredWidth();
                heightSize += viewPartMainHeight + viewPartInfoHeight;
            } else {
                heightSize += viewPartMainHeight;
                widthSize += viewPartMainWidth + viewPartInfoWidth;
            }

            // if (isInEditMode()) {
            //     TextView wDebugView = (TextView) ((ViewGroup) this.getParent()).findViewWithTag("debug");
            //     wDebugView.setText(lastLineWithInfoWidth
            //                                + "\n" + availableWidth
            //                                + "\n" + viewPartMain.getMeasuredWidth()
            //                                + "\n" + (lastLineWithInfoWidth <= viewPartMain.getMeasuredWidth())
            //                                + "\n" + (lastLineWithInfoWidth > availableWidth)
            //                                + "\n" + (viewPartMainWidth + viewPartInfoWidth > availableWidth));
            // }
        }
        setMeasuredDimension(widthSize, heightSize);
        super.onMeasure(MeasureSpec.makeMeasureSpec(widthSize, MeasureSpec.EXACTLY), MeasureSpec.makeMeasureSpec(heightSize, MeasureSpec.EXACTLY));

    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);

        if (viewPartMain == null || viewPartInfo == null) {
            return;
        }
        // if (withGroupHeader) {
        //     viewPartMain.layout(
        //             getPaddingLeft(),
        //             getPaddingTop() - Utils.convertDpToPx(4),
        //             viewPartMain.getWidth() + getPaddingLeft(),
        //             viewPartMain.getHeight() + getPaddingTop());
        //
        // } else {
        viewPartMain.layout(
                getPaddingLeft(),
                getPaddingTop(),
                viewPartMain.getWidth() + getPaddingLeft(),
                viewPartMain.getHeight() + getPaddingTop());

        // }
        viewPartInfo.layout(
                right - left - viewPartInfoWidth - getPaddingRight(),
                bottom - top - getPaddingBottom() - viewPartInfoHeight,
                right - left - getPaddingRight(),
                bottom - top - getPaddingBottom());
    }
}
