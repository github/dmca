package awais.instagrabber.repositories.responses;

import java.util.List;

public class WrappedFeedResponse {
    private final int numResults;
    private final String nextMaxId;
    private final boolean moreAvailable;
    private final String status;
    private final List<WrappedMedia> items;

    public WrappedFeedResponse(final int numResults,
                               final String nextMaxId,
                               final boolean moreAvailable,
                               final String status,
                               final List<WrappedMedia> items) {
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

    public List<WrappedMedia> getItems() {
        return items;
    }
}
