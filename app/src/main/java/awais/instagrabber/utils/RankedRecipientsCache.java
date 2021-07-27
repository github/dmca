package awais.instagrabber.utils;

import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;
import java.util.Collections;
import java.util.List;

import awais.instagrabber.repositories.responses.directmessages.RankedRecipient;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipientsResponse;

public class RankedRecipientsCache {
    private static final Object LOCK = new Object();

    private static RankedRecipientsCache instance;

    public static RankedRecipientsCache getInstance() {
        if (instance == null) {
            synchronized (LOCK) {
                if (instance == null) {
                    instance = new RankedRecipientsCache();
                }
            }
        }
        return instance;
    }

    private LocalDateTime lastUpdatedOn;
    private RankedRecipientsResponse response;
    private boolean updateInitiated = false;
    private boolean failed = false;

    private RankedRecipientsCache() {}

    public List<RankedRecipient> getRankedRecipients() {
        if (response != null) {
            return response.getRankedRecipients();
        }
        return Collections.emptyList();
    }

    public void setRankedRecipientsResponse(final RankedRecipientsResponse response) {
        this.response = response;
        lastUpdatedOn = LocalDateTime.now();
    }

    public boolean isExpired() {
        if (lastUpdatedOn == null || response == null) {
            return true;
        }
        final long expiresInSecs = response.getExpires();
        return LocalDateTime.now().isAfter(lastUpdatedOn.plus(expiresInSecs, ChronoUnit.SECONDS));
    }

    public boolean isUpdateInitiated() {
        return updateInitiated;
    }

    public void setUpdateInitiated(final boolean updateInitiated) {
        this.updateInitiated = updateInitiated;
    }

    public boolean isFailed() {
        return failed;
    }

    public void setFailed(final boolean failed) {
        this.failed = failed;
    }
}
