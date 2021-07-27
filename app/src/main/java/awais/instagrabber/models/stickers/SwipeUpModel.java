package awais.instagrabber.models.stickers;

import java.io.Serializable;

public final class SwipeUpModel implements Serializable {
    private final String url, text;

    public SwipeUpModel(final String url, final String text) {
        this.url = url;
        this.text = text;
    }

    public String getUrl() {
        return url;
    }

    public String getText() {
        return text;
    }
}