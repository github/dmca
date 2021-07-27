package awais.instagrabber.customviews;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.ViewGroup;

import androidx.annotation.DimenRes;
import androidx.annotation.NonNull;

import com.facebook.drawee.generic.GenericDraweeHierarchy;
import com.facebook.drawee.generic.RoundingParams;

import java.util.HashMap;
import java.util.Map;

import awais.instagrabber.R;

public final class ProfilePicView extends CircularImageView {
    private static final String TAG = "ProfilePicView";

    private Size size;
    private int dimensionPixelSize;

    public ProfilePicView(Context context, GenericDraweeHierarchy hierarchy) {
        super(context);
        setHierarchy(hierarchy);
        size = Size.REGULAR;
        updateLayout();
    }

    public ProfilePicView(final Context context) {
        super(context);
        size = Size.REGULAR;
        updateLayout();
    }

    public ProfilePicView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
        parseAttrs(context, attrs);
        updateLayout();
    }

    public ProfilePicView(final Context context,
                          final AttributeSet attrs,
                          final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        parseAttrs(context, attrs);
        updateLayout();
    }

    private void parseAttrs(final Context context, final AttributeSet attrs) {
        final TypedArray a = context.getTheme().obtainStyledAttributes(
                attrs,
                R.styleable.ProfilePicView,
                0,
                0);
        try {
            final int sizeValue = a.getInt(R.styleable.ProfilePicView_size, Size.REGULAR.getValue());
            size = Size.valueOf(sizeValue);
        } finally {
            a.recycle();
        }
    }

    private void updateLayout() {
        @DimenRes final int dimenRes;
        switch (size) {
            case SMALL:
                dimenRes = R.dimen.profile_pic_size_small;
                break;
            case SMALLER:
                dimenRes = R.dimen.profile_pic_size_smaller;
                break;
            case TINY:
                dimenRes = R.dimen.profile_pic_size_tiny;
                break;
            case LARGE:
                dimenRes = R.dimen.profile_pic_size_large;
                break;
            default:
            case REGULAR:
                dimenRes = R.dimen.profile_pic_size_regular;
                break;
        }
        ViewGroup.LayoutParams layoutParams = getLayoutParams();
        if (layoutParams == null) {
            layoutParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        }
        dimensionPixelSize = getResources().getDimensionPixelSize(dimenRes);
        layoutParams.width = dimensionPixelSize;
        layoutParams.height = dimensionPixelSize;

        // invalidate();
        // requestLayout();
    }

    public void setSize(final Size size) {
        this.size = size;
        updateLayout();
    }

    public void setStoriesBorder() {
        // private final int borderSize = 8;
        final int color = Color.GREEN;
        RoundingParams roundingParams = getHierarchy().getRoundingParams();
        if (roundingParams == null) {
            roundingParams = RoundingParams.asCircle().setRoundingMethod(RoundingParams.RoundingMethod.BITMAP_ONLY);
        }
        roundingParams.setBorder(color, 5.0f);
        getHierarchy().setRoundingParams(roundingParams);
    }

    public enum Size {
        TINY(0),
        SMALL(1),
        REGULAR(2),
        LARGE(3),
        SMALLER(4);

        private final int value;
        private static final Map<Integer, Size> map = new HashMap<>();

        static {
            for (Size size : Size.values()) {
                map.put(size.value, size);
            }
        }

        Size(final int value) {
            this.value = value;
        }

        @NonNull
        public static Size valueOf(final int value) {
            final Size size = map.get(value);
            return size != null ? size : Size.REGULAR;
        }

        public int getValue() {
            return value;
        }
    }

    @Override
    protected void onMeasure(final int widthMeasureSpec, final int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        setMeasuredDimension(dimensionPixelSize, dimensionPixelSize);
    }
}