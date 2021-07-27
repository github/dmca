package awais.instagrabber.customviews.helpers;

import android.graphics.drawable.Animatable;
import android.view.ViewGroup;

import com.facebook.drawee.controller.BaseControllerListener;
import com.facebook.drawee.generic.GenericDraweeHierarchy;
import com.facebook.drawee.view.DraweeView;
import com.facebook.imagepipeline.image.ImageInfo;

import awais.instagrabber.utils.NumberUtils;

public class ImageResizingControllerListener<T extends DraweeView<GenericDraweeHierarchy>> extends BaseControllerListener<ImageInfo> {
    private static final String TAG = "ImageResizingController";

    private T imageView;
    private final int requiredWidth;

    public ImageResizingControllerListener(final T imageView, final int requiredWidth) {
        this.imageView = imageView;
        this.requiredWidth = requiredWidth;
    }

    @Override
    public void onIntermediateImageSet(final String id, final ImageInfo imageInfo) {
        super.onIntermediateImageSet(id, imageInfo);
    }

    public void onFinalImageSet(String id, ImageInfo imageInfo, Animatable animatable) {
        if (imageInfo != null) {
            // updateViewSize(imageInfo);
            final int height = imageInfo.getHeight();
            final int width = imageInfo.getWidth();
            // final float aspectRatio = ((float) width) / height;
            final ViewGroup.LayoutParams layoutParams = imageView.getLayoutParams();
            // final int deviceWidth = Utils.displayMetrics.widthPixels;
            final int resultingHeight = NumberUtils.getResultingHeight(requiredWidth, height, width);
            layoutParams.width = requiredWidth;
            layoutParams.height = resultingHeight;
            imageView.requestLayout();
        }
    }
}