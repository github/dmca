package awais.instagrabber.repositories.responses.directmessages;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.Objects;

public final class RavenExpiringMediaActionSummary implements Serializable {
    private final ActionType type;
    private final long timestamp;
    private final int count;

    public RavenExpiringMediaActionSummary(final long timestamp, final int count, final ActionType type) {
        this.timestamp = timestamp;
        this.count = count;
        this.type = type;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public int getCount() {
        return count;
    }

    public ActionType getType() {
        return type;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final RavenExpiringMediaActionSummary that = (RavenExpiringMediaActionSummary) o;
        return timestamp == that.timestamp &&
                count == that.count &&
                type == that.type;
    }

    @Override
    public int hashCode() {
        return Objects.hash(type, timestamp, count);
    }

    // thanks to http://github.com/warifp/InstagramAutoPostImageUrl/blob/master/vendor/mgp25/instagram-php/src/Response/Model/ActionBadge.php
    public enum ActionType {
        @SerializedName("raven_delivered")
        DELIVERED,
        @SerializedName("raven_sent")
        SENT,
        @SerializedName("raven_opened")
        OPENED,
        @SerializedName("raven_screenshot")
        SCREENSHOT,
        @SerializedName("raven_replayed")
        REPLAYED,
        @SerializedName("raven_cannot_deliver")
        CANNOT_DELIVER,
        @SerializedName("raven_sending")
        SENDING,
        @SerializedName("raven_blocked")
        BLOCKED,
        @SerializedName("raven_unknown")
        UNKNOWN,
        @SerializedName("raven_suggested")
        SUGGESTED,
    }
}
