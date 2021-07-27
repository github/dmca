package awaisomereport;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.ImageSpan;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.lang.ref.WeakReference;

import awais.instagrabber.R;

public final class ErrorReporterActivity extends Activity implements View.OnClickListener {
    private View btnReport;

    @Override
    protected void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_crash_error);

        setFinishOnTouchOutside(false);
        getWindow().setLayout(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);

        final SpannableString crashTitle = new SpannableString("   " + getString(R.string.crash_title));
        crashTitle.setSpan(new CenteredImageSpan(this, android.R.drawable.stat_notify_error),
                0, 1, Spannable.SPAN_INCLUSIVE_EXCLUSIVE);
        setTitle(crashTitle);

        btnReport = findViewById(R.id.btnReport);
        btnReport.setOnClickListener(this);
        findViewById(R.id.btnCancel).setOnClickListener(this);
    }

    @Override
    public void onClick(@NonNull final View v) {
        if (v == btnReport) {
            CrashReporterHelper.startCrashEmailIntent(this);
        }
        finish();
        System.exit(10);
    }

    public static class CenteredImageSpan extends ImageSpan {
        private WeakReference<Drawable> drawable;

        public CenteredImageSpan(final Context context, final int drawableRes) {
            super(context, drawableRes);
        }

        @Override
        public int getSize(@NonNull final Paint paint, final CharSequence text, final int start, final int end, @Nullable final Paint.FontMetricsInt fm) {
            final Drawable drawable = getCachedDrawable();
            final Rect rect = drawable.getBounds();

            if (fm != null) {
                final Paint.FontMetricsInt pfm = paint.getFontMetricsInt();
                fm.ascent = pfm.ascent;
                fm.descent = pfm.descent;
                fm.top = pfm.top;
                fm.bottom = pfm.bottom;
            }

            return rect.right;
        }

        @Override
        public void draw(@NonNull final Canvas canvas, final CharSequence text, final int start, final int end, final float x, final int top,
                         final int y, final int bottom, @NonNull final Paint paint) {
            final Drawable drawable = getCachedDrawable();
            canvas.save();

            final int drawableHeight = drawable.getIntrinsicHeight();
            final Paint.FontMetricsInt fontMetricsInt = paint.getFontMetricsInt();
            int transY = bottom - drawable.getBounds().bottom + (drawableHeight - fontMetricsInt.descent + fontMetricsInt.ascent) / 2;

            canvas.translate(x, transY);
            drawable.draw(canvas);
            canvas.restore();
        }

        private Drawable getCachedDrawable() {
            Drawable d = null;
            if (drawable != null) d = drawable.get();
            if (d == null) {
                d = getDrawable();
                drawable = new WeakReference<>(d);
            }
            return d;
        }
    }
}

