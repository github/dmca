package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Collections;
import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageExposureFilter;

public class ExposureFilter extends Filter<GPUImageExposureFilter> {
    private static final int PROP_EXPOSURE = 0;

    private final GPUImageExposureFilter filter;
    private final Map<Integer, Property<?>> properties;

    public ExposureFilter() {
        super(FiltersHelper.FilterType.EXPOSURE, R.string.exposure);
        properties = Collections.singletonMap(
                PROP_EXPOSURE, new FloatProperty(R.string.exposure, 0f, -3.0f, 3.0f)
        );
        filter = new GPUImageExposureFilter((Float) getProperty(PROP_EXPOSURE).getDefaultValue());
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return properties;
    }

    @Override
    public void adjust(final int property, final Object value) {
        super.adjust(property, value);
        if (!(value instanceof Float)) return;
        filter.setExposure((Float) value);
    }

    @Override
    public GPUImageExposureFilter getInstance() {
        return filter;
    }
}
