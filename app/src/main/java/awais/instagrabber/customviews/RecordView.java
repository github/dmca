package awais.instagrabber.customviews;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.media.MediaPlayer;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.widget.RelativeLayout;

import androidx.annotation.ColorInt;
import androidx.annotation.DrawableRes;
import androidx.annotation.Nullable;
import androidx.appcompat.content.res.AppCompatResources;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.graphics.drawable.DrawableCompat;

import java.io.IOException;

import awais.instagrabber.R;
import awais.instagrabber.customviews.helpers.RecordViewAnimationHelper;
import awais.instagrabber.databinding.RecordViewLayoutBinding;
import awais.instagrabber.utils.Utils;

/**
 * Created by Devlomi on 24/08/2017.
 */

public class RecordView extends RelativeLayout {
    private static final String TAG = RecordView.class.getSimpleName();

    public static final int DEFAULT_CANCEL_BOUNDS = 8; //8dp
    // private ImageView smallBlinkingMic;
    // private ImageView basketImg;
    // private Chronometer counterTime;
    // private TextView slideToCancel;
    // private LinearLayout slideToCancelLayout;
    private float initialX;
    private float basketInitialY;
    private float difX = 0;
    private float cancelBounds = DEFAULT_CANCEL_BOUNDS;
    private long startTime;
    private final Context context;
    private OnRecordListener onRecordListener;
    private boolean isSwiped;
    private boolean isLessThanMinAllowed = false;
    private boolean isSoundEnabled = true;
    private int RECORD_START = R.raw.record_start;
    private int RECORD_FINISHED = R.raw.record_finished;
    private int RECORD_ERROR = R.raw.record_error;
    private RecordViewAnimationHelper recordViewAnimationHelper;
    private RecordViewLayoutBinding binding;
    private int minMillis = 1000;


    public RecordView(Context context) {
        super(context);
        this.context = context;
        init(context, null, -1, -1);
    }


    public RecordView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
        init(context, attrs, -1, -1);
    }

    public RecordView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        this.context = context;
        init(context, attrs, defStyleAttr, -1);
    }

    private void init(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        binding = RecordViewLayoutBinding.inflate(LayoutInflater.from(context), this, false);
        addView(binding.getRoot());
        hideViews(true);
        if (attrs != null && defStyleAttr == -1 && defStyleRes == -1) {
            TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.RecordView, defStyleAttr, defStyleRes);
            int slideArrowResource = typedArray.getResourceId(R.styleable.RecordView_slide_to_cancel_arrow, -1);
            String slideToCancelText = typedArray.getString(R.styleable.RecordView_slide_to_cancel_text);
            int slideToCancelTextColor = typedArray.getResourceId(R.styleable.RecordView_slide_to_cancel_text_color, -1);
            int slideMarginRight = (int) typedArray.getDimension(R.styleable.RecordView_slide_to_cancel_margin_right, 30);
            int counterTimeColor = typedArray.getResourceId(R.styleable.RecordView_counter_time_color, -1);
            int arrowColor = typedArray.getResourceId(R.styleable.RecordView_slide_to_cancel_arrow_color, -1);
            int cancelBounds = typedArray.getDimensionPixelSize(R.styleable.RecordView_slide_to_cancel_bounds, -1);
            if (cancelBounds != -1) {
                setCancelBounds(cancelBounds, false);//don't convert it to pixels since it's already in pixels
            }
            if (slideToCancelText != null) {
                setSlideToCancelText(slideToCancelText);
            }
            if (slideToCancelTextColor != -1) {
                setSlideToCancelTextColor(getResources().getColor(slideToCancelTextColor));
            }
            if (slideArrowResource != -1) {
                setSlideArrowDrawable(slideArrowResource);
            }
            if (arrowColor != -1) {
                setSlideToCancelArrowColor(getResources().getColor(arrowColor));
            }
            if (counterTimeColor != -1) {
                setCounterTimeColor(getResources().getColor(counterTimeColor));
            }
            setMarginRight(slideMarginRight, true);
            typedArray.recycle();
        }
        recordViewAnimationHelper = new RecordViewAnimationHelper(context, binding.basketImg, binding.glowingMic);
    }

    private void hideViews(boolean hideSmallMic) {
        binding.slideToCancel.setVisibility(GONE);
        binding.basketImg.setVisibility(GONE);
        binding.counterTv.setVisibility(GONE);
        if (hideSmallMic) {
            binding.glowingMic.setVisibility(GONE);
        }
    }

    private void showViews() {
        binding.slideToCancel.setVisibility(VISIBLE);
        binding.glowingMic.setVisibility(VISIBLE);
        binding.counterTv.setVisibility(VISIBLE);
    }

    private boolean isLessThanMin(long time) {
        return time <= minMillis;
    }

    private void playSound(int soundRes) {
        if (!isSoundEnabled) return;
        if (soundRes == 0) return;
        try {
            final MediaPlayer player = new MediaPlayer();
            AssetFileDescriptor afd = context.getResources().openRawResourceFd(soundRes);
            if (afd == null) return;
            player.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
            afd.close();
            player.prepare();
            player.start();
            player.setOnCompletionListener(MediaPlayer::release);
            player.setLooping(false);
        } catch (IOException e) {
            Log.e(TAG, "playSound", e);
        }
    }

    protected void onActionDown(RecordButton recordBtn, MotionEvent motionEvent) {
        if (onRecordListener != null) {
            onRecordListener.onStart();
        }
        recordViewAnimationHelper.setStartRecorded(true);
        recordViewAnimationHelper.resetBasketAnimation();
        recordViewAnimationHelper.resetSmallMic();
        recordBtn.startScale();
        // slideToCancelLayout.startShimmerAnimation();

        initialX = recordBtn.getX();
        basketInitialY = binding.basketImg.getY() + 90;
        // playSound(RECORD_START);
        showViews();

        recordViewAnimationHelper.animateSmallMicAlpha();
        binding.counterTv.setBase(SystemClock.elapsedRealtime());
        startTime = System.currentTimeMillis();
        binding.counterTv.start();
        isSwiped = false;
    }

    protected void onActionMove(RecordButton recordBtn, MotionEvent motionEvent, final boolean forceCancel) {
        long time = System.currentTimeMillis() - startTime;
        if (isSwiped) return;
        //Swipe To Cancel
        if (forceCancel || (binding.slideToCancel.getX() != 0 && binding.slideToCancel.getX() <= binding.counterTv.getRight() + cancelBounds)) {
            //if the time was less than one second then do not start basket animation
            if (isLessThanMin(time)) {
                hideViews(true);
                recordViewAnimationHelper.clearAlphaAnimation(false);
                if (onRecordListener != null) {
                    onRecordListener.onLessThanMin();
                }
                recordViewAnimationHelper.onAnimationEnd();
            } else {
                hideViews(false);
                recordViewAnimationHelper.animateBasket(basketInitialY);
            }
            recordViewAnimationHelper.moveRecordButtonAndSlideToCancelBack(recordBtn, binding.slideToCancel, initialX, difX);
            binding.counterTv.stop();
            // slideToCancelLayout.stopShimmerAnimation();
            isSwiped = true;
            recordViewAnimationHelper.setStartRecorded(false);
            if (onRecordListener != null) {
                onRecordListener.onCancel();
            }
            return;
        }
        //if statement is to Prevent Swiping out of bounds
        if (!(motionEvent.getRawX() < initialX)) return;
        recordBtn.animate()
                 .x(motionEvent.getRawX())
                 .setDuration(0)
                 .start();
        if (difX == 0) {
            difX = (initialX - binding.slideToCancel.getX());
        }
        binding.slideToCancel.animate()
                             .x(motionEvent.getRawX() - difX)
                             .setDuration(0)
                             .start();
    }

    protected void onActionUp(RecordButton recordBtn) {
        final long elapsedTime = System.currentTimeMillis() - startTime;
        if (!isLessThanMinAllowed && isLessThanMin(elapsedTime) && !isSwiped) {
            if (onRecordListener != null) {
                onRecordListener.onLessThanMin();
            }
            recordViewAnimationHelper.setStartRecorded(false);
            // playSound(RECORD_ERROR);
        } else {
            if (onRecordListener != null && !isSwiped) {
                onRecordListener.onFinish(elapsedTime);
            }
            recordViewAnimationHelper.setStartRecorded(false);
            if (!isSwiped) {
                // playSound(RECORD_FINISHED);
            }
        }
        //if user has swiped then do not hide SmallMic since it will be hidden after swipe Animation
        hideViews(!isSwiped);
        if (!isSwiped) {
            recordViewAnimationHelper.clearAlphaAnimation(true);
        }
        recordViewAnimationHelper.moveRecordButtonAndSlideToCancelBack(recordBtn, binding.slideToCancel, initialX, difX);
        binding.counterTv.stop();
        // slideToCancelLayout.stopShimmerAnimation();
    }

    private void setMarginRight(int marginRight, boolean convertToDp) {
        ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) binding.slideToCancel.getLayoutParams();
        if (convertToDp) {
            layoutParams.rightMargin = Utils.convertDpToPx(marginRight);
        } else {
            layoutParams.rightMargin = marginRight;
        }
        binding.slideToCancel.setLayoutParams(layoutParams);
    }

    private void setSlideArrowDrawable(@DrawableRes final int slideArrowResource) {
        Drawable slideArrow = AppCompatResources.getDrawable(getContext(), slideArrowResource);
        // Log.d(TAG, "setSlideArrowDrawable: slideArrow: " + slideArrow);
        if (slideArrow == null) return;
        slideArrow.setBounds(0, 0, slideArrow.getIntrinsicWidth(), slideArrow.getIntrinsicHeight());
        binding.slideToCancel.setCompoundDrawablesRelative(slideArrow, null, null, null);
    }

    public void setOnRecordListener(OnRecordListener onRecordListener) {
        this.onRecordListener = onRecordListener;
    }

    public void setOnBasketAnimationEndListener(OnBasketAnimationEnd onBasketAnimationEndListener) {
        recordViewAnimationHelper.setOnBasketAnimationEndListener(onBasketAnimationEndListener);
    }

    public void setSoundEnabled(boolean isEnabled) {
        isSoundEnabled = isEnabled;
    }

    public void setLessThanMinAllowed(boolean isAllowed) {
        isLessThanMinAllowed = isAllowed;
    }

    public void setSlideToCancelText(String text) {
        binding.slideToCancel.setText(text);
    }

    public void setSlideToCancelTextColor(int color) {
        binding.slideToCancel.setTextColor(color);
    }

    public void setSmallMicColor(int color) {
        binding.glowingMic.setColorFilter(color);
    }

    public void setSmallMicIcon(int icon) {
        binding.glowingMic.setImageResource(icon);
    }

    public void setSlideMarginRight(int marginRight) {
        setMarginRight(marginRight, true);
    }

    public void setCustomSounds(int startSound, int finishedSound, int errorSound) {
        //0 means do not play sound
        RECORD_START = startSound;
        RECORD_FINISHED = finishedSound;
        RECORD_ERROR = errorSound;
    }

    public float getCancelBounds() {
        return cancelBounds;
    }

    public void setCancelBounds(float cancelBounds) {
        setCancelBounds(cancelBounds, true);
    }

    //set Chronometer color
    public void setCounterTimeColor(@ColorInt int color) {
        binding.counterTv.setTextColor(color);
    }

    public void setSlideToCancelArrowColor(@ColorInt int color) {
        Drawable drawable = binding.slideToCancel.getCompoundDrawablesRelative()[0];
        drawable = DrawableCompat.wrap(drawable);
        DrawableCompat.setTint(drawable.mutate(), color);
        binding.slideToCancel.setCompoundDrawablesRelative(drawable, null, null, null);
    }

    private void setCancelBounds(float cancelBounds, boolean convertDpToPixel) {
        this.cancelBounds = convertDpToPixel ? Utils.convertDpToPx(cancelBounds) : cancelBounds;
    }

    public void setMinMillis(final int minMillis) {
        this.minMillis = minMillis;
    }

    public void cancelRecording(final RecordButton recordBtn) {
        onActionMove(recordBtn, null, true);
    }

    public interface OnBasketAnimationEnd {
        void onAnimationEnd();
    }

    public interface OnRecordListener {
        void onStart();

        void onCancel();

        void onFinish(long recordTime);

        void onLessThanMin();
    }
}


