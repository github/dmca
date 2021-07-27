package awais.instagrabber.customviews;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Color;
import android.util.AttributeSet;

import androidx.annotation.Nullable;

import com.facebook.drawee.drawable.ScalingUtils;
import com.facebook.drawee.generic.GenericDraweeHierarchy;
import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.drawee.generic.GenericDraweeHierarchyInflater;
import com.facebook.drawee.generic.RoundingParams;
import com.facebook.drawee.view.SimpleDraweeView;

import awais.instagrabber.R;

public class CircularImageView extends SimpleDraweeView {
    public CircularImageView(Context context, GenericDraweeHierarchy hierarchy) {
        super(context);
        setHierarchy(hierarchy);
    }

    public CircularImageView(final Context context) {
        super(context);
        inflateHierarchy(context, null);
    }

    public CircularImageView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
        inflateHierarchy(context, attrs);
    }

    public CircularImageView(final Context context, final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        inflateHierarchy(context, attrs);
    }

    protected void inflateHierarchy(Context context, @Nullable AttributeSet attrs) {
        Resources resources = context.getResources();
        final RoundingParams roundingParams = RoundingParams.asCircle();
        GenericDraweeHierarchyBuilder builder = new GenericDraweeHierarchyBuilder(resources)
                .setRoundingParams(roundingParams)
                .setActualImageScaleType(ScalingUtils.ScaleType.FIT_CENTER);
        GenericDraweeHierarchyInflater.updateBuilder(builder, context, attrs);
        setAspectRatio(builder.getDesiredAspectRatio());
        setHierarchy(builder.build());
        setBackgroundResource(R.drawable.shape_oval_light);
    }

    /* types: 0 clear, 1 green (feed bestie / has story), 2 red (live) */
    public void setStoriesBorder(final int type) {
        // private final int borderSize = 8;
        final int color = type == 2 ? Color.RED : Color.GREEN;
        RoundingParams roundingParams = getHierarchy().getRoundingParams();
        if (roundingParams == null) {
            roundingParams = RoundingParams.asCircle().setRoundingMethod(RoundingParams.RoundingMethod.BITMAP_ONLY);
        }
        roundingParams.setBorder(color, type == 0 ? 0f : 5.0f);
        getHierarchy().setRoundingParams(roundingParams);
    }
}