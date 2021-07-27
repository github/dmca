package awais.instagrabber.repositories.responses.giphy;

import androidx.annotation.NonNull;

import java.util.Objects;

public class GiphyGif {
    private final String type;
    private final String id;
    private final String title;
    private final int isSticker;
    private final GiphyGifImages images;

    public GiphyGif(final String type, final String id, final String title, final int isSticker, final GiphyGifImages images) {
        this.type = type;
        this.id = id;
        this.title = title;
        this.isSticker = isSticker;
        this.images = images;
    }

    public String getType() {
        return type;
    }

    public String getId() {
        return id;
    }

    public String getTitle() {
        return title;
    }

    public boolean isSticker() {
        return isSticker ==  1;
    }

    public GiphyGifImages getImages() {
        return images;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final GiphyGif giphyGif = (GiphyGif) o;
        return isSticker == giphyGif.isSticker &&
                Objects.equals(type, giphyGif.type) &&
                Objects.equals(id, giphyGif.id) &&
                Objects.equals(title, giphyGif.title) &&
                Objects.equals(images, giphyGif.images);
    }

    @Override
    public int hashCode() {
        return Objects.hash(type, id, title, isSticker, images);
    }

    @NonNull
    @Override
    public String toString() {
        return "GiphyGif{" +
                "type='" + type + '\'' +
                ", id='" + id + '\'' +
                ", title='" + title + '\'' +
                ", isSticker=" + isSticker() +
                ", images=" + images +
                '}';
    }
}
