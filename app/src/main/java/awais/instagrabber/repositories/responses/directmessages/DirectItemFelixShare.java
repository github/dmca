package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

import awais.instagrabber.repositories.responses.Media;

public class DirectItemFelixShare implements Serializable {
    private final Media video;

    public DirectItemFelixShare(final Media video) {
        this.video = video;
    }

    public Media getVideo() {
        return video;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemFelixShare that = (DirectItemFelixShare) o;
        return Objects.equals(video, that.video);
    }

    @Override
    public int hashCode() {
        return Objects.hash(video);
    }
}
