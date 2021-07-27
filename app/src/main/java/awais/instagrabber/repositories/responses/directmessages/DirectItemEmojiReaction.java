package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

public class DirectItemEmojiReaction implements Serializable {
    private final long senderId;
    private final long timestamp;
    private final String emoji;
    private final String superReactType;

    public DirectItemEmojiReaction(final long senderId, final long timestamp, final String emoji, final String superReactType) {
        this.senderId = senderId;
        this.timestamp = timestamp;
        this.emoji = emoji;
        this.superReactType = superReactType;
    }

    public long getSenderId() {
        return senderId;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public String getEmoji() {
        return emoji;
    }

    public String getSuperReactType() {
        return superReactType;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemEmojiReaction that = (DirectItemEmojiReaction) o;
        return senderId == that.senderId &&
                timestamp == that.timestamp &&
                Objects.equals(emoji, that.emoji) &&
                Objects.equals(superReactType, that.superReactType);
    }

    @Override
    public int hashCode() {
        return Objects.hash(senderId, timestamp, emoji, superReactType);
    }

    @Override
    public String toString() {
        return "DirectItemEmojiReaction{" +
                "senderId=" + senderId +
                ", timestamp=" + timestamp +
                ", emoji='" + emoji + '\'' +
                ", superReactType='" + superReactType + '\'' +
                '}';
    }
}
