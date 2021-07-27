package awais.instagrabber.models;

import androidx.annotation.Nullable;

import java.io.Serializable;

public final class FollowModel implements Serializable {
    private final String id;
    private final String username;
    private final String fullName;
    private final String profilePicUrl;
    private String endCursor;
    private boolean hasNextPage;
    private boolean isShown = true;

    public FollowModel(final String id, final String username, final String fullName, final String profilePicUrl) {
        this.id = id;
        this.username = username;
        this.fullName = fullName;
        this.profilePicUrl = profilePicUrl;
    }

    public String getId() {
        return id;
    }

    public String getUsername() {
        return username;
    }

    public String getFullName() {
        return fullName;
    }

    public String getProfilePicUrl() {
        return profilePicUrl;
    }

    public boolean isShown() {
        return isShown;
    }

    public void setShown(final boolean shown) {
        isShown = shown;
    }

    public void setPageCursor(final boolean hasNextPage, final String endCursor) {
        this.endCursor = endCursor;
        this.hasNextPage = hasNextPage;
    }

    public boolean hasNextPage() {
        return endCursor != null && hasNextPage;
    }

    public String getEndCursor() {
        return endCursor;
    }

    @Override
    public boolean equals(@Nullable final Object obj) {
        if (obj instanceof FollowModel) {
            final FollowModel model = (FollowModel) obj;
            if (model.getId().equals(id) && model.getUsername().equals(username)) return true;
        }
        return super.equals(obj);
    }
}