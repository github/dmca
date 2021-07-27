package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Collections;
import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageSharpenFilter;

public class SharpenFilter extends Filter<GPUImageSharpenFilter> {
    private static final int PROP_SHARPNESS = 0;

    private final GPUImageSharpenFilter filter;
    private final Map<Integer, Property<?>> properties;

    public SharpenFilter() {
        super(FiltersHelper.FilterType.SHARPEN, R.string.sharpen);
        properties = Collections.singletonMap(
                PROP_SHARPNESS, new FloatProperty(R.string.sharpen, 0f, -0.5f, 0.5f)
        );
        filter = new GPUImageSharpenFilter((Float) getProperty(PROP_SHARPNESS).getDefaultValue());
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return properties;
    }

    @Override
    public void adjust(final int property, final Object value) {
        super.adjust(property, value);
        if (!(value instanceof Float)) return;
        filter.setSharpness((Float) value);
    }

    @Override
    public GPUImageSharpenFilter getInstance() {
        return filter;
    }
}
