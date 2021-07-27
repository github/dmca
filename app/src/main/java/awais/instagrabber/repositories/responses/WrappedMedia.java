package awais.instagrabber.repositories.responses;

public class WrappedMedia {
    private final Media media;

    public WrappedMedia(final Media media) {
        this.media = media;
    }

    public Media getMedia() {
        return media;
    }
}
