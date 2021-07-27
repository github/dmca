package awais.instagrabber.repositories.responses;

public class HdProfilePicUrlInfo {
    private final String url;
    private final int width, height;

    public HdProfilePicUrlInfo(final String url, final int width, final int height) {
        this.url = url;
        this.width = width;
        this.height = height;
    }

    public String getUrl() {
        return url;
    }
}
