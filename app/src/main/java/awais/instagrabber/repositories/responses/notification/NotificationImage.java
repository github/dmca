package awais.instagrabber.repositories.responses.notification;

public class NotificationImage {
    private final String id;
    private final String image;

    public NotificationImage(final String id, final String image) {
        this.id = id;
        this.image = image;
    }

    public String getId() {
        return id;
    }

    public String getImage() {
        return image;
    }
}
