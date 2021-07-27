package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Collections;
import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageContrastFilter;

public class ContrastFilter extends Filter<GPUImageContrastFilter> {
    private static final int PROP_CONTRAST = 0;

    private final GPUImageContrastFilter filter;
    private final Map<Integer, Property<?>> properties;

    public ContrastFilter() {
        super(FiltersHelper.FilterType.CONTRAST, R.string.contrast);
        properties = Collections.singletonMap(
                PROP_CONTRAST, new FloatProperty(R.string.contrast, 1.0f, 0.0f, 4.0f)
        );
        filter = new GPUImageContrastFilter((Float) getProperty(PROP_CONTRAST).getDefaultValue());
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return properties;
    }

    @Override
    public void adjust(final int property, final Object value) {
        super.adjust(property, value);
        if (!(value instanceof Float)) return;
        filter.setContrast((Float) value);
    }

    @Override
    public GPUImageContrastFilter getInstance() {
        return filter;
    }
}
