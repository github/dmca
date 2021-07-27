package awais.instagrabber.fragments.imageedit.filters.properties;

import androidx.annotation.StringRes;

/**
 * Min and Max values do not matter here
 */
public class ColorProperty extends Property<Integer> {
    private final int label;
    private final int defaultValue;

    public ColorProperty(@StringRes final int label,
                         final int defaultValue) {
        this.label = label;
        this.defaultValue = defaultValue;
    }

    @Override
    public int getLabel() {
        return label;
    }

    @Override
    public Integer getDefaultValue() {
        return defaultValue;
    }

    @Override
    public Integer getMinValue() {
        return defaultValue;
    }

    @Override
    public Integer getMaxValue() {
        return defaultValue;
    }
}
