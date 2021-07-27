package awais.instagrabber.fragments.imageedit.filters.properties;

import androidx.annotation.StringRes;

public class FloatProperty extends Property<Float> {

    private final int label;
    private final float defaultValue;
    private final float minValue;
    private final float maxValue;

    public FloatProperty(@StringRes final int label,
                         final float defaultValue,
                         final float minValue,
                         final float maxValue) {

        this.label = label;
        this.defaultValue = defaultValue;
        this.minValue = minValue;
        this.maxValue = maxValue;
    }

    public FloatProperty(@StringRes final int label, final float value) {
        this.label = label;
        this.defaultValue = value;
        this.minValue = value;
        this.maxValue = value;
    }

    @Override
    public int getLabel() {
        return label;
    }

    @Override
    public Float getDefaultValue() {
        return defaultValue;
    }

    @Override
    public Float getMinValue() {
        return minValue;
    }

    @Override
    public Float getMaxValue() {
        return maxValue;
    }
}
