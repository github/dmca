package awais.instagrabber.fragments.imageedit.filters.properties;

import android.util.Log;

import androidx.annotation.StringRes;

public abstract class Property<T> {
    private static final String TAG = Property.class.getSimpleName();
    protected T value;

    @StringRes
    public abstract int getLabel();

    public abstract T getDefaultValue();

    public abstract T getMinValue();

    public abstract T getMaxValue();

    public T getValue() {
        return value;
    }

    public void setValue(final Object value) {
        try {
            //noinspection unchecked
            this.value = (T) value;
        } catch (ClassCastException e) {
            Log.e(TAG, "setValue: ", e);
        }
    }

    public void reset() {
        setValue(getDefaultValue());
    }
}
