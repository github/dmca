package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class User implements Serializable {
    private final long pk;
    private final String username;
    private final String fullName;
    private final boolean isPrivate;
    private final String profilePicUrl;
    private final String profilePicId;
    private FriendshipStatus friendshipStatus;
    private final boolean isVerified;
    private final boolean hasAnonymousProfilePicture;
    private final boolean isUnpublished;
    private final boolean isFavorite;
    private final boolean isDirectappInstalled;
    private final boolean hasChaining;
    private final String reelAutoArchive;
    private final String allowedCommenterType;
    private final long mediaCount;
    private final long followerCount;
    private final long followingCount;
    private final long followingTagCount;
    private final String biography;
    private final String externalUrl;
    private final long usertagsCount;
    private final String publicEmail;
    private final HdProfilePicUrlInfo hdProfilePicUrlInfo;
    private final String profileContext; // "also followed by" your friends
    private final List<UserProfileContextLink> profileContextLinksWithUserIds; // ^
    private final String socialContext; // AYML
    private final String interopMessagingUserFbid; // in DMs only: Facebook user ID

    public User(final long pk,
                final String username,
                final String fullName,
                final boolean isPrivate,
                final String profilePicUrl,
                final String profilePicId,
                final FriendshipStatus friendshipStatus,
                final boolean isVerified,
                final boolean hasAnonymousProfilePicture,
                final boolean isUnpublished,
                final boolean isFavorite,
                final boolean isDirectappInstalled,
                final boolean hasChaining,
                final String reelAutoArchive,
                final String allowedCommenterType,
                final long mediaCount,
                final long followerCount,
                final long followingCount,
                final long followingTagCount,
                final String biography,
                final String externalUrl,
                final long usertagsCount,
                final String publicEmail,
                final HdProfilePicUrlInfo hdProfilePicUrlInfo,
                final String profileContext,
                final List<UserProfileContextLink> profileContextLinksWithUserIds,
                final String socialContext,
                final String interopMessagingUserFbid) {
        this.pk = pk;
        this.username = username;
        this.fullName = fullName;
        this.isPrivate = isPrivate;
        this.profilePicUrl = profilePicUrl;
        this.profilePicId = profilePicId;
        this.friendshipStatus = friendshipStatus;
        this.isVerified = isVerified;
        this.hasAnonymousProfilePicture = hasAnonymousProfilePicture;
        this.isUnpublished = isUnpublished;
        this.isFavorite = isFavorite;
        this.isDirectappInstalled = isDirectappInstalled;
        this.hasChaining = hasChaining;
        this.reelAutoArchive = reelAutoArchive;
        this.allowedCommenterType = allowedCommenterType;
        this.mediaCount = mediaCount;
        this.followerCount = followerCount;
        this.followingCount = followingCount;
        this.followingTagCount = followingTagCount;
        this.biography = biography;
        this.externalUrl = externalUrl;
        this.usertagsCount = usertagsCount;
        this.publicEmail = publicEmail;
        this.hdProfilePicUrlInfo = hdProfilePicUrlInfo;
        this.profileContext = profileContext;
        this.profileContextLinksWithUserIds = profileContextLinksWithUserIds;
        this.socialContext = socialContext;
        this.interopMessagingUserFbid = interopMessagingUserFbid;
    }

    public User(final long pk,
                final String username,
                final String fullName,
                final boolean isPrivate,
                final String profilePicUrl,
                final boolean isVerified) {
        this.pk = pk;
        this.username = username;
        this.fullName = fullName;
        this.isPrivate = isPrivate;
        this.profilePicUrl = profilePicUrl;
        this.profilePicId = null;
        this.friendshipStatus = new FriendshipStatus(
                false,
                false,
                false,
                false,
                false,
                false,
                false,
                false,
                false,
                false
        );
        this.isVerified = isVerified;
        this.hasAnonymousProfilePicture = false;
        this.isUnpublished = false;
        this.isFavorite = false;
        this.isDirectappInstalled = false;
        this.hasChaining = false;
        this.reelAutoArchive = null;
        this.allowedCommenterType = null;
        this.mediaCount = 0;
        this.followerCount = 0;
        this.followingCount = 0;
        this.followingTagCount = 0;
        this.biography = null;
        this.externalUrl = null;
        this.usertagsCount = 0;
        this.publicEmail = null;
        this.hdProfilePicUrlInfo = null;
        this.profileContext = null;
        this.profileContextLinksWithUserIds = null;
        this.socialContext = null;
        this.interopMessagingUserFbid = null;
    }

    public long getPk() {
        return pk;
    }

    public String getUsername() {
        return username;
    }

    public String getFullName() {
        return fullName;
    }

    public boolean isPrivate() {
        return isPrivate;
    }

    public String getProfilePicUrl() {
        return profilePicUrl;
    }

    public String getHDProfilePicUrl() {
        if (hdProfilePicUrlInfo == null) {
            return getProfilePicUrl();
        }
        return hdProfilePicUrlInfo.getUrl();
    }

    public String getProfilePicId() {
        return profilePicId;
    }

    public FriendshipStatus getFriendshipStatus() {
        return friendshipStatus;
    }

    public void setFriendshipStatus(final FriendshipStatus friendshipStatus) {
        this.friendshipStatus = friendshipStatus;
    }

    public boolean isVerified() {
        return isVerified;
    }

    public boolean hasAnonymousProfilePicture() {
        return hasAnonymousProfilePicture;
    }

    public boolean isUnpublished() {
        return isUnpublished;
    }

    public boolean isFavorite() {
        return isFavorite;
    }

    public boolean isDirectappInstalled() {
        return isDirectappInstalled;
    }

    public boolean hasChaining() {
        return hasChaining;
    }

    public String getReelAutoArchive() {
        return reelAutoArchive;
    }

    public String getAllowedCommenterType() {
        return allowedCommenterType;
    }

    public long getMediaCount() {
        return mediaCount;
    }

    public long getFollowerCount() {
        return followerCount;
    }

    public long getFollowingCount() {
        return followingCount;
    }

    public long getFollowingTagCount() {
        return followingTagCount;
    }

    public String getBiography() {
        return biography;
    }

    public String getExternalUrl() {
        return externalUrl;
    }

    public long getUsertagsCount() {
        return usertagsCount;
    }

    public String getPublicEmail() {
        return publicEmail;
    }

    public String getProfileContext() {
        return profileContext;
    }

    public String getSocialContext() {
        return socialContext;
    }

    public List<UserProfileContextLink> getProfileContextLinks() {
        return profileContextLinksWithUserIds;
    }

    public String getFbId() {
        return interopMessagingUserFbid;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final User user = (User) o;
        return pk == user.pk &&
                isPrivate == user.isPrivate &&
                isVerified == user.isVerified &&
                hasAnonymousProfilePicture == user.hasAnonymousProfilePicture &&
                isUnpublished == user.isUnpublished &&
                isFavorite == user.isFavorite &&
                isDirectappInstalled == user.isDirectappInstalled &&
                mediaCount == user.mediaCount &&
                followerCount == user.followerCount &&
                followingCount == user.followingCount &&
                followingTagCount == user.followingTagCount &&
                usertagsCount == user.usertagsCount &&
                Objects.equals(username, user.username) &&
                Objects.equals(fullName, user.fullName) &&
                Objects.equals(profilePicUrl, user.profilePicUrl) &&
                Objects.equals(profilePicId, user.profilePicId) &&
                Objects.equals(friendshipStatus, user.friendshipStatus) &&
                Objects.equals(reelAutoArchive, user.reelAutoArchive) &&
                Objects.equals(allowedCommenterType, user.allowedCommenterType) &&
                Objects.equals(biography, user.biography) &&
                Objects.equals(externalUrl, user.externalUrl) &&
                Objects.equals(publicEmail, user.publicEmail);
    }

    @Override
    public int hashCode() {
        return Objects.hash(pk, username, fullName, isPrivate, profilePicUrl, profilePicId, friendshipStatus, isVerified, hasAnonymousProfilePicture,
                            isUnpublished, isFavorite, isDirectappInstalled, hasChaining, reelAutoArchive, allowedCommenterType, mediaCount,
                            followerCount, followingCount, followingTagCount, biography, externalUrl, usertagsCount, publicEmail);
    }
}
