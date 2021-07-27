package awais.instagrabber.repositories.responses;

import java.util.List;

import awais.instagrabber.repositories.responses.notification.Notification;
import awais.instagrabber.repositories.responses.notification.NotificationCounts;

public class NewsInboxResponse {
    private final NotificationCounts counts;
    private final List<Notification> newStories;
    private final List<Notification> oldStories;

    public NewsInboxResponse(final NotificationCounts counts,
                             final List<Notification> newStories,
                             final List<Notification> oldStories) {
        this.counts = counts;
        this.newStories = newStories;
        this.oldStories = oldStories;
    }

    public NotificationCounts getCounts() {
        return counts;
    }

    public List<Notification> getNewStories() {
        return newStories;
    }

    public List<Notification> getOldStories() {
        return oldStories;
    }
}
