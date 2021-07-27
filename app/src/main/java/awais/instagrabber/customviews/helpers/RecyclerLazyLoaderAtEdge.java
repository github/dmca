package awais.instagrabber.customviews.helpers;

import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

public final class RecyclerLazyLoaderAtEdge extends RecyclerView.OnScrollListener {

    private final RecyclerView.LayoutManager layoutManager;
    private final LazyLoadListener lazyLoadListener;
    private final boolean atTop;
    private int currentPage;
    private int previousItemCount;
    private boolean loading;

    public RecyclerLazyLoaderAtEdge(@NonNull final RecyclerView.LayoutManager layoutManager,
                                    final LazyLoadListener lazyLoadListener) {
        this.layoutManager = layoutManager;
        this.atTop = false;
        this.lazyLoadListener = lazyLoadListener;
    }

    public RecyclerLazyLoaderAtEdge(@NonNull final RecyclerView.LayoutManager layoutManager,
                                    final boolean atTop,
                                    final LazyLoadListener lazyLoadListener) {
        this.layoutManager = layoutManager;
        this.atTop = atTop;
        this.lazyLoadListener = lazyLoadListener;
    }

    @Override
    public void onScrollStateChanged(@NonNull final RecyclerView recyclerView, final int newState) {
        super.onScrollStateChanged(recyclerView, newState);
        final int itemCount = layoutManager.getItemCount();
        if (itemCount > previousItemCount) {
            loading = false;
        }
        if (!recyclerView.canScrollVertically(atTop ? -1 : 1)
                && newState == RecyclerView.SCROLL_STATE_IDLE
                && !loading
                && lazyLoadListener != null) {
            loading = true;
            new Handler().postDelayed(() -> lazyLoadListener.onLoadMore(++currentPage), 300);
        }
    }

    public int getCurrentPage() {
        return currentPage;
    }

    public void resetState() {
        currentPage = 0;
        previousItemCount = 0;
        loading = true;
    }

    public interface LazyLoadListener {
        void onLoadMore(final int page);
    }
}