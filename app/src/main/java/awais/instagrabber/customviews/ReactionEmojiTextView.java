package awais.instagrabber.customviews;

import android.annotation.SuppressLint;
import android.content.Context;
import android.text.SpannableString;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.util.AttributeSet;

import androidx.annotation.NonNull;
import androidx.emoji.widget.EmojiAppCompatTextView;

import java.util.List;
import java.util.stream.Collectors;

public class ReactionEmojiTextView extends EmojiAppCompatTextView {
    private static final String TAG = ReactionEmojiTextView.class.getSimpleName();

    private final SpannableStringBuilder spannableStringBuilder = new SpannableStringBuilder();

    private String count = "";
    private SpannableString ellipsisSpannable;
    private String distinctEmojis;

    public ReactionEmojiTextView(final Context context) {
        super(context);
        init();
    }

    public ReactionEmojiTextView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public ReactionEmojiTextView(final Context context, final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        ellipsisSpannable = new SpannableString(count);
    }

    @SuppressLint("SetTextI18n")
    public void setEmojis(@NonNull final List<String> emojis) {
        count = String.valueOf(emojis.size());
        distinctEmojis = emojis.stream()
                               .distinct()
                               .collect(Collectors.joining());
        ellipsisSpannable = new SpannableString(count);
        setText(distinctEmojis + (emojis.size() > 1 ? count : ""));
    }

    @Override
    protected void onMeasure(final int widthMeasureSpec, final int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        final CharSequence text = getText();
        if (text == null) return;
        final int measuredWidth = getMeasuredWidth();
        float availableTextWidth = measuredWidth - getCompoundPaddingLeft() - getCompoundPaddingRight();
        CharSequence ellipsizedText = TextUtils.ellipsize(text, getPaint(), availableTextWidth, getEllipsize());
        if (!ellipsizedText.toString().equals(text.toString())) {
            // If the ellipsizedText is different than the original text, this means that it didn't fit and got indeed ellipsized.
            // Calculate the new availableTextWidth by taking into consideration the size of the custom ellipsis, too.
            availableTextWidth = (availableTextWidth - getPaint().measureText(count));
            ellipsizedText = TextUtils.ellipsize(text, getPaint(), availableTextWidth, getEllipsize());
            final int defaultEllipsisStart = ellipsizedText.toString().indexOf(getDefaultEllipsis());
            final int defaultEllipsisEnd = defaultEllipsisStart + 1;
            spannableStringBuilder.clear();
            // Update the text with the ellipsized version and replace the default ellipsis with the custom one.
            final SpannableStringBuilder replace = spannableStringBuilder.append(ellipsizedText)
                                                                         .replace(defaultEllipsisStart, defaultEllipsisEnd, ellipsisSpannable);
            setText(replace);
            super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        }
    }

    private char getDefaultEllipsis() {
        return '…';
    }

}
