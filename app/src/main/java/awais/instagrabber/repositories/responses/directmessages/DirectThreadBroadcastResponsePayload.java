package awais.instagrabber.repositories.responses.directmessages;

public class DirectThreadBroadcastResponsePayload {
    private final String clientContext;
    private final String itemId;
    private final long timestamp;
    private final String threadId;

    public DirectThreadBroadcastResponsePayload(final String clientContext,
                                                final String itemId,
                                                final long timestamp,
                                                final String threadId) {
        this.clientContext = clientContext;
        this.itemId = itemId;
        this.timestamp = timestamp;
        this.threadId = threadId;
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

    @Override
    public String toString() {
        return "DirectThreadBroadcastResponsePayload{" +
                "clientContext='" + clientContext + '\'' +
                ", itemId='" + itemId + '\'' +
                ", timestamp=" + timestamp +
                ", threadId='" + threadId + '\'' +
                '}';
    }
}
