package awais.instagrabber.webservices;

public interface ServiceCallback<T> {
    void onSuccess(T result);

    void onFailure(Throwable t);
}
