package awais.instagrabber.customviews;

import android.annotation.SuppressLint;
import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.google.android.material.button.MaterialButton;

import awais.instagrabber.animations.ScaleAnimation;

/**
 * Created by Devlomi on 13/12/2017.
 */

public class RecordButton extends MaterialButton implements View.OnTouchListener, View.OnClickListener, View.OnLongClickListener {

    private ScaleAnimation scaleAnimation;
    private RecordView recordView;
    private boolean listenForRecord = true;
    private OnRecordClickListener onRecordClickListener;
    private OnRecordLongClickListener onRecordLongClickListener;

    public RecordButton(Context context) {
        super(context);
        init(context, null);
    }

    public RecordButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context, attrs);
    }

    public RecordButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context, attrs);
    }

    @SuppressLint("ClickableViewAccessibility")
    private void init(Context context, AttributeSet attrs) {
        scaleAnimation = new ScaleAnimation(this);
        this.setOnTouchListener(this);
        this.setOnClickListener(this);
        this.setOnLongClickListener(this);
    }

    public void setRecordView(RecordView recordView) {
        this.recordView = recordView;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (isListenForRecord()) {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    recordView.onActionDown((RecordButton) v, event);
                    break;
                case MotionEvent.ACTION_MOVE:
                    recordView.onActionMove((RecordButton) v, event, false);
                    break;
                case MotionEvent.ACTION_UP:
                    recordView.onActionUp((RecordButton) v);
                    break;
            }
        }
        return isListenForRecord();
    }

    protected void startScale() {
        scaleAnimation.start();
    }

    public void stopScale() {
        scaleAnimation.stop();
    }

    public void setListenForRecord(boolean listenForRecord) {
        this.listenForRecord = listenForRecord;
    }

    public boolean isListenForRecord() {
        return listenForRecord;
    }

    public void setOnRecordClickListener(OnRecordClickListener onRecordClickListener) {
        this.onRecordClickListener = onRecordClickListener;
    }

    public void setOnRecordLongClickListener(OnRecordLongClickListener onRecordLongClickListener) {
        this.onRecordLongClickListener = onRecordLongClickListener;
    }

    @Override
    public void onClick(View v) {
        if (onRecordClickListener != null) {
            onRecordClickListener.onClick(v);
        }
    }

    @Override
    public boolean onLongClick(final View v) {
        if (onRecordLongClickListener != null) {
            return onRecordLongClickListener.onLongClick(v);
        }
        return false;
    }

    public interface OnRecordClickListener {
        void onClick(View v);
    }

    public interface OnRecordLongClickListener {
        boolean onLongClick(View v);
    }
}
