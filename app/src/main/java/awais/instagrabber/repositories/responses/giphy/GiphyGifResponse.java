package awais.instagrabber.repositories.responses.giphy;

import androidx.annotation.NonNull;

import java.util.Objects;

public class GiphyGifResponse {
    private final GiphyGifResults results;
    private final String status;

    public GiphyGifResponse(final GiphyGifResults results, final String status) {
        this.results = results;
        this.status = status;
    }

    public GiphyGifResults getResults() {
        return results;
    }

    public String getStatus() {
        return status;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final GiphyGifResponse that = (GiphyGifResponse) o;
        return Objects.equals(results, that.results) &&
                Objects.equals(status, that.status);
    }

    @Override
    public int hashCode() {
        return Objects.hash(results, status);
    }

    @NonNull
    @Override
    public String toString() {
        return "GiphyGifResponse{" +
                "results=" + results +
                ", status='" + status + '\'' +
                '}';
    }
}
