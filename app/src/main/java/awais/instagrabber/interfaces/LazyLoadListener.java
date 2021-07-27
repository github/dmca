package awais.instagrabber.interfaces;

public interface LazyLoadListener {
    void onLoadMore(final int page, final int totalItemsCount);
}