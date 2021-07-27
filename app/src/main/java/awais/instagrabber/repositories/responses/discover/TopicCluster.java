package awais.instagrabber.repositories.responses.discover;

import java.io.Serializable;

import awais.instagrabber.repositories.responses.Media;

public class TopicCluster implements Serializable {
    private final String id;
    private final String title;
    private final String type;
    private final boolean canMute;
    private final boolean isMuted;
    private final int rankedPosition;
    private Media coverMedia;

    public TopicCluster(final String id,
                        final String title,
                        final String type,
                        final boolean canMute,
                        final boolean isMuted,
                        final int rankedPosition,
                        final Media coverMedia) {
        this.id = id;
        this.title = title;
        this.type = type;
        this.canMute = canMute;
        this.isMuted = isMuted;
        this.rankedPosition = rankedPosition;
        this.coverMedia = coverMedia;
    }

    public String getId() {
        return id;
    }

    public String getTitle() {
        return title;
    }

    public String getType() {
        return type;
    }

    public boolean isCanMute() {
        return canMute;
    }

    public boolean isMuted() {
        return isMuted;
    }

    public int getRankedPosition() {
        return rankedPosition;
    }

    public Media getCoverMedia() {
        return coverMedia;
    }

    public void setCoverMedia(final Media coverMedia) {
        this.coverMedia = coverMedia;
    }
}
