package awais.instagrabber.repositories.responses.giphy;

import androidx.annotation.NonNull;

import java.util.Objects;

import awais.instagrabber.repositories.responses.AnimatedMediaFixedHeight;

public class GiphyGifImages {
    private final AnimatedMediaFixedHeight fixedHeight;

    public GiphyGifImages(final AnimatedMediaFixedHeight fixedHeight) {
        this.fixedHeight = fixedHeight;
    }

    public AnimatedMediaFixedHeight getFixedHeight() {
        return fixedHeight;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final GiphyGifImages that = (GiphyGifImages) o;
        return Objects.equals(fixedHeight, that.fixedHeight);
    }

    @Override
    public int hashCode() {
        return Objects.hash(fixedHeight);
    }

    @NonNull
    @Override
    public String toString() {
        return "GiphyGifImages{" +
                "fixedHeight=" + fixedHeight +
                '}';
    }
}
