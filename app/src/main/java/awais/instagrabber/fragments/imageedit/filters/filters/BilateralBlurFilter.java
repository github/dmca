// package awais.instagrabber.fragments.imageedit.filters.filters;
//
// import java.util.Collections;
// import java.util.Map;
//
// import awais.instagrabber.R;
// import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
// import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
// import awais.instagrabber.fragments.imageedit.filters.properties.Property;
// import jp.co.cyberagent.android.gpuimage.filter.GPUImageBilateralBlurFilter;
//
// public class BilateralBlurFilter extends Filter<GPUImageBilateralBlurFilter> {
//     private static final int PROP_DISTANCE = 0;
//
//     private final GPUImageBilateralBlurFilter filter;
//     private final Map<Integer, Property<?>> properties;
//
//     public BilateralBlurFilter() {
//         super(FiltersHelper.FilterType.BILATERAL_BLUR, R.string.bilateral_blur);
//         properties = Collections.singletonMap(
//                 PROP_DISTANCE, new FloatProperty(-1, 8f, 0f, 15.0f)
//         );
//         filter = new GPUImageBilateralBlurFilter((Float) getProperty(PROP_DISTANCE).getDefaultValue());
//     }
//
//     @Override
//     public Map<Integer, Property<?>> getProperties() {
//         return properties;
//     }
//
//     @Override
//     public void adjust(final int property, final Object value) {
//         super.adjust(property, value);
//         if (!(value instanceof Float)) return;
//         filter.setDistanceNormalizationFactor((Float) value);
//     }
//
//     @Override
//     public GPUImageBilateralBlurFilter getInstance() {
//         return filter;
//     }
// }
