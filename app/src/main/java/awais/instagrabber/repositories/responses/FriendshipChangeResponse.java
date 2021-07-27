package awais.instagrabber.repositories.responses;

import androidx.annotation.NonNull;

public class FriendshipChangeResponse {
    private final FriendshipStatus friendshipStatus;
    private final String status;

    public FriendshipChangeResponse(final FriendshipStatus friendshipStatus,
                                    final String status) {
        this.friendshipStatus = friendshipStatus;
        this.status = status;
    }

    public FriendshipStatus getFriendshipStatus() {
        return friendshipStatus;
    }

    public String getStatus() {
        return status;
    }

    @NonNull
    @Override
    public String toString() {
        return "FriendshipChangeResponse{" +
                "friendshipStatus=" + friendshipStatus +
                ", status='" + status + '\'' +
                '}';
    }
}
