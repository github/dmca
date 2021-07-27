package awais.instagrabber.repositories.responses.feed;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

import awais.instagrabber.repositories.responses.Media;

public class EndOfFeedGroup implements Serializable {
    private final String id;
    private final String title;
    private final String nextMaxId;
    private final List<Media> feedItems;

    public EndOfFeedGroup(final String id, final String title, final String nextMaxId, final List<Media> feedItems) {
        this.id = id;
        this.title = title;
        this.nextMaxId = nextMaxId;
        this.feedItems = feedItems;
    }

    public String getId() {
        return id;
    }

    public String getTitle() {
        return title;
    }

    public String getNextMaxId() {
        return nextMaxId;
    }

    public List<Media> getFeedItems() {
        return feedItems;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final EndOfFeedGroup that = (EndOfFeedGroup) o;
        return Objects.equals(id, that.id) &&
                Objects.equals(title, that.title) &&
                Objects.equals(nextMaxId, that.nextMaxId) &&
                Objects.equals(feedItems, that.feedItems);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, title, nextMaxId, feedItems);
    }
}
