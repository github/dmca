package awais.instagrabber.fragments.imageedit.filters.filters;

import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.custom.GPUImage1977Filter;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;

public class One977Filter extends Filter<GPUImage1977Filter> {

    private final GPUImage1977Filter filter;

    public One977Filter() {
        super(FiltersHelper.FilterType.ONE977, R.string.one977);
        filter = new GPUImage1977Filter();
    }

    @Override
    public GPUImage1977Filter getInstance() {
        return filter;
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return null;
    }
}
