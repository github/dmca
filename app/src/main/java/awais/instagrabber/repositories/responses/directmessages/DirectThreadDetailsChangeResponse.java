package awais.instagrabber.repositories.responses.directmessages;

public class DirectThreadDetailsChangeResponse {
    private final DirectThread thread;
    private final String status;

    public DirectThreadDetailsChangeResponse(final DirectThread thread, final String status) {
        this.thread = thread;
        this.status = status;
    }

    public DirectThread getThread() {
        return thread;
    }

    public String getStatus() {
        return status;
    }
}
