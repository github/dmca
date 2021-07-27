package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.custom.GPUImageClarendonFilter;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;

public class ClarendonFilter extends Filter<GPUImageClarendonFilter> {

    private final GPUImageClarendonFilter filter;

    public ClarendonFilter() {
        super(FiltersHelper.FilterType.CLARENDON, R.string.clarendon);
        filter = new GPUImageClarendonFilter();
    }

    @Override
    public GPUImageClarendonFilter getInstance() {
        return filter;
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return null;
    }
}
