package awais.instagrabber.repositories.responses;

public class LoginRequiredResponse {
    private String message = "login_required";
    private int logoutReason;
    private String status = "fail";

    public LoginRequiredResponse(final String message, final int logoutReason, final String status) {
        this.message = message;
        this.logoutReason = logoutReason;
        this.status = status;
    }

    public String getMessage() {
        return message;
    }

    public int getLogoutReason() {
        return logoutReason;
    }

    public String getStatus() {
        return status;
    }
}
