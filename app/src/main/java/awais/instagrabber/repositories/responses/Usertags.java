package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class Usertags implements Serializable {
    private final List<UsertagIn> in;

    public Usertags(final List<UsertagIn> in) {
        this.in = in;
    }

    public List<UsertagIn> getIn() {
        return in;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Usertags usertags = (Usertags) o;
        return Objects.equals(in, usertags.in);
    }

    @Override
    public int hashCode() {
        return Objects.hash(in);
    }
}
