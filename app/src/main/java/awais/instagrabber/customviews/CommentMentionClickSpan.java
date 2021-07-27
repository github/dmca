package awais.instagrabber.customviews;

import android.text.TextPaint;
import android.text.style.ClickableSpan;
import android.view.View;

import androidx.annotation.NonNull;

public final class CommentMentionClickSpan extends ClickableSpan {
    @Override
    public void onClick(@NonNull final View widget) { }

    @Override
    public void updateDrawState(@NonNull final TextPaint ds) {
        ds.setColor(ds.linkColor);
    }
}