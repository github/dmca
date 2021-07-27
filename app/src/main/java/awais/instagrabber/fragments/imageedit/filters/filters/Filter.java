package awais.instagrabber.fragments.imageedit.filters.filters;

import androidx.annotation.CallSuper;
import androidx.annotation.StringRes;

import java.util.Map;
import java.util.Set;

import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageFilter;

public abstract class Filter<T extends GPUImageFilter> {
    private final FiltersHelper.FilterType type;
    private final int label;

    public Filter(final FiltersHelper.FilterType type, @StringRes final int label) {
        this.type = type;
        this.label = label;
    }

    public FiltersHelper.FilterType getType() {
        return type;
    }

    @StringRes
    public int getLabel() {
        return label;
    }

    public abstract T getInstance();

    public abstract Map<Integer, Property<?>> getProperties();

    public Property<?> getProperty(int property) {
        return getProperties().get(property);
    }

    @CallSuper
    public void adjust(final int property, final Object value) {
        final Property<?> propertyObj = getProperty(property);
        propertyObj.setValue(value);
    }

    public void reset() {
        final Map<Integer, Property<?>> propertyMap = getProperties();
        if (propertyMap == null) return;
        final Set<Map.Entry<Integer, Property<?>>> entries = propertyMap.entrySet();
        for (final Map.Entry<Integer, Property<?>> entry : entries) {
            adjust(entry.getKey(), entry.getValue().getDefaultValue());
        }
    }
}
