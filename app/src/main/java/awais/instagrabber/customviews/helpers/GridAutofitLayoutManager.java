package awais.instagrabber.customviews.helpers;

import android.content.Context;

import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.utils.Utils;

public class GridAutofitLayoutManager extends GridLayoutManager {
    private int mColumnWidth;
    private boolean mColumnWidthChanged = true;

    public GridAutofitLayoutManager(Context context, int columnWidth) {
        super(context, 1);
        if (columnWidth <= 0) columnWidth = (int) (48 * Utils.displayMetrics.density);
        if (columnWidth > 0 && columnWidth != mColumnWidth) {
            mColumnWidth = columnWidth;
            mColumnWidthChanged = true;
        }
    }

    @Override
    public void onLayoutChildren(final RecyclerView.Recycler recycler, final RecyclerView.State state) {
        final int width = getWidth();
        final int height = getHeight();
        if (mColumnWidthChanged && mColumnWidth > 0 && width > 0 && height > 0) {
            final int totalSpace = getOrientation() == VERTICAL ? width - getPaddingRight() - getPaddingLeft()
                    : height - getPaddingTop() - getPaddingBottom();

            setSpanCount(Math.max(1, Math.min(totalSpace / mColumnWidth, 3)));

            mColumnWidthChanged = false;
        }
        super.onLayoutChildren(recycler, state);
    }
}
