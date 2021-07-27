package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class UsertagIn implements Serializable {
    private final User user;
    private final List<String> position;

    public UsertagIn(final User user, final List<String> position) {
        this.user = user;
        this.position = position;
    }

    public User getUser() {
        return user;
    }

    public List<String> getPosition() {
        return position;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final UsertagIn usertagIn = (UsertagIn) o;
        return Objects.equals(user, usertagIn.user) &&
                Objects.equals(position, usertagIn.position);
    }

    @Override
    public int hashCode() {
        return Objects.hash(user, position);
    }
}
