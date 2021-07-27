package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

public class DirectThreadLastSeenAt implements Serializable {
    private final String timestamp;
    private final String itemId;

    public DirectThreadLastSeenAt(final String timestamp, final String itemId) {
        this.timestamp = timestamp;
        this.itemId = itemId;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public String getItemId() {
        return itemId;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectThreadLastSeenAt that = (DirectThreadLastSeenAt) o;
        return Objects.equals(timestamp, that.timestamp) &&
                Objects.equals(itemId, that.itemId);
    }

    @Override
    public int hashCode() {
        return Objects.hash(timestamp, itemId);
    }
}
