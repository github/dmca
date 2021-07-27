package awais.instagrabber.utils;

import android.content.res.Resources;

import androidx.annotation.NonNull;

import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;

import awais.instagrabber.R;
import awais.instagrabber.models.enums.DirectItemType;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemAnimatedMedia;
import awais.instagrabber.repositories.responses.directmessages.DirectItemReelShare;
import awais.instagrabber.repositories.responses.directmessages.DirectItemVisualMedia;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadLastSeenAt;
import awais.instagrabber.repositories.responses.directmessages.RavenExpiringMediaActionSummary;

public final class DMUtils {
    public static boolean isRead(@NonNull final DirectItem item,
                                 @NonNull final Map<Long, DirectThreadLastSeenAt> lastSeenAt,
                                 @NonNull final List<Long> userIdsToCheck) {
        return lastSeenAt.entrySet()
                         .stream()
                         .filter(entry -> userIdsToCheck.contains(entry.getKey()))
                         .anyMatch(entry -> {
                             final DirectThreadLastSeenAt threadLastSeenAt = entry.getValue();
                             if (threadLastSeenAt == null) return false;
                             final String userLastSeenTsString = threadLastSeenAt.getTimestamp();
                             if (userLastSeenTsString == null) return false;
                             final long userTs = Long.parseLong(userLastSeenTsString);
                             final long itemTs = item.getTimestamp();
                             return userTs >= itemTs;
                         });
    }

    public static boolean isRead(@NonNull final DirectThread thread) {
        final boolean read;
        // if (thread.getDirectStory() != null) {
        //     return false;
        // }
        final DirectItem item = thread.getFirstDirectItem();
        final long viewerId = thread.getViewerId();
        if (item != null && item.getUserId() == viewerId) {
            // if last item was sent by user, then it is read (even though we have auto read unchecked?)
            read = true;
        } else {
            final Map<Long, DirectThreadLastSeenAt> lastSeenAtMap = thread.getLastSeenAt();
            read = item != null && isRead(item, lastSeenAtMap, Collections.singletonList(viewerId));
        }
        return read;
    }

    public static String getMessageString(@NonNull final DirectThread thread,
                                          final Resources resources,
                                          final long viewerId,
                                          final DirectItem item) {
        final long senderId = item.getUserId();
        final DirectItemType itemType = item.getItemType();
        String subtitle = null;
        final String username = getUsername(thread.getUsers(), senderId, viewerId, resources);
        String message = "";
        if (itemType == null) {
            message = resources.getString(R.string.dms_inbox_raven_message_unknown);
        } else {
            switch (itemType) {
                case TEXT:
                    message = item.getText();
                    break;
                case LIKE:
                    message = item.getLike();
                    break;
                case LINK:
                    message = item.getLink().getText();
                    break;
                case PLACEHOLDER:
                    message = item.getPlaceholder().getMessage();
                    break;
                case MEDIA_SHARE:
                    final Media mediaShare = item.getMediaShare();
                    User mediaShareUser = null;
                    if (mediaShare != null) {
                        mediaShareUser = mediaShare.getUser();
                    }
                    subtitle = resources.getString(R.string.dms_inbox_shared_post,
                                                   username != null ? username : "",
                                                   mediaShareUser == null ? "" : mediaShareUser.getUsername());
                    break;
                case ANIMATED_MEDIA:
                    final DirectItemAnimatedMedia animatedMedia = item.getAnimatedMedia();
                    subtitle = resources.getString(animatedMedia.isSticker() ? R.string.dms_inbox_shared_sticker
                                                                             : R.string.dms_inbox_shared_gif,
                                                   username != null ? username : "");
                    break;
                case PROFILE:
                    subtitle = resources
                            .getString(R.string.dms_inbox_shared_profile, username != null ? username : "", item.getProfile().getUsername());
                    break;
                case LOCATION:
                    subtitle = resources
                            .getString(R.string.dms_inbox_shared_location, username != null ? username : "", item.getLocation().getName());
                    break;
                case MEDIA: {
                    final MediaItemType mediaType = item.getMedia().getMediaType();
                    subtitle = getMediaSpecificSubtitle(username, resources, mediaType);
                    break;
                }
                case STORY_SHARE: {
                    final String reelType = item.getStoryShare().getReelType();
                    if (reelType == null) {
                        subtitle = item.getStoryShare().getTitle();
                    } else {
                        final int format = reelType.equals("highlight_reel")
                                           ? R.string.dms_inbox_shared_highlight
                                           : R.string.dms_inbox_shared_story;
                        final Media media = item.getStoryShare().getMedia();
                        User storyShareMediaUser = null;
                        if (media != null) {
                            storyShareMediaUser = media.getUser();
                        }
                        subtitle = resources.getString(format,
                                                       username != null ? username : "",
                                                       storyShareMediaUser == null ? "" : storyShareMediaUser.getUsername());
                    }
                    break;
                }
                case VOICE_MEDIA:
                    subtitle = resources.getString(R.string.dms_inbox_shared_voice, username != null ? username : "");
                    break;
                case ACTION_LOG:
                    subtitle = item.getActionLog().getDescription();
                    break;
                case VIDEO_CALL_EVENT:
                    subtitle = item.getVideoCallEvent().getDescription();
                    break;
                case CLIP:
                    final Media clip = item.getClip().getClip();
                    User clipUser = null;
                    if (clip != null) {
                        clipUser = clip.getUser();
                    }
                    subtitle = resources.getString(R.string.dms_inbox_shared_clip,
                                                   username != null ? username : "",
                                                   clipUser == null ? "" : clipUser.getUsername());
                    break;
                case FELIX_SHARE:
                    final Media video = item.getFelixShare().getVideo();
                    User felixShareVideoUser = null;
                    if (video != null) {
                        felixShareVideoUser = video.getUser();
                    }
                    subtitle = resources.getString(R.string.dms_inbox_shared_igtv,
                                                   username != null ? username : "",
                                                   felixShareVideoUser == null ? "" : felixShareVideoUser.getUsername());
                    break;
                case RAVEN_MEDIA:
                    subtitle = getRavenMediaSubtitle(item, resources, username);
                    break;
                case REEL_SHARE:
                    final DirectItemReelShare reelShare = item.getReelShare();
                    if (reelShare == null) {
                        subtitle = "";
                        break;
                    }
                    final String reelType = reelShare.getType();
                    switch (reelType) {
                        case "reply":
                            if (viewerId == item.getUserId()) {
                                subtitle = resources.getString(R.string.dms_inbox_replied_story_outgoing, reelShare.getText());
                            } else {
                                subtitle = resources
                                        .getString(R.string.dms_inbox_replied_story_incoming, username != null ? username : "", reelShare.getText());
                            }
                            break;
                        case "mention":
                            if (viewerId == item.getUserId()) {
                                // You mentioned the other person
                                final long mentionedUserId = item.getReelShare().getMentionedUserId();
                                final String otherUsername = getUsername(thread.getUsers(), mentionedUserId, viewerId, resources);
                                subtitle = resources.getString(R.string.dms_inbox_mentioned_story_outgoing, otherUsername);
                            } else {
                                // They mentioned you
                                subtitle = resources.getString(R.string.dms_inbox_mentioned_story_incoming, username != null ? username : "");
                            }
                            break;
                        case "reaction":
                            if (viewerId == item.getUserId()) {
                                subtitle = resources.getString(R.string.dms_inbox_reacted_story_outgoing, reelShare.getText());
                            } else {
                                subtitle = resources
                                        .getString(R.string.dms_inbox_reacted_story_incoming, username != null ? username : "", reelShare.getText());
                            }
                            break;
                        default:
                            subtitle = "";
                            break;
                    }
                    break;
                case XMA:
                    subtitle = resources.getString(R.string.dms_inbox_shared_sticker, username != null ? username : "");
                    break;
                default:
                    message = resources.getString(R.string.dms_inbox_raven_message_unknown);
            }
        }
        if (subtitle == null) {
            if (thread.isGroup() || (!thread.isGroup() && senderId == viewerId)) {
                subtitle = String.format("%s: %s", username != null ? username : "", message);
            } else {
                subtitle = message;
            }
        }
        return subtitle;
    }

    public static String getUsername(final List<User> users,
                                     final long userId,
                                     final long viewerId,
                                     final Resources resources) {
        if (userId == viewerId) {
            return resources.getString(R.string.you);
        }
        final Optional<User> senderOptional = users.stream()
                                                   .filter(Objects::nonNull)
                                                   .filter(user -> user.getPk() == userId)
                                                   .findFirst();
        return senderOptional.map(user -> {
            // return full name for fb users
            final String username = user.getUsername();
            if (TextUtils.isEmpty(username)) {
                return user.getFullName();
            }
            return username;
        }).orElse(null);
    }

    public static String getMediaSpecificSubtitle(final String username, final Resources resources, final MediaItemType mediaType) {
        final String userSharedAnImage = resources.getString(R.string.dms_inbox_shared_image, username != null ? username : "");
        final String userSharedAVideo = resources.getString(R.string.dms_inbox_shared_video, username != null ? username : "");
        final String userSentAMessage = resources.getString(R.string.dms_inbox_shared_message, username != null ? username : "");
        String subtitle;
        switch (mediaType) {
            case MEDIA_TYPE_IMAGE:
                subtitle = userSharedAnImage;
                break;
            case MEDIA_TYPE_VIDEO:
                subtitle = userSharedAVideo;
                break;
            default:
                subtitle = userSentAMessage;
                break;
        }
        return subtitle;
    }

    private static String getRavenMediaSubtitle(final DirectItem item,
                                                final Resources resources,
                                                final String username) {
        String subtitle = "↗ ";
        final DirectItemVisualMedia visualMedia = item.getVisualMedia();
        final RavenExpiringMediaActionSummary summary = visualMedia.getExpiringMediaActionSummary();
        if (summary != null) {
            final RavenExpiringMediaActionSummary.ActionType expiringMediaType = summary.getType();
            int textRes = 0;
            switch (expiringMediaType) {
                case DELIVERED:
                    textRes = R.string.dms_inbox_raven_media_delivered;
                    break;
                case SENT:
                    textRes = R.string.dms_inbox_raven_media_sent;
                    break;
                case OPENED:
                    textRes = R.string.dms_inbox_raven_media_opened;
                    break;
                case REPLAYED:
                    textRes = R.string.dms_inbox_raven_media_replayed;
                    break;
                case SENDING:
                    textRes = R.string.dms_inbox_raven_media_sending;
                    break;
                case BLOCKED:
                    textRes = R.string.dms_inbox_raven_media_blocked;
                    break;
                case SUGGESTED:
                    textRes = R.string.dms_inbox_raven_media_suggested;
                    break;
                case SCREENSHOT:
                    textRes = R.string.dms_inbox_raven_media_screenshot;
                    break;
                case CANNOT_DELIVER:
                    textRes = R.string.dms_inbox_raven_media_cant_deliver;
                    break;
            }
            if (textRes > 0) {
                subtitle += resources.getString(textRes);
            }
            return subtitle;
        }
        final MediaItemType mediaType = visualMedia.getMedia().getMediaType();
        subtitle = getMediaSpecificSubtitle(username, resources, mediaType);
        return subtitle;
    }
}
