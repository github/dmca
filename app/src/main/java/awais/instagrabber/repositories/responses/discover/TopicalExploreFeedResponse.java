package awais.instagrabber.repositories.responses.discover;

import java.util.List;
import awais.instagrabber.repositories.responses.WrappedMedia;

public class TopicalExploreFeedResponse {
    private final boolean moreAvailable;
    private final String nextMaxId;
    private final String maxId;
    private final String status;
    private final int numResults;
    private final List<TopicCluster> clusters;
    private final List<WrappedMedia> items;

    public TopicalExploreFeedResponse(final boolean moreAvailable,
                                      final String nextMaxId,
                                      final String maxId,
                                      final String status,
                                      final int numResults,
                                      final List<TopicCluster> clusters,
                                      final List<WrappedMedia> items) {
        this.moreAvailable = moreAvailable;
        this.nextMaxId = nextMaxId;
        this.maxId = maxId;
        this.status = status;
        this.numResults = numResults;
        this.clusters = clusters;
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

    public int getNumResults() {
        return numResults;
    }

    public List<TopicCluster> getClusters() {
        return clusters;
    }

    public List<WrappedMedia> getItems() {
        return items;
    }
}
