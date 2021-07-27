package awais.instagrabber.repositories.responses;

import java.util.List;

public class UserSearchResponse {
    private final int numResults;
    private final List<User> users;
    private final boolean hasMore;
    private final String status;

    public UserSearchResponse(final int numResults, final List<User> users, final boolean hasMore, final String status) {
        this.numResults = numResults;
        this.users = users;
        this.hasMore = hasMore;
        this.status = status;
    }

    public int getNumResults() {
        return numResults;
    }

    public List<User> getUsers() {
        return users;
    }

    public boolean hasMore() {
        return hasMore;
    }

    public String getStatus() {
        return status;
    }
}
