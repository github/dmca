package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Collections;
import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageSaturationFilter;

public class SaturationFilter extends Filter<GPUImageSaturationFilter> {
    private static final int PROP_SATURATION = 0;

    private final GPUImageSaturationFilter filter;
    private final Map<Integer, Property<?>> properties;

    public SaturationFilter() {
        super(FiltersHelper.FilterType.SATURATION, R.string.saturation);
        properties = Collections.singletonMap(
                PROP_SATURATION, new FloatProperty(R.string.saturation, 1.0f, 0f, 2.0f)
        );
        filter = new GPUImageSaturationFilter((Float) getProperty(PROP_SATURATION).getDefaultValue());
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return properties;
    }

    @Override
    public void adjust(final int property, final Object value) {
        super.adjust(property, value);
        if (!(value instanceof Float)) return;
        filter.setSaturation((Float) value);
    }

    @Override
    public GPUImageSaturationFilter getInstance() {
        return filter;
    }
}
