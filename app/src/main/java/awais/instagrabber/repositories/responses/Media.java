package awais.instagrabber.repositories.responses;

import androidx.annotation.Nullable;

import java.io.Serializable;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.feed.EndOfFeedDemarcator;
import awais.instagrabber.utils.Utils;

public class Media implements Serializable {
    private final String pk;
    private final String id;
    private final String code;
    private final long takenAt;
    private final User user;
    private final MediaItemType mediaType;
    private final boolean canViewerReshare;
    private final boolean commentLikesEnabled;
    private final boolean commentsDisabled;
    private final long nextMaxId;
    private final long commentCount;
    private final ImageVersions2 imageVersions2;
    private final int originalWidth;
    private final int originalHeight;
    private long likeCount;
    private boolean hasLiked;
    private final boolean isReelMedia;
    private final List<VideoVersion> videoVersions;
    private final boolean hasAudio;
    private final double videoDuration;
    private final long viewCount;
    private Caption caption;
    private final boolean canViewerSave;
    private final Audio audio;
    private final String title;
    private final Location location;
    private final Usertags usertags;
    private final List<Media> carouselMedia;
    private boolean isSidecarChild;
    private boolean hasViewerSaved;
    private final Map<String, Object> injected;
    private final EndOfFeedDemarcator endOfFeedDemarcator;

    private String dateString;

    public Media(final String pk,
                 final String id,
                 final String code,
                 final long takenAt,
                 final User user,
                 final boolean canViewerReshare,
                 final ImageVersions2 imageVersions2,
                 final int originalWidth,
                 final int originalHeight,
                 final MediaItemType mediaType,
                 final boolean commentLikesEnabled,
                 final boolean commentsDisabled,
                 final long nextMaxId,
                 final long commentCount,
                 final long likeCount,
                 final boolean hasLiked,
                 final boolean isReelMedia,
                 final List<VideoVersion> videoVersions,
                 final boolean hasAudio,
                 final double videoDuration,
                 final long viewCount,
                 final Caption caption,
                 final boolean canViewerSave,
                 final Audio audio,
                 final String title,
                 final List<Media> carouselMedia,
                 final Location location,
                 final Usertags usertags,
                 final boolean isSidecarChild,
                 final boolean hasViewerSaved,
                 final Map<String, Object> injected,
                 final EndOfFeedDemarcator endOfFeedDemarcator) {
        this.pk = pk;
        this.id = id;
        this.code = code;
        this.takenAt = takenAt;
        this.user = user;
        this.canViewerReshare = canViewerReshare;
        this.imageVersions2 = imageVersions2;
        this.originalWidth = originalWidth;
        this.originalHeight = originalHeight;
        this.mediaType = mediaType;
        this.commentLikesEnabled = commentLikesEnabled;
        this.commentsDisabled = commentsDisabled;
        this.nextMaxId = nextMaxId;
        this.commentCount = commentCount;
        this.likeCount = likeCount;
        this.hasLiked = hasLiked;
        this.isReelMedia = isReelMedia;
        this.videoVersions = videoVersions;
        this.hasAudio = hasAudio;
        this.videoDuration = videoDuration;
        this.viewCount = viewCount;
        this.caption = caption;
        this.canViewerSave = canViewerSave;
        this.audio = audio;
        this.title = title;
        this.carouselMedia = carouselMedia;
        this.location = location;
        this.usertags = usertags;
        this.isSidecarChild = isSidecarChild;
        this.hasViewerSaved = hasViewerSaved;
        this.injected = injected;
        this.endOfFeedDemarcator = endOfFeedDemarcator;
    }

    public String getPk() {
        return pk;
    }

    public String getId() {
        return id;
    }

    public String getCode() {
        return code;
    }

    public long getTakenAt() {
        return takenAt;
    }

    @Nullable
    public User getUser() {
        return user;
    }

    public boolean canViewerReshare() {
        return canViewerReshare;
    }

    public ImageVersions2 getImageVersions2() {
        return imageVersions2;
    }

    public int getOriginalWidth() {
        return originalWidth;
    }

    public int getOriginalHeight() {
        return originalHeight;
    }

    public MediaItemType getMediaType() {
        return mediaType;
    }

    public boolean isReelMedia() {
        return isReelMedia;
    }

    public List<VideoVersion> getVideoVersions() {
        return videoVersions;
    }

    public boolean isHasAudio() {
        return hasAudio;
    }

    public Caption getCaption() {
        return caption;
    }

    public Audio getAudio() {
        return audio;
    }

    public String getTitle() {
        return title;
    }

    public List<Media> getCarouselMedia() {
        return carouselMedia;
    }

    public boolean isCommentLikesEnabled() {
        return commentLikesEnabled;
    }

    public boolean isCommentsDisabled() {
        return commentsDisabled;
    }

    public long getNextMaxId() {
        return nextMaxId;
    }

    public long getCommentCount() {
        return commentCount;
    }

    public long getLikeCount() {
        return likeCount;
    }

    public boolean hasLiked() {
        return hasLiked;
    }

    public double getVideoDuration() {
        return videoDuration;
    }

    public long getViewCount() {
        return viewCount;
    }

    public boolean canViewerSave() {
        return canViewerSave;
    }

    public Location getLocation() {
        return location;
    }

    public Usertags getUsertags() {
        return usertags;
    }

    public void setIsSidecarChild(boolean isSidecarChild) {
        this.isSidecarChild = isSidecarChild;
    }

    public boolean isSidecarChild() {
        return isSidecarChild;
    }

    public boolean hasViewerSaved() {
        return hasViewerSaved;
    }

    public boolean isInjected() {
        return injected != null;
    }

    @Nullable
    public String getDate() {
        if (takenAt <= 0) {
            return null;
        }
        if (dateString == null) {
            dateString = Utils.datetimeParser.format(new Date(takenAt * 1000L));
        }
        return dateString;
    }

    public EndOfFeedDemarcator getEndOfFeedDemarcator() {
        return endOfFeedDemarcator;
    }

    public void setHasLiked(final boolean liked) {
        this.hasLiked = liked;
    }

    public void setLikeCount(final long likeCount) {
        this.likeCount = likeCount;
    }

    public void setHasViewerSaved(final boolean hasViewerSaved) {
        this.hasViewerSaved = hasViewerSaved;
    }

    public void setPostCaption(final String caption) {
        Caption caption1 = getCaption();
        if (caption1 == null) {
            final User user = getUser();
            if (user == null) return;
            caption1 = new Caption(user.getPk(), caption);
            this.caption = caption1;
            return;
        }
        caption1.setText(caption);
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Media media = (Media) o;
        return takenAt == media.takenAt &&
                canViewerReshare == media.canViewerReshare &&
                commentLikesEnabled == media.commentLikesEnabled &&
                commentsDisabled == media.commentsDisabled &&
                nextMaxId == media.nextMaxId &&
                commentCount == media.commentCount &&
                originalWidth == media.originalWidth &&
                originalHeight == media.originalHeight &&
                likeCount == media.likeCount &&
                hasLiked == media.hasLiked &&
                isReelMedia == media.isReelMedia &&
                hasAudio == media.hasAudio &&
                Double.compare(media.videoDuration, videoDuration) == 0 &&
                viewCount == media.viewCount &&
                canViewerSave == media.canViewerSave &&
                isSidecarChild == media.isSidecarChild &&
                hasViewerSaved == media.hasViewerSaved &&
                Objects.equals(pk, media.pk) &&
                Objects.equals(id, media.id) &&
                Objects.equals(code, media.code) &&
                Objects.equals(user, media.user) &&
                mediaType == media.mediaType &&
                Objects.equals(imageVersions2, media.imageVersions2) &&
                Objects.equals(videoVersions, media.videoVersions) &&
                Objects.equals(caption, media.caption) &&
                Objects.equals(audio, media.audio) &&
                Objects.equals(title, media.title) &&
                Objects.equals(location, media.location) &&
                Objects.equals(usertags, media.usertags) &&
                Objects.equals(carouselMedia, media.carouselMedia) &&
                Objects.equals(injected, media.injected) &&
                Objects.equals(endOfFeedDemarcator, media.endOfFeedDemarcator) &&
                Objects.equals(dateString, media.dateString);
    }

    @Override
    public int hashCode() {
        return Objects.hash(pk, id, code, takenAt, user, mediaType, canViewerReshare, commentLikesEnabled, commentsDisabled, nextMaxId, commentCount,
                            imageVersions2, originalWidth, originalHeight, likeCount, hasLiked, isReelMedia, videoVersions, hasAudio, videoDuration,
                            viewCount, caption, canViewerSave, audio, title, location, usertags, carouselMedia, isSidecarChild, hasViewerSaved,
                            injected, endOfFeedDemarcator, dateString);
    }
}
