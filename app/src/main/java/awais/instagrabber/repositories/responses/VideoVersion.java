package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.Objects;

public class VideoVersion implements Serializable {
    private final String id;
    private final String type;
    private final int width;
    private final int height;
    private final String url;

    public VideoVersion(final String id, final String type, final int width, final int height, final String url) {
        this.id = id;
        this.type = type;
        this.width = width;
        this.height = height;
        this.url = url;
    }

    public String getId() {
        return id;
    }

    public String getType() {
        return type;
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
        final VideoVersion that = (VideoVersion) o;
        return width == that.width &&
                height == that.height &&
                Objects.equals(id, that.id) &&
                Objects.equals(type, that.type) &&
                Objects.equals(url, that.url);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, type, width, height, url);
    }
}
