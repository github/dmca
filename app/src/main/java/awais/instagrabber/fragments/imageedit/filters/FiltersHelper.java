package awais.instagrabber.fragments.imageedit.filters;

import com.google.common.collect.ImmutableList;

import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.fragments.imageedit.filters.filters.Filter;
import awais.instagrabber.fragments.imageedit.filters.filters.FilterFactory;

public final class FiltersHelper {

    public static List<Filter<?>> getTuneFilters() {
        return TUNING_FILTERS.stream()
                             .map(FilterFactory::getInstance)
                             .filter(Objects::nonNull)
                             .collect(Collectors.toList());
    }

    public static List<Filter<?>> getFilters() {
        // Return all non-tuning filters
        return Arrays.stream(FilterType.values())
                     .filter(filterType -> !TUNING_FILTERS.contains(filterType))
                     .map(FilterFactory::getInstance)
                     .filter(Objects::nonNull)
                     .collect(Collectors.toList());
    }

    public static final List<FilterType> TUNING_FILTERS = ImmutableList.of(
            FilterType.BRIGHTNESS,
            FilterType.CONTRAST,
            FilterType.VIBRANCE,
            FilterType.SATURATION,
            FilterType.SHARPEN,
            FilterType.EXPOSURE
    );

    public enum FilterType {
        // Tune
        BRIGHTNESS,
        CONTRAST,
        VIBRANCE,
        SATURATION,
        SHARPEN,
        EXPOSURE,

        // Filters
        NORMAL,
        SEPIA,
        CLARENDON,
        ONE977,
        ADEN,
        VIGNETTE,
        // BILATERAL_BLUR,
        // BOX_BLUR,
        // BULGE_DISTORTION,
        // CGA_COLORSPACE,
        // COLOR_BALANCE,
        // CROSSHATCH,
        // DILATION,
        // EMBOSS,
        // FALSE_COLOR,
        // GAMMA,
        // GAUSSIAN_BLUR,
        // GLASS_SPHERE,
        // GRAYSCALE,
        // HALFTONE,
        // HAZE,
        // HIGHLIGHT_SHADOW,
        // HUE,
        // INVERT,
        // KUWAHARA,
        // LAPLACIAN,
        // LEVELS_FILTER_MIN,
        // LOOKUP_AMATORKA,
        // LUMINANCE,
        // LUMINANCE_THRESHOLD,
        // MONOCHROME,
        // NON_MAXIMUM_SUPPRESSION,
        // OPACITY,
        // PIXELATION,
        // POSTERIZE,
        // RGB,
        // RGB_DILATION,
        // SKETCH,
        // SMOOTH_TOON,
        // SOBEL_EDGE_DETECTION,
        // SOLARIZE,
        // SPHERE_REFRACTION,
        // SWIRL,
        // THREE_X_THREE_CONVOLUTION,
        // THRESHOLD_EDGE_DETECTION,
        // TONE_CURVE,
        // TOON,
        // TRANSFORM2D,
        // WEAK_PIXEL_INCLUSION,
        // ZOOM_BLUR

        // Can be separate tunings
        // WHITE_BALANCE,

        // BLEND_ADD,
        // BLEND_ALPHA,
        // BLEND_CHROMA_KEY,
        // BLEND_COLOR,
        // BLEND_COLOR_BURN,
        // BLEND_COLOR_DODGE,
        // BLEND_DARKEN,
        // BLEND_DIFFERENCE,
        // BLEND_DISSOLVE,
        // BLEND_DIVIDE,
        // BLEND_EXCLUSION,
        // BLEND_HARD_LIGHT,
        // BLEND_HUE,
        // BLEND_LIGHTEN,
        // BLEND_LINEAR_BURN,
        // BLEND_LUMINOSITY,
        // BLEND_MULTIPLY,
        // BLEND_NORMAL,
        // BLEND_OVERLAY,
        // BLEND_SATURATION,
        // BLEND_SCREEN,
        // BLEND_SOFT_LIGHT,
        // BLEND_SOURCE_OVER,
        // BLEND_SUBTRACT,
    }
}
