package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Collections;
import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageSepiaToneFilter;

public class SepiaToneFilter extends Filter<GPUImageSepiaToneFilter> {
    private static final int PROP_INTENSITY = 0;

    private final GPUImageSepiaToneFilter filter;
    private final Map<Integer, Property<?>> properties;

    public SepiaToneFilter() {
        super(FiltersHelper.FilterType.SEPIA, R.string.sepia);
        properties = Collections.singletonMap(
                PROP_INTENSITY, new FloatProperty(-1, 1f, 1f, 10.0f)
        );
        filter = new GPUImageSepiaToneFilter((Float) getProperty(PROP_INTENSITY).getDefaultValue());
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return properties;
    }

    @Override
    public void adjust(final int property, final Object value) {
        super.adjust(property, value);
        if (!(value instanceof Float)) return;
        filter.setIntensity((Float) value);
    }

    @Override
    public GPUImageSepiaToneFilter getInstance() {
        return filter;
    }
}
