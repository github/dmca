package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.Objects;

public class AnimatedMediaFixedHeight implements Serializable {
    private final int height;
    private final int width;
    private final String mp4;
    private final String url;
    private final String webp;

    public AnimatedMediaFixedHeight(final int height, final int width, final String mp4, final String url, final String webp) {
        this.height = height;
        this.width = width;
        this.mp4 = mp4;
        this.url = url;
        this.webp = webp;
    }

    public int getHeight() {
        return height;
    }

    public int getWidth() {
        return width;
    }

    public String getMp4() {
        return mp4;
    }

    public String getUrl() {
        return url;
    }

    public String getWebp() {
        return webp;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final AnimatedMediaFixedHeight that = (AnimatedMediaFixedHeight) o;
        return height == that.height &&
                width == that.width &&
                Objects.equals(mp4, that.mp4) &&
                Objects.equals(url, that.url) &&
                Objects.equals(webp, that.webp);
    }

    @Override
    public int hashCode() {
        return Objects.hash(height, width, mp4, url, webp);
    }
}
