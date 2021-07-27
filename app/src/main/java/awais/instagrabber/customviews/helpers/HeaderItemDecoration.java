package awais.instagrabber.customviews.helpers;

import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.util.Pair;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Java implementation of <a href="https://gist.github.com/filipkowicz/1a769001fae407b8813ab4387c42fcbd/3cda7542b12100b01da449e8648368b8f1369c70">this gist</a> by filipkowicz
 */
public class HeaderItemDecoration extends RecyclerView.ItemDecoration {
    private static final String TAG = HeaderItemDecoration.class.getSimpleName();

    private final HeaderItemDecorationCallback callback;

    private boolean layoutReversed = false;
    private Pair<Integer, RecyclerView.ViewHolder> currentHeader;

    public HeaderItemDecoration(@NonNull RecyclerView parent,
                                @NonNull HeaderItemDecorationCallback callback) {
        this.callback = callback;
        final RecyclerView.LayoutManager layoutManager = parent.getLayoutManager();
        if (layoutManager instanceof LinearLayoutManager) {
            layoutReversed = ((LinearLayoutManager) layoutManager).getReverseLayout();
        }
        //noinspection rawtypes
        final RecyclerView.Adapter adapter = parent.getAdapter();
        if (adapter == null) return;
        adapter.registerAdapterDataObserver(new RecyclerView.AdapterDataObserver() {
            @Override
            public void onChanged() {
                // clear saved header as it can be outdated now
                Log.d(TAG, "registerAdapterDataObserver");
                currentHeader = null;
            }
        });
        parent.addOnLayoutChangeListener((v, left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom) -> {
            // clear saved layout as it may need layout update
            Log.d(TAG, "addOnLayoutChangeListener");
            currentHeader = null;
        });
        parent.addOnItemTouchListener(new RecyclerView.SimpleOnItemTouchListener() {
            @Override
            public boolean onInterceptTouchEvent(@NonNull final RecyclerView rv, @NonNull final MotionEvent e) {
                if (e.getAction() == MotionEvent.ACTION_DOWN && currentHeader != null) {
                    final RecyclerView.ViewHolder viewHolder = currentHeader.second;
                    if (viewHolder != null && viewHolder.itemView != null) {
                        final int bottom = viewHolder.itemView.getBottom();
                        return e.getY() <= bottom;
                    }
                }
                return super.onInterceptTouchEvent(rv, e);
            }
        });
    }

    @Override
    public void onDrawOver(@NonNull final Canvas c, @NonNull final RecyclerView parent, @NonNull final RecyclerView.State state) {
        super.onDrawOver(c, parent, state);
        final View topChild = parent.findChildViewUnder(
                parent.getPaddingLeft(),
                parent.getPaddingTop()
        );
        if (topChild == null) {
            return;
        }
        final int topChildPosition = parent.getChildAdapterPosition(topChild);
        if (topChildPosition == RecyclerView.NO_POSITION) {
            return;
        }
        final View headerView = getHeaderViewForItem(topChildPosition, parent);
        if (headerView == null) {
            return;
        }
        final int contactPoint = headerView.getBottom() + parent.getPaddingTop();
        final View childInContact = getChildInContact(parent, contactPoint);
        if (childInContact != null && callback.isHeader(parent.getChildAdapterPosition(childInContact))) {
            moveHeader(c, headerView, childInContact, parent.getPaddingTop());
            return;
        }
        drawHeader(c, headerView, parent.getPaddingTop());
    }

    private void drawHeader(@NonNull final Canvas c, @NonNull final View header, final int paddingTop) {
        c.save();
        c.translate(0f, paddingTop);
        header.draw(c);
        c.restore();
    }

    private void moveHeader(@NonNull final Canvas c, @NonNull final View currentHeader, @NonNull final View nextHeader, final int paddingTop) {
        c.save();
        c.translate(0f, nextHeader.getTop() - currentHeader.getHeight() /*+ paddingTop*/);
        currentHeader.draw(c);
        c.restore();
    }

    @Nullable
    private View getChildInContact(@NonNull final RecyclerView parent, final int contactPoint) {
        View childInContact = null;
        final int childCount = parent.getChildCount();
        for (int i = 0; i < childCount; i++) {
            final View child = parent.getChildAt(i);
            final Rect mBounds = new Rect();
            parent.getDecoratedBoundsWithMargins(child, mBounds);
            if (mBounds.bottom > contactPoint) {
                if (mBounds.top <= contactPoint) {
                    // This child overlaps the contactPoint
                    childInContact = child;
                    break;
                }
            }
        }
        return childInContact;
    }

    @Nullable
    private View getHeaderViewForItem(final int itemPosition, @NonNull final RecyclerView parent) {
        if (parent.getAdapter() == null) {
            return null;
        }
        final int headerPosition = getHeaderPositionForItem(itemPosition, parent.getAdapter());
        if (headerPosition == RecyclerView.NO_POSITION) return null;
        final int headerType = parent.getAdapter().getItemViewType(headerPosition);
        // if match reuse viewHolder
        if (currentHeader != null
                && currentHeader.first == headerPosition
                && currentHeader.second.getItemViewType() == headerType) {
            return currentHeader.second.itemView;
        }
        final RecyclerView.ViewHolder headerHolder = parent.getAdapter().createViewHolder(parent, headerType);
        if (headerHolder != null) {
            //noinspection unchecked
            parent.getAdapter().onBindViewHolder(headerHolder, headerPosition);
            fixLayoutSize(parent, headerHolder.itemView);
            // save for next draw
            currentHeader = new Pair<>(headerPosition, headerHolder);
            return headerHolder.itemView;
        }
        return null;
    }

    @SuppressWarnings("rawtypes")
    private int getHeaderPositionForItem(final int itemPosition, final RecyclerView.Adapter adapter) {
        int headerPosition = RecyclerView.NO_POSITION;
        int currentPosition = itemPosition;
        do {
            if (callback.isHeader(currentPosition)) {
                headerPosition = currentPosition;
                break;
            }
            currentPosition += layoutReversed ? 1 : -1;
        } while (layoutReversed ? currentPosition < adapter.getItemCount() : currentPosition >= 0);
        return headerPosition;
    }

    /**
     * Properly measures and layouts the top sticky header.
     *
     * @param parent ViewGroup: RecyclerView in this case.
     */
    private void fixLayoutSize(@NonNull final ViewGroup parent, @NonNull final View view) {

        // Specs for parent (RecyclerView)
        final int widthSpec = View.MeasureSpec.makeMeasureSpec(parent.getWidth(), View.MeasureSpec.EXACTLY);
        final int heightSpec = View.MeasureSpec.makeMeasureSpec(parent.getHeight(), View.MeasureSpec.UNSPECIFIED);

        // Specs for children (headers)
        final int childWidthSpec = ViewGroup.getChildMeasureSpec(
                widthSpec,
                parent.getPaddingLeft() + parent.getPaddingRight(),
                view.getLayoutParams().width
        );
        final int childHeightSpec = ViewGroup.getChildMeasureSpec(
                heightSpec,
                parent.getPaddingTop() + parent.getPaddingBottom(),
                view.getLayoutParams().height
        );

        view.measure(childWidthSpec, childHeightSpec);
        view.layout(0, 0, view.getMeasuredWidth(), view.getMeasuredHeight());
    }

    public View getCurrentHeader() {
        return currentHeader == null ? null : currentHeader.second.itemView;
    }

    public interface HeaderItemDecorationCallback {
        boolean isHeader(int itemPosition);
    }
}
