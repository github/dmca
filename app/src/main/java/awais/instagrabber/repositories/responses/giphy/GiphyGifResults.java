package awais.instagrabber.repositories.responses.giphy;

import androidx.annotation.NonNull;

import java.util.List;
import java.util.Objects;

public class GiphyGifResults {
    private final List<GiphyGif> giphyGifs;
    private final List<GiphyGif> giphy;

    public GiphyGifResults(final List<GiphyGif> giphyGifs, final List<GiphyGif> giphy) {
        this.giphyGifs = giphyGifs;
        this.giphy = giphy;
    }

    public List<GiphyGif> getGiphyGifs() {
        return giphyGifs;
    }

    public List<GiphyGif> getGiphy() {
        return giphy;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final GiphyGifResults that = (GiphyGifResults) o;
        return Objects.equals(giphyGifs, that.giphyGifs) &&
                Objects.equals(giphy, that.giphy);
    }

    @Override
    public int hashCode() {
        return Objects.hash(giphyGifs, giphy);
    }

    @NonNull
    @Override
    public String toString() {
        return "GiphyGifResults{" +
                "giphyGifs=" + giphyGifs +
                ", giphy=" + giphy +
                '}';
    }
}
