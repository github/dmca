package awais.instagrabber.fragments.imageedit.filters.filters;

import android.graphics.Color;
import android.graphics.PointF;

import com.google.common.collect.ImmutableMap;

import java.util.Map;

import awais.instagrabber.R;
import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.fragments.imageedit.filters.properties.ColorProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.FloatProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.PointFProperty;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageVignetteFilter;

public class VignetteFilter extends Filter<GPUImageVignetteFilter> {
    private static final int PROP_CENTER = 0;
    private static final int PROP_COLOR = 1;
    private static final int PROP_START = 2;
    private static final int PROP_END = 3;

    private final GPUImageVignetteFilter filter;
    private final Map<Integer, Property<?>> properties;

    public VignetteFilter() {
        super(FiltersHelper.FilterType.VIGNETTE, R.string.vignette);
        properties = ImmutableMap.of(
                PROP_CENTER, new PointFProperty(R.string.center, new PointF(0.5f, 0.5f)),
                PROP_COLOR, new ColorProperty(R.string.color, Color.BLACK),
                PROP_START, new FloatProperty(R.string.start, 0.3f),
                PROP_END, new FloatProperty(R.string.end, 0.75f)
        );
        filter = new GPUImageVignetteFilter(
                (PointF) getProperty(PROP_CENTER).getDefaultValue(),
                getFloatArrayFromColor((Integer) getProperty(PROP_COLOR).getDefaultValue()),
                (Float) getProperty(PROP_START).getDefaultValue(),
                (Float) getProperty(PROP_END).getDefaultValue()
        );
    }

    @Override
    public Map<Integer, Property<?>> getProperties() {
        return properties;
    }

    @Override
    public void adjust(final int property, final Object value) {
        super.adjust(property, value);
        switch (property) {
            case PROP_CENTER:
                filter.setVignetteCenter((PointF) value);
                return;
            case PROP_COLOR:
                final int color = (int) value;
                filter.setVignetteColor(getFloatArrayFromColor(color));
                return;
            case PROP_START:
                filter.setVignetteStart((float) value);
                return;
            case PROP_END:
                filter.setVignetteEnd((float) value);
                return;
            default:
        }
    }

    private float[] getFloatArrayFromColor(final int color) {
        return new float[]{Color.red(color) / 255f, Color.green(color) / 255f, Color.blue(color) / 255f};
    }

    @Override
    public GPUImageVignetteFilter getInstance() {
        return filter;
    }
}
