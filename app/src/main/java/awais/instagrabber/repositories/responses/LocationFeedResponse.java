package awais.instagrabber.repositories.responses;

import java.util.List;

public class LocationFeedResponse {
    private final int numResults;
    private final String nextMaxId;
    private final boolean moreAvailable;
    private final long mediaCount;
    private final String status;
    private final List<Media> items;
    private final Location location;

    public LocationFeedResponse(final int numResults,
                                final String nextMaxId,
                                final boolean moreAvailable,
                                final long mediaCount,
                                final String status,
                                final List<Media> items,
                                final Location location) {
        this.numResults = numResults;
        this.nextMaxId = nextMaxId;
        this.moreAvailable = moreAvailable;
        this.mediaCount = mediaCount;
        this.status = status;
        this.items = items;
        this.location = location;
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

    public long getMediaCount() {
        return mediaCount;
    }

    public Location getLocation() {
        return location;
    }
}
