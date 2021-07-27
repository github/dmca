package awais.instagrabber.repositories.responses;

import java.util.List;

public class AymlUserList {
    private final List<AymlUser> suggestions;

    public AymlUserList(final List<AymlUser> suggestions) {
        this.suggestions = suggestions;
    }

    public List<AymlUser> getSuggestions() {
        return suggestions;
    }
}
