package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

import awais.instagrabber.models.enums.RavenMediaViewMode;
import awais.instagrabber.repositories.responses.Media;

public class DirectItemVisualMedia implements Serializable {
    private final long urlExpireAtSecs;
    private final int playbackDurationSecs;
    private final List<Long> seenUserIds;
    private final RavenMediaViewMode viewMode;
    private final int seenCount;
    private final long replayExpiringAtUs;
    private final RavenExpiringMediaActionSummary expiringMediaActionSummary;
    private final Media media;

    public DirectItemVisualMedia(final long urlExpireAtSecs,
                                 final int playbackDurationSecs,
                                 final List<Long> seenUserIds,
                                 final RavenMediaViewMode viewMode,
                                 final int seenCount,
                                 final long replayExpiringAtUs,
                                 final RavenExpiringMediaActionSummary expiringMediaActionSummary,
                                 final Media media) {
        this.urlExpireAtSecs = urlExpireAtSecs;
        this.playbackDurationSecs = playbackDurationSecs;
        this.seenUserIds = seenUserIds;
        this.viewMode = viewMode;
        this.seenCount = seenCount;
        this.replayExpiringAtUs = replayExpiringAtUs;
        this.expiringMediaActionSummary = expiringMediaActionSummary;
        this.media = media;
    }

    public long getUrlExpireAtSecs() {
        return urlExpireAtSecs;
    }

    public int getPlaybackDurationSecs() {
        return playbackDurationSecs;
    }

    public List<Long> getSeenUserIds() {
        return seenUserIds;
    }

    public RavenMediaViewMode getViewMode() {
        return viewMode;
    }

    public int getSeenCount() {
        return seenCount;
    }

    public long getReplayExpiringAtUs() {
        return replayExpiringAtUs;
    }

    public RavenExpiringMediaActionSummary getExpiringMediaActionSummary() {
        return expiringMediaActionSummary;
    }

    public Media getMedia() {
        return media;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemVisualMedia media1 = (DirectItemVisualMedia) o;
        return urlExpireAtSecs == media1.urlExpireAtSecs &&
                playbackDurationSecs == media1.playbackDurationSecs &&
                seenCount == media1.seenCount &&
                replayExpiringAtUs == media1.replayExpiringAtUs &&
                Objects.equals(seenUserIds, media1.seenUserIds) &&
                viewMode == media1.viewMode &&
                Objects.equals(expiringMediaActionSummary, media1.expiringMediaActionSummary) &&
                Objects.equals(media, media1.media);
    }

    @Override
    public int hashCode() {
        return Objects
                .hash(urlExpireAtSecs, playbackDurationSecs, seenUserIds, viewMode, seenCount, replayExpiringAtUs, expiringMediaActionSummary, media);
    }
}
