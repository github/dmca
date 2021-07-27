package awais.instagrabber.repositories.responses.directmessages;

import androidx.annotation.NonNull;

import java.io.Serializable;
import java.util.List;
import java.util.Map;

import awais.instagrabber.repositories.responses.User;

public class DirectThreadParticipantRequestsResponse implements Serializable, Cloneable {
    private List<User> users;
    private final Map<Long, String> requesterUsernames;
    private final String cursor;
    private final int totalThreadParticipants;
    private int totalParticipantRequests;
    private final String status;

    public DirectThreadParticipantRequestsResponse(final List<User> users,
                                                   final Map<Long, String> requesterUsernames,
                                                   final String cursor,
                                                   final int totalThreadParticipants,
                                                   final int totalParticipantRequests,
                                                   final String status) {
        this.users = users;
        this.requesterUsernames = requesterUsernames;
        this.cursor = cursor;
        this.totalThreadParticipants = totalThreadParticipants;
        this.totalParticipantRequests = totalParticipantRequests;
        this.status = status;
    }

    public List<User> getUsers() {
        return users;
    }

    public void setUsers(final List<User> users) {
        this.users = users;
    }

    public Map<Long, String> getRequesterUsernames() {
        return requesterUsernames;
    }

    public String getCursor() {
        return cursor;
    }

    public int getTotalThreadParticipants() {
        return totalThreadParticipants;
    }

    public int getTotalParticipantRequests() {
        return totalParticipantRequests;
    }

    public void setTotalParticipantRequests(final int totalParticipantRequests) {
        this.totalParticipantRequests = totalParticipantRequests;
    }

    public String getStatus() {
        return status;
    }

    @NonNull
    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }
}
