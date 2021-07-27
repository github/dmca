package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

import awais.instagrabber.repositories.responses.User;

public class RankedRecipient implements Serializable {
    private final User user;
    private final DirectThread thread;

    public RankedRecipient(final User user, final DirectThread thread) {
        this.user = user;
        this.thread = thread;
    }

    public User getUser() {
        return user;
    }

    public DirectThread getThread() {
        return thread;
    }

    public static RankedRecipient of(final User user) {
        return new RankedRecipient(user, null);
    }

    public static RankedRecipient of(final DirectThread thread) {
        return new RankedRecipient(null, thread);
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final RankedRecipient that = (RankedRecipient) o;
        return Objects.equals(user, that.user) &&
                Objects.equals(thread, that.thread);
    }

    @Override
    public int hashCode() {
        return Objects.hash(user, thread);
    }
}
