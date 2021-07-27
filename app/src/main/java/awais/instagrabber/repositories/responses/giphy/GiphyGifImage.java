package awais.instagrabber.repositories.responses.giphy;

import java.util.Objects;

public class GiphyGifImage {
    private final int height;
    private final int width;
    private final long webpSize;
    private final String webp;

    public GiphyGifImage(final int height, final int width, final long webpSize, final String webp) {
        this.height = height;
        this.width = width;
        this.webpSize = webpSize;
        this.webp = webp;
    }

    public int getHeight() {
        return height;
    }

    public int getWidth() {
        return width;
    }

    public long getWebpSize() {
        return webpSize;
    }

    public String getWebp() {
        return webp;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final GiphyGifImage that = (GiphyGifImage) o;
        return height == that.height &&
                width == that.width &&
                webpSize == that.webpSize &&
                Objects.equals(webp, that.webp);
    }

    @Override
    public int hashCode() {
        return Objects.hash(height, width, webpSize, webp);
    }

    @Override
    public String toString() {
        return "GiphyGifImage{" +
                "height=" + height +
                ", width=" + width +
                ", webpSize=" + webpSize +
                ", webp='" + webp + '\'' +
                '}';
    }
}
