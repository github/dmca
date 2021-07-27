package awais.instagrabber.db.repositories;

import androidx.annotation.MainThread;

public interface RepositoryCallback<T> {
    @MainThread
    void onSuccess(T result);

    @MainThread
    void onDataNotAvailable();
}
