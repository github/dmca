package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

import awais.instagrabber.repositories.responses.Media;

public class DirectItemReelShare implements Serializable {
    private final String text;
    private final String type;
    private final long reelOwnerId;
    private final long mentionedUserId;
    private final boolean isReelPersisted;
    private final String reelType;
    private final Media media;
    private final DirectItemReelShareReactionInfo reactionInfo;

    public DirectItemReelShare(final String text,
                               final String type,
                               final long reelOwnerId,
                               final long mentionedUserId,
                               final boolean isReelPersisted,
                               final String reelType,
                               final Media media,
                               final DirectItemReelShareReactionInfo reactionInfo) {
        this.text = text;
        this.type = type;
        this.reelOwnerId = reelOwnerId;
        this.mentionedUserId = mentionedUserId;
        this.isReelPersisted = isReelPersisted;
        this.reelType = reelType;
        this.media = media;
        this.reactionInfo = reactionInfo;
    }

    public String getText() {
        return text;
    }

    public String getType() {
        return type;
    }

    public long getReelOwnerId() {
        return reelOwnerId;
    }

    public boolean isReelPersisted() {
        return isReelPersisted;
    }

    public String getReelType() {
        return reelType;
    }

    public Media getMedia() {
        return media;
    }

    public DirectItemReelShareReactionInfo getReactionInfo() {
        return reactionInfo;
    }

    public long getMentionedUserId() {
        return mentionedUserId;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemReelShare that = (DirectItemReelShare) o;
        return reelOwnerId == that.reelOwnerId &&
                mentionedUserId == that.mentionedUserId &&
                isReelPersisted == that.isReelPersisted &&
                Objects.equals(text, that.text) &&
                Objects.equals(type, that.type) &&
                Objects.equals(reelType, that.reelType) &&
                Objects.equals(media, that.media) &&
                Objects.equals(reactionInfo, that.reactionInfo);
    }

    @Override
    public int hashCode() {
        return Objects.hash(text, type, reelOwnerId, mentionedUserId, isReelPersisted, reelType, media, reactionInfo);
    }
}
