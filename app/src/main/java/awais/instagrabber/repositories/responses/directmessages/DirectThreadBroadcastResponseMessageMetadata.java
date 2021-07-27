package awais.instagrabber.repositories.responses.directmessages;

import java.util.List;

public class DirectThreadBroadcastResponseMessageMetadata {
    private final String clientContext;
    private final String itemId;
    private final long timestamp;
    private final String threadId;
    private final List<String> participantIds;


    public DirectThreadBroadcastResponseMessageMetadata(final String clientContext,
                                                        final String itemId,
                                                        final long timestamp,
                                                        final String threadId,
                                                        final List<String> participantIds) {
        this.clientContext = clientContext;
        this.itemId = itemId;
        this.timestamp = timestamp;
        this.threadId = threadId;
        this.participantIds = participantIds;
    }

    public String getClientContext() {
        return clientContext;
    }

    public String getItemId() {
        return itemId;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public String getThreadId() {
        return threadId;
    }

    public List<String> getParticipantIds() {
        return participantIds;
    }

    @Override
    public String toString() {
        return "DirectThreadBroadcastResponseMessageMetadata{" +
                "clientContext='" + clientContext + '\'' +
                ", itemId='" + itemId + '\'' +
                ", timestamp=" + timestamp +
                ", threadId='" + threadId + '\'' +
                ", participantIds=" + participantIds +
                '}';
    }
}
