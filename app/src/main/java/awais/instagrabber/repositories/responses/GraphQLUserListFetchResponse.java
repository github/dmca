package awais.instagrabber.repositories.responses;

import androidx.annotation.NonNull;

import java.util.List;
import java.util.Objects;

import awais.instagrabber.utils.TextUtils;

public class GraphQLUserListFetchResponse {
    private String nextMaxId;
    private String status;
    private List<User> items;

    public GraphQLUserListFetchResponse(final String nextMaxId,
                                        final String status,
                                        final List<User> items) {
        this.nextMaxId = nextMaxId;
        this.status = status;
        this.items = items;
    }

    public boolean isMoreAvailable() {
        return !TextUtils.isEmpty(nextMaxId);
    }

    public String getNextMaxId() {
        return nextMaxId;
    }

    public GraphQLUserListFetchResponse setNextMaxId(final String nextMaxId) {
        this.nextMaxId = nextMaxId;
        return this;
    }

    public String getStatus() {
        return status;
    }

    public GraphQLUserListFetchResponse setStatus(final String status) {
        this.status = status;
        return this;
    }

    public List<User> getItems() {
        return items;
    }

    public GraphQLUserListFetchResponse setItems(final List<User> items) {
        this.items = items;
        return this;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final GraphQLUserListFetchResponse that = (GraphQLUserListFetchResponse) o;
        return Objects.equals(nextMaxId, that.nextMaxId) &&
                Objects.equals(status, that.status) &&
                Objects.equals(items, that.items);
    }

    @Override
    public int hashCode() {
        return Objects.hash(nextMaxId, status, items);
    }

    @NonNull
    @Override
    public String toString() {
        return "GraphQLUserListFetchResponse{" +
                "nextMaxId='" + nextMaxId + '\'' +
                ", status='" + status + '\'' +
                ", items=" + items +
                '}';
    }
}
