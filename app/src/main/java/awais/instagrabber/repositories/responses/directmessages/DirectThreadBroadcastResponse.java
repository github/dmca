package awais.instagrabber.repositories.responses.directmessages;

import androidx.annotation.NonNull;

import java.util.List;

public class DirectThreadBroadcastResponse {
    private final String action;
    private final String statusCode;
    private final DirectThreadBroadcastResponsePayload payload;
    private final List<DirectThreadBroadcastResponseMessageMetadata> messageMetadata;
    private final String status;

    public DirectThreadBroadcastResponse(final String action,
                                         final String statusCode,
                                         final DirectThreadBroadcastResponsePayload payload,
                                         final List<DirectThreadBroadcastResponseMessageMetadata> messageMetadata,
                                         final String status) {
        this.action = action;
        this.statusCode = statusCode;
        this.payload = payload;
        this.messageMetadata = messageMetadata;
        this.status = status;
    }

    public String getAction() {
        return action;
    }

    public String getStatusCode() {
        return statusCode;
    }

    public DirectThreadBroadcastResponsePayload getPayload() {
        return payload;
    }

    public List<DirectThreadBroadcastResponseMessageMetadata> getMessageMetadata() {
        return messageMetadata;
    }

    public String getStatus() {
        return status;
    }

    @NonNull
    @Override
    public String toString() {
        return "DirectThreadBroadcastResponse{" +
                "action='" + action + '\'' +
                ", statusCode='" + statusCode + '\'' +
                ", payload=" + payload +
                ", messageMetadata=" + messageMetadata +
                ", status='" + status + '\'' +
                '}';
    }
}
