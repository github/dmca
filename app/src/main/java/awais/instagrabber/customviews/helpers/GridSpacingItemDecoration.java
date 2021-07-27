package awais.instagrabber.customviews.helpers;

import android.graphics.Rect;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

public class GridSpacingItemDecoration extends RecyclerView.ItemDecoration {
    private final int halfSpace;

    private boolean hasHeader;

    public GridSpacingItemDecoration(int spacing) {
        halfSpace = spacing / 2;
    }

    @Override
    public void getItemOffsets(@NonNull Rect outRect, @NonNull View view, @NonNull RecyclerView parent, @NonNull RecyclerView.State state) {
        if (hasHeader && parent.getChildAdapterPosition(view) == 0) {
            outRect.bottom = halfSpace;
            outRect.left = -halfSpace;
            outRect.right = -halfSpace;
            return;
        }
        if (parent.getPaddingLeft() != halfSpace) {
            parent.setPadding(halfSpace, hasHeader ? 0 : halfSpace, halfSpace, halfSpace);
            parent.setClipToPadding(false);
        }
        outRect.top = halfSpace;
        outRect.bottom = halfSpace;
        outRect.left = halfSpace;
        outRect.right = halfSpace;
    }

    public void setHasHeader(final boolean hasHeader) {
        this.hasHeader = hasHeader;
    }
}