package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.Objects;

public class MediaCandidate implements Serializable {
    private final int width;
    private final int height;
    private final String url;

    public MediaCandidate(final int width, final int height, final String url) {
        this.width = width;
        this.height = height;
        this.url = url;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public String getUrl() {
        return url;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final MediaCandidate that = (MediaCandidate) o;
        return width == that.width &&
                height == that.height &&
                Objects.equals(url, that.url);
    }

    @Override
    public int hashCode() {
        return Objects.hash(width, height, url);
    }
}
