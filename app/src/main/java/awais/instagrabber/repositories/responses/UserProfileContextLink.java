package awais.instagrabber.repositories.responses;

public class UserProfileContextLink {
    private final String username;
    private final int start;
    private final int end;

    public UserProfileContextLink(final String username, final int start, final int end) {
        this.username = username;
        this.start = start;
        this.end = end;
    }

    public String getUsername() {
        return username;
    }

    public int getStart() {
        return start;
    }
}
