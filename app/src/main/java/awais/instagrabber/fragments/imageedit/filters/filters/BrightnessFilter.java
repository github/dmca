package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Collections;
import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageBrightnessFilter;

public class BrightnessFilter extends Filter<GPUImageBrightnessFilter> {
    private static final int PROP_BRIGHTNESS = 0;

    private final GPUImageBrightnessFilter filter;
    private final Map<Integer, Property<?>> properties;

    public BrightnessFilter() {
        super(FiltersHelper.FilterType.BRIGHTNESS, R.string.brightness);
        properties = Collections.singletonMap(
                PROP_BRIGHTNESS, new FloatProperty(R.string.brightness, 0.0f, -1.0f, 1.0f)
        );
        filter = new GPUImageBrightnessFilter((Float) getProperty(PROP_BRIGHTNESS).getDefaultValue());
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return properties;
    }

    @Override
    public void adjust(final int property, final Object value) {
        super.adjust(property, value);
        if (!(value instanceof Float)) return;
        filter.setBrightness((Float) value);
    }

    @Override
    public GPUImageBrightnessFilter getInstance() {
        return filter;
    }
}
