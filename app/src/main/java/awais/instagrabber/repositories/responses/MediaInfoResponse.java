package awais.instagrabber.repositories.responses;

import java.util.List;

public class MediaInfoResponse {
    private final List<Media> items;

    public MediaInfoResponse(final List<Media> items) {
        this.items = items;
    }

    public List<Media> getItems() {
        return items;
    }
}
