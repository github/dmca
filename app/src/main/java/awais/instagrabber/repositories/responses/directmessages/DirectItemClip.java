package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

import awais.instagrabber.repositories.responses.Media;

public class DirectItemClip implements Serializable {
    private final Media clip;

    public DirectItemClip(final Media clip) {
        this.clip = clip;
    }

    public Media getClip() {
        return clip;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemClip that = (DirectItemClip) o;
        return Objects.equals(clip, that.clip);
    }

    @Override
    public int hashCode() {
        return Objects.hash(clip);
    }
}
