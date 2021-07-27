package awais.instagrabber.repositories.responses;

import java.util.List;

public class AymlResponse {
    private final AymlUserList newSuggestedUsers;
    private final AymlUserList suggestedUsers;

    public AymlResponse(final AymlUserList newSuggestedUsers,
                        final AymlUserList suggestedUsers) {
        this.newSuggestedUsers = newSuggestedUsers;
        this.suggestedUsers = suggestedUsers;
    }

    public AymlUserList getNewSuggestedUsers() {
        return newSuggestedUsers;
    }

    public AymlUserList getSuggestedUsers() {
        return suggestedUsers;
    }
}
