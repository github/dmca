package awais.instagrabber.interfaces;

public interface FetchListener<T> {
    void onResult(T result);

    default void doBefore() {}

    default void onFailure(Throwable t) {}
}