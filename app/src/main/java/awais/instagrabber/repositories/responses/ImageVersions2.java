package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class ImageVersions2 implements Serializable {
    private final List<MediaCandidate> candidates;

    public ImageVersions2(final List<MediaCandidate> candidates) {
        this.candidates = candidates;
    }

    public List<MediaCandidate> getCandidates() {
        return candidates;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final ImageVersions2 that = (ImageVersions2) o;
        return Objects.equals(candidates, that.candidates);
    }

    @Override
    public int hashCode() {
        return Objects.hash(candidates);
    }
}
