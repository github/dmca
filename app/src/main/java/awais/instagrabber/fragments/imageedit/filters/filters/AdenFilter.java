package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.custom.GPUImageAdenFilter;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;

public class AdenFilter extends Filter<GPUImageAdenFilter> {

    private final GPUImageAdenFilter filter;

    public AdenFilter() {
        super(FiltersHelper.FilterType.ADEN, R.string.aden);
        filter = new GPUImageAdenFilter();
    }

    @Override
    public GPUImageAdenFilter getInstance() {
        return filter;
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return null;
    }
}
