package awais.instagrabber.repositories.responses;

public class StoryStickerResponse {
    private final String status;

    public StoryStickerResponse(final String status) {
        this.status = status;
    }

    public String getStatus() {
        return status;
    }

    @Override
    public String toString() {
        return "StoryStickerResponse{" +
                "status='" + status + '\'' +
                '}';
    }
}
