package awais.instagrabber.repositories.responses.directmessages;

import java.util.List;

public class RankedRecipientsResponse {
    private final List<RankedRecipient> rankedRecipients;
    private final long expires;
    private final boolean filtered;
    private final String requestId;
    private final String rankToken;
    private final String status;

    public RankedRecipientsResponse(final List<RankedRecipient> rankedRecipients,
                                    final long expires,
                                    final boolean filtered,
                                    final String requestId,
                                    final String rankToken,
                                    final String status) {
        this.rankedRecipients = rankedRecipients;
        this.expires = expires;
        this.filtered = filtered;
        this.requestId = requestId;
        this.rankToken = rankToken;
        this.status = status;
    }

    public List<RankedRecipient> getRankedRecipients() {
        return rankedRecipients;
    }

    public long getExpires() {
        return expires;
    }

    public boolean isFiltered() {
        return filtered;
    }

    public String getRequestId() {
        return requestId;
    }

    public String getRankToken() {
        return rankToken;
    }

    public String getStatus() {
        return status;
    }
}
