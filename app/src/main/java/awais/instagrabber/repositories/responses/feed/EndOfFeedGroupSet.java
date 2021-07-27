package awais.instagrabber.repositories.responses.feed;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class EndOfFeedGroupSet implements Serializable {
    private final long id;
    private final String activeGroupId;
    private final String connectedGroupId;
    private final String nextMaxId;
    private final String paginationSource;
    private final List<EndOfFeedGroup> groups;

    public EndOfFeedGroupSet(final long id,
                             final String activeGroupId,
                             final String connectedGroupId,
                             final String nextMaxId,
                             final String paginationSource,
                             final List<EndOfFeedGroup> groups) {
        this.id = id;
        this.activeGroupId = activeGroupId;
        this.connectedGroupId = connectedGroupId;
        this.nextMaxId = nextMaxId;
        this.paginationSource = paginationSource;
        this.groups = groups;
    }

    public long getId() {
        return id;
    }

    public String getActiveGroupId() {
        return activeGroupId;
    }

    public String getConnectedGroupId() {
        return connectedGroupId;
    }

    public String getNextMaxId() {
        return nextMaxId;
    }

    public String getPaginationSource() {
        return paginationSource;
    }

    public List<EndOfFeedGroup> getGroups() {
        return groups;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final EndOfFeedGroupSet that = (EndOfFeedGroupSet) o;
        return id == that.id &&
                Objects.equals(activeGroupId, that.activeGroupId) &&
                Objects.equals(connectedGroupId, that.connectedGroupId) &&
                Objects.equals(nextMaxId, that.nextMaxId) &&
                Objects.equals(paginationSource, that.paginationSource) &&
                Objects.equals(groups, that.groups);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, activeGroupId, connectedGroupId, nextMaxId, paginationSource, groups);
    }
}
