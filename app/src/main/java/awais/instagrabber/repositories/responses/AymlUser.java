package awais.instagrabber.repositories.responses;

public class AymlUser {
    private final User user;
    private final String algorithm;
    private final String socialContext;
    private final String uuid;

    public AymlUser(final User user,
                    final String algorithm,
                    final String socialContext,
                    final String uuid) {
        this.user = user;
        this.algorithm = algorithm;
        this.socialContext = socialContext;
        this.uuid = uuid;
    }

    public User getUser() {
        return user;
    }

    public String getAlgorithm() {
        return algorithm;
    }

    public String getSocialContext() {
        return socialContext;
    }

    public String getUuid() {
        return uuid;
    }
}
