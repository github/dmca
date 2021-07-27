package awais.instagrabber.customviews;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.util.AttributeSet;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.content.res.AppCompatResources;
import androidx.appcompat.widget.AppCompatTextView;

import awais.instagrabber.R;
import awais.instagrabber.utils.Utils;

public class UsernameTextView extends AppCompatTextView {
    private static final String TAG = UsernameTextView.class.getSimpleName();

    private final int drawableSize = Utils.convertDpToPx(24);

    private boolean verified;
    private VerticalImageSpan verifiedSpan;

    public UsernameTextView(@NonNull final Context context) {
        this(context, null);
    }

    public UsernameTextView(@NonNull final Context context, @Nullable final AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public UsernameTextView(@NonNull final Context context, @Nullable final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        try {
            final Drawable verifiedDrawable = AppCompatResources.getDrawable(getContext(), R.drawable.verified);
            final Drawable drawable = verifiedDrawable.mutate();
            drawable.setBounds(0, 0, drawableSize, drawableSize);
            verifiedSpan = new VerticalImageSpan(drawable);
        } catch (Exception e) {
            Log.e(TAG, "init: ", e);
        }
    }

    public void setUsername(final CharSequence username) {
        setUsername(username, false);
    }

    public void setUsername(final CharSequence username, final boolean verified) {
        this.verified = verified;
        final SpannableStringBuilder sb = new SpannableStringBuilder(username);
        if (verified) {
            try {
                if (verifiedSpan != null) {
                    sb.append("  ");
                    sb.setSpan(verifiedSpan, sb.length() - 1, sb.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                }
            } catch (Exception e) {
                Log.e(TAG, "bind: ", e);
            }
        }
        super.setText(sb);
    }

    public boolean isVerified() {
        return verified;
    }

    public void setVerified(final boolean verified) {
        setUsername(getText(), verified);
    }
}
