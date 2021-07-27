package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

import awais.instagrabber.repositories.responses.Media;

public class DirectItemStoryShare implements Serializable {
    private final String reelId;
    private final String reelType;
    private final String text;
    private final boolean isReelPersisted;
    private final Media media;
    private final String title;
    private final String message;

    public DirectItemStoryShare(final String reelId,
                                final String reelType,
                                final String text,
                                final boolean isReelPersisted,
                                final Media media,
                                final String title,
                                final String message) {
        this.reelId = reelId;
        this.reelType = reelType;
        this.text = text;
        this.isReelPersisted = isReelPersisted;
        this.media = media;
        this.title = title;
        this.message = message;
    }

    public String getReelId() {
        return reelId;
    }

    public String getReelType() {
        return reelType;
    }

    public String getText() {
        return text;
    }

    public boolean isReelPersisted() {
        return isReelPersisted;
    }

    public Media getMedia() {
        return media;
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
        final DirectItemStoryShare that = (DirectItemStoryShare) o;
        return isReelPersisted == that.isReelPersisted &&
                Objects.equals(reelId, that.reelId) &&
                Objects.equals(reelType, that.reelType) &&
                Objects.equals(text, that.text) &&
                Objects.equals(media, that.media) &&
                Objects.equals(title, that.title) &&
                Objects.equals(message, that.message);
    }

    @Override
    public int hashCode() {
        return Objects.hash(reelId, reelType, text, isReelPersisted, media, title, message);
    }
}
