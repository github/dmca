package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

import awais.instagrabber.repositories.responses.Media;

public class DirectItemVoiceMedia implements Serializable {
    private final Media media;
    private final int seenCount;
    private final String viewMode;

    public DirectItemVoiceMedia(final Media media, final int seenCount, final String viewMode) {
        this.media = media;
        this.seenCount = seenCount;
        this.viewMode = viewMode;
    }

    public Media getMedia() {
        return media;
    }

    public int getSeenCount() {
        return seenCount;
    }

    public String getViewMode() {
        return viewMode;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemVoiceMedia that = (DirectItemVoiceMedia) o;
        return seenCount == that.seenCount &&
                Objects.equals(media, that.media) &&
                Objects.equals(viewMode, that.viewMode);
    }

    @Override
    public int hashCode() {
        return Objects.hash(media, seenCount, viewMode);
    }
}
