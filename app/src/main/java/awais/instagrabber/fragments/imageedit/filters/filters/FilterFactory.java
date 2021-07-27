package awais.instagrabber.fragments.imageedit.filters.filters;

import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;

public final class FilterFactory {

    public static Filter<?> getInstance(final FiltersHelper.FilterType type) {
        switch (type) {
            case BRIGHTNESS:
                return new BrightnessFilter();
            case CONTRAST:
                return new ContrastFilter();
            case VIBRANCE:
                return new VibranceFilter();
            case SATURATION:
                return new SaturationFilter();
            case SHARPEN:
                return new SharpenFilter();
            case EXPOSURE:
                return new ExposureFilter();
            case NORMAL:
                return new NormalFilter();
            case SEPIA:
                return new SepiaToneFilter();
            case CLARENDON:
                return new ClarendonFilter();
            case ONE977:
                return new One977Filter();
            case ADEN:
                return new AdenFilter();
            // case BULGE_DISTORTION:
            //     break;
            // case CGA_COLORSPACE:
            //     break;
            // case COLOR_BALANCE:
            //     break;
            // case CROSSHATCH:
            //     break;
            // case DILATION:
            //     break;
            // case EMBOSS:
            //     break;
            // case FALSE_COLOR:
            //     break;
            // case GAMMA:
            //     break;
            // case GAUSSIAN_BLUR:
            //     break;
            // case GLASS_SPHERE:
            //     break;
            // case GRAYSCALE:
            //     break;
            // case HALFTONE:
            //     break;
            // case HAZE:
            //     break;
            // case HIGHLIGHT_SHADOW:
            //     break;
            // case HUE:
            //     break;
            // case INVERT:
            //     break;
            // case KUWAHARA:
            //     break;
            // case LAPLACIAN:
            //     break;
            // case LEVELS_FILTER_MIN:
            //     break;
            // case LOOKUP_AMATORKA:
            //     break;
            // case LUMINANCE:
            //     break;
            // case LUMINANCE_THRESHOLD:
            //     break;
            // case MONOCHROME:
            //     break;
            // case NON_MAXIMUM_SUPPRESSION:
            //     break;
            // case OPACITY:
            //     break;
            // case PIXELATION:
            //     break;
            // case POSTERIZE:
            //     break;
            // case RGB:
            //     break;
            // case RGB_DILATION:
            //     break;
            // case SKETCH:
            //     break;
            // case SMOOTH_TOON:
            //     break;
            // case SOBEL_EDGE_DETECTION:
            //     break;
            // case SOLARIZE:
            //     break;
            // case SPHERE_REFRACTION:
            //     break;
            // case SWIRL:
            //     break;
            // case THREE_X_THREE_CONVOLUTION:
            //     break;
            // case THRESHOLD_EDGE_DETECTION:
            //     break;
            // case TONE_CURVE:
            //     break;
            // case TOON:
            //     break;
            // case TRANSFORM2D:
            //     break;
            // case WEAK_PIXEL_INCLUSION:
            //     break;
            // case WHITE_BALANCE:
            //     break;
            // case ZOOM_BLUR:
            //     break;
            case VIGNETTE:
                return new VignetteFilter();
            // case BILATERAL_BLUR:
            //     return new BilateralBlurFilter();
            // case BOX_BLUR:
            //     return new BoxBlurFilter();
        }
        return null;
    }
}
