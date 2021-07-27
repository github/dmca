package awais.instagrabber.repositories.responses.feed;

import java.util.List;

import awais.instagrabber.repositories.responses.Media;

public class FeedFetchResponse {
    private final List<Media> items;
    private final int numResults;
    private final boolean moreAvailable;
    private final String nextMaxId;
    private final String status;

    public FeedFetchResponse(final List<Media> items,
                             final int numResults,
                             final boolean moreAvailable,
                             final String nextMaxId,
                             final String status) {
        this.items = items;
        this.numResults = numResults;
        this.moreAvailable = moreAvailable;
        this.nextMaxId = nextMaxId;
        this.status = status;
    }

    public List<Media> getItems() {
        return items;
    }

    public int getNumResults() {
        return numResults;
    }

    public boolean isMoreAvailable() {
        return moreAvailable;
    }

    public String getNextMaxId() {
        return nextMaxId;
    }

    public String getStatus() {
        return status;
    }
}
