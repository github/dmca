package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

public class DirectItemPlaceholder implements Serializable {
    private final boolean isLinked;
    private final String title;
    private final String message;

    public DirectItemPlaceholder(final boolean isLinked,
                                 final String title,
                                 final String message) {
        this.isLinked = isLinked;
        this.title = title;
        this.message = message;
    }

    public boolean isLinked() {
        return isLinked;
    }

    public String getTitle() {
        return title;
    }

    public String getMessage() {
        return message;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemPlaceholder that = (DirectItemPlaceholder) o;
        return isLinked == that.isLinked &&
                Objects.equals(title, that.title) &&
                Objects.equals(message, that.message);
    }

    @Override
    public int hashCode() {
        return Objects.hash(isLinked, title, message);
    }
}
