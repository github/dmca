package awais.instagrabber.fragments.imageedit.filters.custom;

import jp.co.cyberagent.android.gpuimage.filter.GPUImageBrightnessFilter;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageContrastFilter;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageFilterGroup;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageHueFilter;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageSaturationFilter;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageSepiaToneFilter;

public class GPUImage1977Filter extends GPUImageFilterGroup {
    public GPUImage1977Filter() {
        addFilter(new GPUImageSepiaToneFilter(0.35f));
        addFilter(new GPUImageHueFilter(-30f));
        addFilter(new GPUImageSaturationFilter(1.4f));
    }
}
