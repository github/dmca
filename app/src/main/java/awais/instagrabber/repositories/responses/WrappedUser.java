package awais.instagrabber.repositories.responses;

public class WrappedUser {
    private final User user;

    public WrappedUser(final User user) {
        this.user = user;
    }

    public User getUser() {
        return user;
    }
}
