package awais.instagrabber.repositories.responses.directmessages;

import androidx.annotation.NonNull;

import java.util.Objects;

public class DirectItemSeenResponse {
    private final String action;
    private final DirectItemSeenResponsePayload payload;
    private final String status;

    public DirectItemSeenResponse(final String action, final DirectItemSeenResponsePayload payload, final String status) {
        this.action = action;
        this.payload = payload;
        this.status = status;
    }

    public String getAction() {
        return action;
    }

    public DirectItemSeenResponsePayload getPayload() {
        return payload;
    }

    public String getStatus() {
        return status;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemSeenResponse that = (DirectItemSeenResponse) o;
        return Objects.equals(action, that.action) &&
                Objects.equals(payload, that.payload) &&
                Objects.equals(status, that.status);
    }

    @Override
    public int hashCode() {
        return Objects.hash(action, payload, status);
    }

    @NonNull
    @Override
    public String toString() {
        return "DirectItemSeenResponse{" +
                "action='" + action + '\'' +
                ", payload=" + payload +
                ", status='" + status + '\'' +
                '}';
    }

    public static class DirectItemSeenResponsePayload {
        private final int count;
        private final String timestamp;

        public DirectItemSeenResponsePayload(final int count, final String timestamp) {
            this.count = count;
            this.timestamp = timestamp;
        }

        public int getCount() {
            return count;
        }

        public String getTimestamp() {
            return timestamp;
        }

        @Override
        public boolean equals(final Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            final DirectItemSeenResponsePayload that = (DirectItemSeenResponsePayload) o;
            return count == that.count &&
                    Objects.equals(timestamp, that.timestamp);
        }

        @Override
        public int hashCode() {
            return Objects.hash(count, timestamp);
        }

        @NonNull
        @Override
        public String toString() {
            return "DirectItemSeenResponsePayload{" +
                    "count=" + count +
                    ", timestamp='" + timestamp + '\'' +
                    '}';
        }
    }
}
