package awais.instagrabber.repositories.responses.feed;

import java.io.Serializable;
import java.util.Objects;

public class EndOfFeedDemarcator implements Serializable {
    private final long id;
    private final EndOfFeedGroupSet groupSet;

    public EndOfFeedDemarcator(final long id, final EndOfFeedGroupSet groupSet) {
        this.id = id;
        this.groupSet = groupSet;
    }

    public long getId() {
        return id;
    }

    public EndOfFeedGroupSet getGroupSet() {
        return groupSet;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final EndOfFeedDemarcator that = (EndOfFeedDemarcator) o;
        return id == that.id &&
                Objects.equals(groupSet, that.groupSet);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, groupSet);
    }
}
