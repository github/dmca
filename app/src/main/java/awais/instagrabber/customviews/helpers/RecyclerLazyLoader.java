package awais.instagrabber.customviews.helpers;

import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.StaggeredGridLayoutManager;

import awais.instagrabber.interfaces.LazyLoadListener;

/**
 * thanks to nesquena's <a href="https://gist.github.com/nesquena/d09dc68ff07e845cc622">EndlessRecyclerViewScrollListener</a>
 */
public final class RecyclerLazyLoader extends RecyclerView.OnScrollListener {
    /**
     * The current offset index of data you have loaded
     */
    private int currentPage = 0;
    /**
     * The total number of items in the data set after the last load
     */
    private int previousTotalItemCount = 0;
    /**
     * <code>true</code> if we are still waiting for the last set of data to load.
     */
    private boolean loading = true;
    /**
     * The minimum amount of items to have below your current scroll position before loading more.
     */
    private final int visibleThreshold;
    private final LazyLoadListener lazyLoadListener;
    private final RecyclerView.LayoutManager layoutManager;

    public RecyclerLazyLoader(@NonNull final RecyclerView.LayoutManager layoutManager,
                              final LazyLoadListener lazyLoadListener,
                              final int threshold) {
        this.layoutManager = layoutManager;
        this.lazyLoadListener = lazyLoadListener;
        if (threshold > 0) {
            this.visibleThreshold = threshold;
            return;
        }
        if (layoutManager instanceof GridLayoutManager) {
            this.visibleThreshold = 5 * Math.max(3, ((GridLayoutManager) layoutManager).getSpanCount());
        } else if (layoutManager instanceof StaggeredGridLayoutManager) {
            this.visibleThreshold = 4 * Math.max(3, ((StaggeredGridLayoutManager) layoutManager).getSpanCount());
        } else if (layoutManager instanceof LinearLayoutManager) {
            this.visibleThreshold = ((LinearLayoutManager) layoutManager).getReverseLayout() ? 4 : 8;
        } else {
            this.visibleThreshold = 5;
        }
    }

    public RecyclerLazyLoader(@NonNull final RecyclerView.LayoutManager layoutManager,
                              final LazyLoadListener lazyLoadListener) {
        this(layoutManager, lazyLoadListener, -1);
    }

    @Override
    public void onScrolled(@NonNull final RecyclerView recyclerView, final int dx, final int dy) {
        final int totalItemCount = layoutManager.getItemCount();

        if (totalItemCount < previousTotalItemCount) {
            currentPage = 0;
            previousTotalItemCount = totalItemCount;
            if (totalItemCount == 0) loading = true;
        }

        if (loading && totalItemCount > previousTotalItemCount) {
            loading = false;
            previousTotalItemCount = totalItemCount;
        }

        int lastVisibleItemPosition;
        if (layoutManager instanceof GridLayoutManager) {
            final GridLayoutManager layoutManager = (GridLayoutManager) this.layoutManager;
            lastVisibleItemPosition = layoutManager.findLastVisibleItemPosition();
        } else if (layoutManager instanceof StaggeredGridLayoutManager) {
            final StaggeredGridLayoutManager layoutManager = (StaggeredGridLayoutManager) this.layoutManager;
            final int spanCount = layoutManager.getSpanCount();
            final int[] lastVisibleItemPositions = layoutManager.findLastVisibleItemPositions(null);
            lastVisibleItemPosition = 0;
            for (final int itemPosition : lastVisibleItemPositions) {
                if (itemPosition > lastVisibleItemPosition) {
                    lastVisibleItemPosition = itemPosition;
                }
            }
        } else {
            final LinearLayoutManager layoutManager = (LinearLayoutManager) this.layoutManager;
            lastVisibleItemPosition = layoutManager.findLastVisibleItemPosition();
        }

        if (!loading && lastVisibleItemPosition + visibleThreshold > totalItemCount) {
            loading = true;
            if (lazyLoadListener != null) {
                new Handler().postDelayed(() -> lazyLoadListener.onLoadMore(++currentPage, totalItemCount), 200);
            }
        }
    }

    public int getCurrentPage() {
        return currentPage;
    }

    public void resetState() {
        this.currentPage = 0;
        this.previousTotalItemCount = 0;
        this.loading = true;
    }
}