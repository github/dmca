package awais.instagrabber.repositories.responses;

import java.util.List;

public class PostsFetchResponse {
    private final List<Media> feedModels;
    private final boolean hasNextPage;
    private final String nextCursor;

    public PostsFetchResponse(final List<Media> feedModels, final boolean hasNextPage, final String nextCursor) {
        this.feedModels = feedModels;
        this.hasNextPage = hasNextPage;
        this.nextCursor = nextCursor;
    }

    public List<Media> getFeedModels() {
        return feedModels;
    }

    public boolean hasNextPage() {
        return hasNextPage;
    }

    public String getNextCursor() {
        return nextCursor;
    }
}
