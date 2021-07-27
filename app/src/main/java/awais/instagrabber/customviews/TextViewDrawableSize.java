package awais.instagrabber.customviews;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.emoji.widget.EmojiAppCompatTextView;

import awais.instagrabber.R;

/**
 * https://stackoverflow.com/a/31916731
 */
public class TextViewDrawableSize extends EmojiAppCompatTextView {

    private int mDrawableWidth;
    private int mDrawableHeight;
    private boolean calledFromInit = false;

    public TextViewDrawableSize(final Context context) {
        this(context, null);
    }

    public TextViewDrawableSize(final Context context, final AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public TextViewDrawableSize(final Context context, final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context, attrs, defStyleAttr);
    }

    private void init(@NonNull final Context context, final AttributeSet attrs, final int defStyleAttr) {
        final TypedArray array = context.obtainStyledAttributes(attrs, R.styleable.TextViewDrawableSize, defStyleAttr, 0);

        try {
            mDrawableWidth = array.getDimensionPixelSize(R.styleable.TextViewDrawableSize_compoundDrawableWidth, -1);
            mDrawableHeight = array.getDimensionPixelSize(R.styleable.TextViewDrawableSize_compoundDrawableHeight, -1);
        } finally {
            array.recycle();
        }

        if (mDrawableWidth > 0 || mDrawableHeight > 0) {
            initCompoundDrawableSize();
        }
    }

    private void initCompoundDrawableSize() {
        final Drawable[] drawables = getCompoundDrawablesRelative();
        for (Drawable drawable : drawables) {
            if (drawable == null) {
                continue;
            }

            final Rect realBounds = drawable.getBounds();
            float scaleFactor = drawable.getIntrinsicHeight() / (float) drawable.getIntrinsicWidth();

            float drawableWidth = drawable.getIntrinsicWidth();
            float drawableHeight = drawable.getIntrinsicHeight();

            if (mDrawableWidth > 0) {
                // save scale factor of image
                if (drawableWidth > mDrawableWidth) {
                    drawableWidth = mDrawableWidth;
                    drawableHeight = drawableWidth * scaleFactor;
                }
            }
            if (mDrawableHeight > 0) {
                // save scale factor of image
                if (drawableHeight > mDrawableHeight) {
                    drawableHeight = mDrawableHeight;
                    drawableWidth = drawableHeight / scaleFactor;
                }
            }

            realBounds.right = realBounds.left + Math.round(drawableWidth);
            realBounds.bottom = realBounds.top + Math.round(drawableHeight);

            drawable.setBounds(realBounds);
        }
        setCompoundDrawablesRelative(drawables[0], drawables[1], drawables[2], drawables[3]);
    }

    public void setCompoundDrawablesRelativeWithSize(@Nullable final Drawable start,
                                                     @Nullable final Drawable top,
                                                     @Nullable final Drawable end,
                                                     @Nullable final Drawable bottom) {
        setCompoundDrawablesRelative(start, top, end, bottom);
        initCompoundDrawableSize();
    }
}
