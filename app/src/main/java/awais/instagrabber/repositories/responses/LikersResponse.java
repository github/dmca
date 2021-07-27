package awais.instagrabber.repositories.responses;

import java.util.List;

public class LikersResponse {
    private final List<User> users;
    private final long userCount;
    private final String status;

    public LikersResponse(final List<User> users, final long userCount, final String status) {
        this.users = users;
        this.userCount = userCount;
        this.status = status;
    }

    public List<User> getUsers() {
        return users;
    }

    public long getUserCount() {
        return userCount;
    }

    public String getStatus() {
        return status;
    }
}
