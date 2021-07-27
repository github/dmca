package awais.instagrabber.customviews;

import android.content.Context;
import android.util.AttributeSet;

import androidx.appcompat.widget.AppCompatImageView;

public final class FixedImageView extends AppCompatImageView {
    public FixedImageView(final Context context) {
        super(context);
    }

    public FixedImageView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
    }

    public FixedImageView(final Context context, final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onMeasure(final int wMeasure, final int hMeasure) {
        super.onMeasure(wMeasure, wMeasure);
    }
}