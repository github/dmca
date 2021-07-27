package awais.instagrabber.repositories.responses;

import java.util.List;

public class TagFeedResponse {
    private final int numResults;
    private final String nextMaxId;
    private final boolean moreAvailable;
    private final String status;
    private final List<Media> items;

    public TagFeedResponse(final int numResults,
                           final String nextMaxId,
                           final boolean moreAvailable,
                           final String status,
                           final List<Media> items) {
        this.numResults = numResults;
        this.nextMaxId = nextMaxId;
        this.moreAvailable = moreAvailable;
        this.status = status;
        this.items = items;
    }

    public int getNumResults() {
        return numResults;
    }

    public String getNextMaxId() {
        return nextMaxId;
    }

    public boolean isMoreAvailable() {
        return moreAvailable;
    }

    public String getStatus() {
        return status;
    }

    public List<Media> getItems() {
        return items;
    }
}
