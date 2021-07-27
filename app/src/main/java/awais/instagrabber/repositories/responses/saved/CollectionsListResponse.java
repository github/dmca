package awais.instagrabber.repositories.responses.saved;

import java.util.List;

public class CollectionsListResponse {
    private final boolean moreAvailable;
    private final String nextMaxId;
    private final String maxId;
    private final String status;
//    private final int numResults;
    private final List<SavedCollection> items;

    public CollectionsListResponse(final boolean moreAvailable,
                                   final String nextMaxId,
                                   final String maxId,
                                   final String status,
//                                   final int numResults,
                                   final List<SavedCollection> items) {
        this.moreAvailable = moreAvailable;
        this.nextMaxId = nextMaxId;
        this.maxId = maxId;
        this.status = status;
//        this.numResults = numResults;
        this.items = items;
    }

    public boolean isMoreAvailable() {
        return moreAvailable;
    }

    public String getNextMaxId() {
        return nextMaxId;
    }

    public String getMaxId() {
        return maxId;
    }

    public String getStatus() {
        return status;
    }

//    public int getNumResults() {
//        return numResults;
//    }

    public List<SavedCollection> getItems() {
        return items;
    }
}
