package awais.instagrabber.models;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Objects;

public class Resource<T> {
    public final Status status;
    public final T data;
    public final String message;
    public final int resId;

    private Resource(@NonNull Status status,
                     @Nullable T data,
                     @Nullable String message,
                     int resId) {
        this.status = status;
        this.data = data;
        this.message = message;
        this.resId = resId;
    }

    @NonNull
    public static <T> Resource<T> success(@NonNull T data) {
        return new Resource<>(Status.SUCCESS, data, null, 0);
    }

    @NonNull
    public static <T> Resource<T> error(String msg, @Nullable T data) {
        return new Resource<>(Status.ERROR, data, msg, 0);
    }

    @NonNull
    public static <T> Resource<T> error(int resId, @Nullable T data) {
        return new Resource<>(Status.ERROR, data, null, resId);
    }

    @NonNull
    public static <T> Resource<T> loading(@Nullable T data) {
        return new Resource<>(Status.LOADING, data, null, 0);
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Resource<?> resource = (Resource<?>) o;
        return status == resource.status &&
                Objects.equals(data, resource.data) &&
                Objects.equals(message, resource.message);
    }

    @Override
    public int hashCode() {
        return Objects.hash(status, data, message);
    }

    public enum Status {
        SUCCESS,
        ERROR,
        LOADING
    }
}
