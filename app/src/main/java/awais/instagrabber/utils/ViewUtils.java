package awais.instagrabber.utils;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.RoundRectShape;
import android.os.Build;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.ColorInt;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.res.ResourcesCompat;
import androidx.core.util.Pair;
import androidx.dynamicanimation.animation.FloatPropertyCompat;
import androidx.dynamicanimation.animation.SpringAnimation;

import org.jetbrains.annotations.NotNull;

import kotlin.jvm.internal.Intrinsics;

public final class ViewUtils {

    public static final int MATCH_PARENT = -1;
    public static final int WRAP_CONTENT = -2;

    public static Drawable createRoundRectDrawableWithIcon(final Context context, int rad, int iconRes) {
        ShapeDrawable defaultDrawable = new ShapeDrawable(new RoundRectShape(new float[]{rad, rad, rad, rad, rad, rad, rad, rad}, null, null));
        defaultDrawable.getPaint().setColor(0xffffffff);
        final Drawable d = ResourcesCompat.getDrawable(context.getResources(), iconRes, null);
        if (d == null) return null;
        Drawable drawable = d.mutate();
        return new CombinedDrawable(defaultDrawable, drawable);
    }

    public static Drawable createRoundRectDrawable(int rad, int defaultColor) {
        ShapeDrawable defaultDrawable = new ShapeDrawable(new RoundRectShape(new float[]{rad, rad, rad, rad, rad, rad, rad, rad}, null, null));
        defaultDrawable.getPaint().setColor(defaultColor);
        return defaultDrawable;
    }

    public static FrameLayout.LayoutParams createFrame(int width,
                                                       float height,
                                                       int gravity,
                                                       float leftMargin,
                                                       float topMargin,
                                                       float rightMargin,
                                                       float bottomMargin) {
        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(getSize(width), getSize(height), gravity);
        layoutParams.setMargins(Utils.convertDpToPx(leftMargin), Utils.convertDpToPx(topMargin), Utils.convertDpToPx(rightMargin),
                                Utils.convertDpToPx(bottomMargin));
        return layoutParams;
    }

    public static GradientDrawable createGradientDrawable(final GradientDrawable.Orientation orientation,
                                                          @ColorInt final int[] colors) {
        final GradientDrawable drawable = new GradientDrawable(orientation, colors);
        drawable.setShape(GradientDrawable.RECTANGLE);
        return drawable;
    }

    private static int getSize(float size) {
        return (int) (size < 0 ? size : Utils.convertDpToPx(size));
    }

    public static Pair<Integer, Integer> measure(@NonNull final View view, @NonNull final View parent) {
        view.measure(
                View.MeasureSpec.makeMeasureSpec(parent.getWidth(), View.MeasureSpec.UNSPECIFIED),
                View.MeasureSpec.makeMeasureSpec(parent.getHeight(), View.MeasureSpec.UNSPECIFIED)
        );
        return new Pair<>(view.getMeasuredHeight(), view.getMeasuredWidth());
    }

    public static float getTextViewValueWidth(final TextView textView, final String text) {
        return textView.getPaint().measureText(text);
    }

    /**
     * Creates [SpringAnimation] for object.
     * If finalPosition is not [Float.NaN] then create [SpringAnimation] with
     * [SpringForce.mFinalPosition].
     *
     * @param object        Object
     * @param property      object's property to be animated.
     * @param finalPosition [SpringForce.mFinalPosition] Final position of spring.
     * @return [SpringAnimation]
     */
    @NonNull
    public static SpringAnimation springAnimationOf(final Object object,
                                                    final FloatPropertyCompat<Object> property,
                                                    @Nullable final Float finalPosition) {
        return finalPosition == null ? new SpringAnimation(object, property) : new SpringAnimation(object, property, finalPosition);
    }

    public static void suppressLayoutCompat(@NotNull ViewGroup $this$suppressLayoutCompat, boolean suppress) {
        Intrinsics.checkNotNullParameter($this$suppressLayoutCompat, "$this$suppressLayoutCompat");
        if (Build.VERSION.SDK_INT >= 29) {
            $this$suppressLayoutCompat.suppressLayout(suppress);
        } else {
            hiddenSuppressLayout($this$suppressLayoutCompat, suppress);
        }
    }

    private static boolean tryHiddenSuppressLayout = true;

    @SuppressLint({"NewApi"})
    private static void hiddenSuppressLayout(ViewGroup group, boolean suppress) {
        if (tryHiddenSuppressLayout) {
            try {
                group.suppressLayout(suppress);
            } catch (NoSuchMethodError var3) {
                tryHiddenSuppressLayout = false;
            }
        }
    }
}
