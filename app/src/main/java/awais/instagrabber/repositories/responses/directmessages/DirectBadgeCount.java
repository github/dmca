package awais.instagrabber.repositories.responses.directmessages;

public class DirectBadgeCount {
    private final long userId;
    private final int badgeCount;
    private final long badgeCountAtMs;
    private final String status;

    public DirectBadgeCount(final long userId, final int badgeCount, final long badgeCountAtMs, final String status) {
        this.userId = userId;
        this.badgeCount = badgeCount;
        this.badgeCountAtMs = badgeCountAtMs;
        this.status = status;
    }

    public long getUserId() {
        return userId;
    }

    public int getBadgeCount() {
        return badgeCount;
    }

    public long getBadgeCountAtMs() {
        return badgeCountAtMs;
    }

    public String getStatus() {
        return status;
    }
}
