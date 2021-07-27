package awais.instagrabber.customviews.helpers;

import android.app.Activity;
import android.graphics.Rect;
import android.graphics.drawable.ColorDrawable;
import android.view.Gravity;
import android.view.View;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.view.WindowManager.LayoutParams;
import android.widget.PopupWindow;

public class HeightProvider extends PopupWindow implements OnGlobalLayoutListener {
    private final Activity mActivity;
    private final View rootView;
    private HeightListener listener;
    private int heightMax;

    public HeightProvider(Activity activity) {
        super(activity);
        this.mActivity = activity;

        rootView = new View(activity);
        setContentView(rootView);

        rootView.getViewTreeObserver().addOnGlobalLayoutListener(this);
        setBackgroundDrawable(new ColorDrawable(0));

        setWidth(0);
        setHeight(LayoutParams.MATCH_PARENT);

        setSoftInputMode(LayoutParams.SOFT_INPUT_ADJUST_RESIZE);
        setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);
    }

    public HeightProvider init() {
        if (!isShowing()) {
            final View view = mActivity.getWindow().getDecorView();
            view.post(() -> showAtLocation(view, Gravity.NO_GRAVITY, 0, 0));
        }
        return this;
    }

    public HeightProvider setHeightListener(HeightListener listener) {
        this.listener = listener;
        return this;
    }

    @Override
    public void onGlobalLayout() {
        Rect rect = new Rect();
        rootView.getWindowVisibleDisplayFrame(rect);
        if (rect.bottom > heightMax) {
            heightMax = rect.bottom;
        }
        
        int keyboardHeight = heightMax - rect.bottom;
        if (listener != null) {
            listener.onHeightChanged(keyboardHeight);
        }
    }

    public interface HeightListener {
        void onHeightChanged(int height);
    }
}

