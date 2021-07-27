package awais.instagrabber.fragments.imageedit.filters.properties;

import android.graphics.PointF;

import androidx.annotation.StringRes;

/**
 * Min and Max values do not matter here
 */
public class PointFProperty extends Property<PointF> {
    private final int label;
    private final PointF defaultValue;

    public PointFProperty(@StringRes final int label,
                          final PointF defaultValue) {
        this.label = label;
        this.defaultValue = defaultValue;
    }

    @Override
    public int getLabel() {
        return label;
    }

    @Override
    public PointF getDefaultValue() {
        return defaultValue;
    }

    @Override
    public PointF getMinValue() {
        return defaultValue;
    }

    @Override
    public PointF getMaxValue() {
        return defaultValue;
    }
}
