// package awais.instagrabber.fragments.imageedit.filters.filters;
//
// import java.util.Collections;
// import java.util.Map;
//
// import awais.instagrabber.R;
// import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
// import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
// import awais.instagrabber.fragments.imageedit.filters.properties.Property;
// import jp.co.cyberagent.android.gpuimage.filter.GPUImageBoxBlurFilter;
//
// public class BoxBlurFilter extends Filter<GPUImageBoxBlurFilter> {
//     private static final int PROP_SIZE = 0;
//
//     private final GPUImageBoxBlurFilter filter;
//     private final Map<Integer, Property<?>> properties;
//
//     public BoxBlurFilter() {
//         super(FiltersHelper.FilterType.BOX_BLUR, R.string.box_blur);
//         properties = Collections.singletonMap(
//                 PROP_SIZE, new FloatProperty(-1, 1f, 1f, 10.0f)
//         );
//         filter = new GPUImageBoxBlurFilter((Float) getProperty(PROP_SIZE).getDefaultValue());
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
//         filter.setBlurSize((Float) value);
//     }
//
//     @Override
//     public GPUImageBoxBlurFilter getInstance() {
//         return filter;
//     }
// }
