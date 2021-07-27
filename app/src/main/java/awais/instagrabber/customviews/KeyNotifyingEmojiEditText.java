package awais.instagrabber.customviews;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;

import androidx.emoji.widget.EmojiEditText;

public class KeyNotifyingEmojiEditText extends EmojiEditText {
    private OnKeyEventListener onKeyEventListener;

    public KeyNotifyingEmojiEditText(final Context context) {
        super(context);
    }

    public KeyNotifyingEmojiEditText(final Context context, final AttributeSet attrs) {
        super(context, attrs);
    }

    public KeyNotifyingEmojiEditText(final Context context, final AttributeSet attrs, final int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public KeyNotifyingEmojiEditText(final Context context, final AttributeSet attrs, final int defStyleAttr, final int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    @Override
    public boolean onKeyPreIme(final int keyCode, final KeyEvent event) {
        if (onKeyEventListener != null) {
            final boolean listenerResult = onKeyEventListener.onKeyPreIme(keyCode, event);
            if (listenerResult) return true;
        }
        return super.onKeyPreIme(keyCode, event);
    }

    public void setOnKeyEventListener(final OnKeyEventListener onKeyEventListener) {
        this.onKeyEventListener = onKeyEventListener;
    }

    public interface OnKeyEventListener {
        boolean onKeyPreIme(int keyCode, KeyEvent keyEvent);
    }
}
