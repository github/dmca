package awais.instagrabber.repositories.responses;

import androidx.annotation.NonNull;

import java.util.List;

public class FriendshipRestrictResponse {
    private final List<User> users;
    private final String status;

    public FriendshipRestrictResponse(final List<User> users, final String status) {
        this.users = users;
        this.status = status;
    }

    public List<User> getUsers() {
        return users;
    }

    public String getStatus() {
        return status;
    }

    @NonNull
    @Override
    public String toString() {
        return "FriendshipRestrictResponse{" +
                "users=" + users +
                ", status='" + status + '\'' +
                '}';
    }
}
