package awais.instagrabber.utils;

import android.net.Uri;
import android.util.Log;

import androidx.annotation.Nullable;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.models.StoryModel;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.models.stickers.PollModel;
import awais.instagrabber.models.stickers.QuestionModel;
import awais.instagrabber.models.stickers.QuizModel;
import awais.instagrabber.models.stickers.SliderModel;
import awais.instagrabber.models.stickers.SwipeUpModel;
import awais.instagrabber.repositories.responses.Caption;
import awais.instagrabber.repositories.responses.FriendshipStatus;
import awais.instagrabber.repositories.responses.ImageVersions2;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.MediaCandidate;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.VideoVersion;
//import awaisomereport.LogCollector;

public final class ResponseBodyUtils {
    private static final String TAG = "ResponseBodyUtils";

    // isI: true if the content was requested from i.instagram.com instead of graphql
    @Nullable
    public static String getHighQualityPost(final JSONArray resources, final boolean isVideo, final boolean isI, final boolean low) {
        try {
            final int resourcesLen = resources.length();

            final String[] sources = new String[resourcesLen];
            int lastResMain = low ? 1000000 : 0, lastIndexMain = -1;
            int lastResBase = low ? 1000000 : 0, lastIndexBase = -1;
            for (int i = 0; i < resourcesLen; ++i) {
                final JSONObject item = resources.getJSONObject(i);
                if (item != null && (!isVideo || item.has(Constants.EXTRAS_PROFILE) || isI)) {
                    sources[i] = item.getString(isI ? "url" : "src");
                    final int currRes = item.getInt(isI ? "width" : "config_width") * item.getInt(isI ? "height" : "config_height");

                    final String profile = isVideo ? item.optString(Constants.EXTRAS_PROFILE) : null;

                    if (!isVideo || "MAIN".equals(profile)) {
                        if (currRes > lastResMain && !low) {
                            lastResMain = currRes;
                            lastIndexMain = i;
                        } else if (currRes < lastResMain && low) {
                            lastResMain = currRes;
                            lastIndexMain = i;
                        }
                    } else {
                        if (currRes > lastResBase && !low) {
                            lastResBase = currRes;
                            lastIndexBase = i;
                        } else if (currRes < lastResBase && low) {
                            lastResBase = currRes;
                            lastIndexBase = i;
                        }
                    }
                }
            }

            if (lastIndexMain >= 0) return sources[lastIndexMain];
            else if (lastIndexBase >= 0) return sources[lastIndexBase];
        } catch (final Exception e) {
//            if (Utils.logCollector != null)
//                Utils.logCollector.appendException(e, LogCollector.LogFile.UTILS, "getHighQualityPost",
//                                                   new Pair<>("resourcesNull", resources == null),
//                                                   new Pair<>("isVideo", isVideo));
            if (BuildConfig.DEBUG) Log.e("AWAISKING_APP", "", e);
        }
        return null;
    }

    public static String getHighQualityImage(final JSONObject resources) {
        String src = null;
        try {
            if (resources.has("display_resources"))
                src = getHighQualityPost(resources.getJSONArray("display_resources"), false, false, false);
            else if (resources.has("image_versions2"))
                src = getHighQualityPost(resources.getJSONObject("image_versions2").getJSONArray("candidates"), false, true, false);
            if (src == null) return resources.getString("display_url");
        } catch (final Exception e) {
//            if (Utils.logCollector != null)
//                Utils.logCollector.appendException(e, LogCollector.LogFile.UTILS, "getHighQualityImage",
//                                                   new Pair<>("resourcesNull", resources == null));
            if (BuildConfig.DEBUG) Log.e("AWAISKING_APP", "", e);
        }
        return src;
    }

    // public static String getLowQualityImage(final JSONObject resources) {
    //     String src = null;
    //     try {
    //         src = getHighQualityPost(resources.getJSONObject("image_versions2").getJSONArray("candidates"), false, true, true);
    //     } catch (final Exception e) {
    //         if (Utils.logCollector != null)
    //             Utils.logCollector.appendException(e, LogCollector.LogFile.UTILS, "getLowQualityImage",
    //                                                new Pair<>("resourcesNull", resources == null));
    //         if (BuildConfig.DEBUG) Log.e(TAG, "Error in getLowQualityImage", e);
    //     }
    //     return src;
    // }

    // public static ThumbnailDetails getItemThumbnail(@NonNull final JSONArray jsonArray) {
    //     final ThumbnailDetails thumbnailDetails = new ThumbnailDetails();
    //     final int imageResLen = jsonArray.length();
    //     for (int i = 0; i < imageResLen; ++i) {
    //         final JSONObject imageResource = jsonArray.optJSONObject(i);
    //         try {
    //             // final float ratio = (float) height / width;
    //             // if (ratio >= 0.95f && ratio <= 1.0f) {
    //             thumbnailDetails.height = imageResource.getInt("height");
    //             thumbnailDetails.width = imageResource.getInt("width");
    //             thumbnailDetails.url = imageResource.getString("url");
    //             break;
    //             // }
    //         } catch (final Exception e) {
    //             if (Utils.logCollector != null)
    //                 Utils.logCollector.appendException(e, LogCollector.LogFile.UTILS, "getItemThumbnail");
    //             if (BuildConfig.DEBUG) Log.e(TAG, "", e);
    //         }
    //     }
    //     // if (TextUtils.isEmpty(thumbnail)) thumbnail = jsonArray.optJSONObject(0).optString("url");
    //     return thumbnailDetails;
    // }

    // public static class ThumbnailDetails {
    //     int width;
    //     int height;
    //     public String url;
    // }

    // @Nullable
    // public static ThumbnailDetails getThumbnailUrl(@NonNull final JSONObject mediaObj, final MediaItemType mediaType) throws Exception {
    //     ThumbnailDetails thumbnail = null;
    //     if (mediaType == MediaItemType.MEDIA_TYPE_IMAGE || mediaType == MediaItemType.MEDIA_TYPE_VIDEO) {
    //         final JSONObject imageVersions = mediaObj.optJSONObject("image_versions2");
    //         if (imageVersions != null)
    //             thumbnail = getItemThumbnail(imageVersions.getJSONArray("candidates"));
    //
    //     } else if (mediaType == MediaItemType.MEDIA_TYPE_SLIDER) {
    //         final JSONArray carouselMedia = mediaObj.optJSONArray("carousel_media");
    //         if (carouselMedia != null)
    //             thumbnail = getItemThumbnail(carouselMedia.getJSONObject(0)
    //                                                       .getJSONObject("image_versions2")
    //                                                       .getJSONArray("candidates"));
    //     }
    //     return thumbnail;
    // }

    // public static String getVideoUrl(@NonNull final JSONObject mediaObj) {
    //     String url = null;
    //     final JSONArray videoVersions = mediaObj.optJSONArray("video_versions");
    //     if (videoVersions == null) {
    //         return null;
    //     }
    //     int largestWidth = 0;
    //     for (int i = 0; i < videoVersions.length(); i++) {
    //         final JSONObject videoVersionJson = videoVersions.optJSONObject(i);
    //         if (videoVersionJson == null) {
    //             continue;
    //         }
    //         final int width = videoVersionJson.optInt("width");
    //         if (largestWidth == 0 || width > largestWidth) {
    //             largestWidth = width;
    //             url = videoVersionJson.optString("url");
    //         }
    //     }
    //     return url;
    // }

    // @Nullable
    // public static MediaItemType getMediaItemType(final int mediaType) {
    //     if (mediaType == 1) return MediaItemType.MEDIA_TYPE_IMAGE;
    //     if (mediaType == 2) return MediaItemType.MEDIA_TYPE_VIDEO;
    //     if (mediaType == 8) return MediaItemType.MEDIA_TYPE_SLIDER;
    //     if (mediaType == 11) return MediaItemType.MEDIA_TYPE_VOICE;
    //     return null;
    // }

    // public static DirectItemModel.DirectItemMediaModel getDirectMediaModel(final JSONObject mediaObj) throws Exception {
    //     final DirectItemModel.DirectItemMediaModel mediaModel;
    //     if (mediaObj == null) mediaModel = null;
    //     else {
    //         final JSONObject userObj = mediaObj.optJSONObject("user");
    //
    //         ProfileModel user = null;
    //         if (userObj != null) {
    //             user = new ProfileModel(
    //                     userObj.getBoolean("is_private"),
    //                     false,
    //                     userObj.optBoolean("is_verified"),
    //                     String.valueOf(userObj.get("pk")),
    //                     userObj.getString("username"),
    //                     userObj.getString("full_name"),
    //                     null, null,
    //                     userObj.getString("profile_pic_url"),
    //                     null, 0, 0, 0, false, false, false, false, false);
    //         }
    //
    //         final MediaItemType mediaType = getMediaItemType(mediaObj.optInt("media_type", -1));
    //
    //         String id = mediaObj.optString("id");
    //         if (TextUtils.isEmpty(id)) id = null;
    //
    //         final ThumbnailDetails thumbnailDetails = getThumbnailUrl(mediaObj, mediaType);
    //         mediaModel = new DirectItemModel.DirectItemMediaModel(
    //                 mediaType,
    //                 mediaObj.optLong("expiring_at"),
    //                 mediaObj.optLong("pk"),
    //                 id,
    //                 thumbnailDetails != null ? thumbnailDetails.url : null,
    //                 mediaType == MediaItemType.MEDIA_TYPE_VIDEO ? getVideoUrl(mediaObj) : null,
    //                 user,
    //                 mediaObj.optString("code"),
    //                 thumbnailDetails != null ? thumbnailDetails.height : 0,
    //                 thumbnailDetails != null ? thumbnailDetails.width : 0);
    //     }
    //     return mediaModel;
    // }
    //
    // private static DirectItemType getDirectItemType(final String itemType) {
    //     if ("placeholder".equals(itemType)) return DirectItemType.PLACEHOLDER;
    //     if ("media".equals(itemType)) return DirectItemType.MEDIA;
    //     if ("link".equals(itemType)) return DirectItemType.LINK;
    //     if ("like".equals(itemType)) return DirectItemType.LIKE;
    //     if ("reel_share".equals(itemType)) return DirectItemType.REEL_SHARE;
    //     if ("media_share".equals(itemType)) return DirectItemType.MEDIA_SHARE;
    //     if ("action_log".equals(itemType)) return DirectItemType.ACTION_LOG;
    //     if ("raven_media".equals(itemType)) return DirectItemType.RAVEN_MEDIA;
    //     if ("profile".equals(itemType)) return DirectItemType.PROFILE;
    //     if ("video_call_event".equals(itemType)) return DirectItemType.VIDEO_CALL_EVENT;
    //     if ("animated_media".equals(itemType)) return DirectItemType.ANIMATED_MEDIA;
    //     if ("voice_media".equals(itemType)) return DirectItemType.VOICE_MEDIA;
    //     if ("story_share".equals(itemType)) return DirectItemType.STORY_SHARE;
    //     if ("clip".equals(itemType)) return DirectItemType.CLIP;
    //     if ("felix_share".equals(itemType)) return DirectItemType.FELIX_SHARE;
    //     return DirectItemType.TEXT;
    // }
    //
    // @NonNull
    // public static InboxThreadModel createInboxThreadModel(@NonNull final JSONObject data, final boolean inThreadView) throws Exception {
    //     final InboxReadState readState = data.getInt("read_state") == 0 ? InboxReadState.STATE_READ : InboxReadState.STATE_UNREAD;
    //     final String threadType = data.getString("thread_type"); // they're all "private", group is identified by boolean "is_group"
    //
    //     final String threadId = data.getString("thread_id");
    //     final String threadV2Id = data.getString("thread_v2_id");
    //     final String threadTitle = data.getString("thread_title");
    //
    //     final String threadNewestCursor = data.optString("newest_cursor");
    //     final String threadOldestCursor = data.optString("oldest_cursor");
    //     final String threadNextCursor = data.has("next_cursor") ? data.getString("next_cursor") : null;
    //     final String threadPrevCursor = data.has("prev_cursor") ? data.getString("prev_cursor") : null;
    //
    //     final boolean threadHasOlder = data.getBoolean("has_older");
    //     final long unreadCount = data.optLong("read_state", 0);
    //
    //     final long lastActivityAt = data.optLong("last_activity_at");
    //     final boolean named = data.optBoolean("named");
    //     final boolean muted = data.optBoolean("muted");
    //     final boolean isPin = data.optBoolean("is_pin");
    //     final boolean isSpam = data.optBoolean("is_spam");
    //     final boolean isGroup = data.optBoolean("is_group");
    //     final boolean pending = data.optBoolean("pending");
    //     final boolean archived = data.optBoolean("archived");
    //     final boolean canonical = data.optBoolean("canonical");
    //
    //     final JSONArray users = data.getJSONArray("users");
    //     final int usersLen = users.length();
    //     final JSONArray leftusers = data.optJSONArray("left_users");
    //     final int leftusersLen = leftusers == null ? 0 : leftusers.length();
    //     final JSONArray admins = data.getJSONArray("admin_user_ids");
    //     final int adminsLen = admins.length();
    //
    //     final ProfileModel[] userModels = new ProfileModel[usersLen];
    //     for (int j = 0; j < usersLen; ++j) {
    //         final JSONObject userObject = users.getJSONObject(j);
    //         userModels[j] = new ProfileModel(userObject.optBoolean("is_private"),
    //                                          false,
    //                                          userObject.optBoolean("is_verified"),
    //                                          String.valueOf(userObject.get("pk")),
    //                                          userObject.optString("username"), // optional cuz fb users
    //                                          userObject.getString("full_name"),
    //                                          null, null,
    //                                          userObject.getString("profile_pic_url"),
    //                                          null, 0, 0, 0, false, false, false, false, false);
    //     }
    //
    //     final ProfileModel[] leftuserModels = new ProfileModel[leftusersLen];
    //     for (int j = 0; j < leftusersLen; ++j) {
    //         final JSONObject userObject = leftusers.getJSONObject(j);
    //         leftuserModels[j] = new ProfileModel(userObject.getBoolean("is_private"),
    //                                              false,
    //                                              userObject.optBoolean("is_verified"),
    //                                              String.valueOf(userObject.get("pk")),
    //                                              userObject.getString("username"),
    //                                              userObject.getString("full_name"),
    //                                              null, null,
    //                                              userObject.getString("profile_pic_url"),
    //                                              null, 0, 0, 0, false, false, false, false, false);
    //     }
    //
    //     final Long[] adminIDs = new Long[adminsLen];
    //     for (int j = 0; j < adminsLen; ++j) {
    //         adminIDs[j] = admins.getLong(j);
    //     }
    //
    //     final JSONArray items = data.getJSONArray("items");
    //     final int itemsLen = items.length();
    //
    //     final ArrayList<DirectItemModel> itemModels = new ArrayList<>(itemsLen);
    //     for (int i = 0; i < itemsLen; ++i) {
    //         final JSONObject itemObject = items.getJSONObject(i);
    //
    //         CharSequence text = null;
    //         ProfileModel profileModel = null;
    //         DirectItemModel.DirectItemLinkModel linkModel = null;
    //         DirectItemModel.DirectItemMediaModel directMedia = null;
    //         DirectItemModel.DirectItemReelShareModel reelShareModel = null;
    //         DirectItemModel.DirectItemActionLogModel actionLogModel = null;
    //         DirectItemModel.DirectItemAnimatedMediaModel animatedMediaModel = null;
    //         DirectItemModel.DirectItemVoiceMediaModel voiceMediaModel = null;
    //         DirectItemModel.DirectItemRavenMediaModel ravenMediaModel = null;
    //         DirectItemModel.DirectItemVideoCallEventModel videoCallEventModel = null;
    //
    //         final DirectItemType itemType = getDirectItemType(itemObject.getString("item_type"));
    //         switch (itemType) {
    //             case ANIMATED_MEDIA: {
    //                 final JSONObject animatedMedia = itemObject.getJSONObject("animated_media");
    //                 final JSONObject stickerImage = animatedMedia.getJSONObject("images").getJSONObject("fixed_height");
    //
    //                 animatedMediaModel = new DirectItemModel.DirectItemAnimatedMediaModel(
    //                         animatedMedia.getBoolean("is_random"),
    //                         animatedMedia.getBoolean("is_sticker"),
    //                         animatedMedia.getString("id"),
    //                         stickerImage.getString("url"),
    //                         stickerImage.optString("webp"),
    //                         stickerImage.optString("mp4"),
    //                         stickerImage.getInt("height"),
    //                         stickerImage.getInt("width"));
    //             }
    //             break;
    //
    //             case VOICE_MEDIA: {
    //                 final JSONObject voiceMedia = itemObject.getJSONObject("voice_media").getJSONObject("media");
    //                 final JSONObject audio = voiceMedia.getJSONObject("audio");
    //
    //                 int[] waveformData = null;
    //                 final JSONArray waveformDataArray = audio.optJSONArray("waveform_data");
    //                 if (waveformDataArray != null) {
    //                     final int waveformDataLen = waveformDataArray.length();
    //                     waveformData = new int[waveformDataLen];
    //                     // 0.011775206
    //                     for (int j = 0; j < waveformDataLen; ++j) {
    //                         waveformData[j] = (int) (waveformDataArray.optDouble(j) * 10);
    //                     }
    //                 }
    //
    //                 voiceMediaModel = new DirectItemModel.DirectItemVoiceMediaModel(
    //                         voiceMedia.getString("id"),
    //                         audio.getString("audio_src"),
    //                         audio.getLong("duration"),
    //                         waveformData);
    //             }
    //             break;
    //
    //             case LINK: {
    //                 final JSONObject linkObj = itemObject.getJSONObject("link");
    //
    //                 DirectItemModel.DirectItemLinkContext itemLinkContext = null;
    //                 final JSONObject linkContext = linkObj.optJSONObject("link_context");
    //                 if (linkContext != null) {
    //                     itemLinkContext = new DirectItemModel.DirectItemLinkContext(
    //                             linkContext.getString("link_url"),
    //                             linkContext.optString("link_title"),
    //                             linkContext.optString("link_summary"),
    //                             linkContext.optString("link_image_url")
    //                     );
    //                 }
    //
    //                 linkModel = new DirectItemModel.DirectItemLinkModel(
    //                         linkObj.getString("text"),
    //                         linkObj.getString("client_context"),
    //                         linkObj.optString("mutation_token"),
    //                         itemLinkContext);
    //             }
    //             break;
    //
    //             case REEL_SHARE: {
    //                 final JSONObject reelShare = itemObject.getJSONObject("reel_share");
    //                 reelShareModel = new DirectItemModel.DirectItemReelShareModel(
    //                         reelShare.optBoolean("is_reel_persisted"),
    //                         reelShare.getLong("reel_owner_id"),
    //                         reelShare.getJSONObject("media").getJSONObject("user").getString("username"),
    //                         reelShare.getString("text"),
    //                         reelShare.getString("type"),
    //                         reelShare.getString("reel_type"),
    //                         reelShare.optString("reel_name"),
    //                         reelShare.optString("reel_id"),
    //                         getDirectMediaModel(reelShare.optJSONObject("media")));
    //             }
    //             break;
    //
    //             case RAVEN_MEDIA: {
    //                 final JSONObject visualMedia = itemObject.getJSONObject("visual_media");
    //
    //                 final JSONArray seenUserIdsArray = visualMedia.getJSONArray("seen_user_ids");
    //                 final int seenUsersLen = seenUserIdsArray.length();
    //                 final String[] seenUserIds = new String[seenUsersLen];
    //                 for (int j = 0; j < seenUsersLen; j++)
    //                     seenUserIds[j] = seenUserIdsArray.getString(j);
    //
    //                 DirectItemModel.RavenExpiringMediaActionSummaryModel expiringSummaryModel = null;
    //                 final JSONObject actionSummary = visualMedia.optJSONObject("expiring_media_action_summary");
    //                 if (actionSummary != null)
    //                     expiringSummaryModel = new DirectItemModel.RavenExpiringMediaActionSummaryModel(
    //                             actionSummary.getLong("timestamp"), actionSummary.getInt("count"),
    //                             getExpiringMediaType(actionSummary.getString("type")));
    //
    //                 final RavenMediaViewType viewType;
    //                 final String viewMode = visualMedia.getString("view_mode");
    //                 switch (viewMode) {
    //                     case "replayable":
    //                         viewType = RavenMediaViewType.REPLAYABLE;
    //                         break;
    //                     case "permanent":
    //                         viewType = RavenMediaViewType.PERMANENT;
    //                         break;
    //                     case "once":
    //                     default:
    //                         viewType = RavenMediaViewType.ONCE;
    //                 }
    //
    //                 ravenMediaModel = new DirectItemModel.DirectItemRavenMediaModel(
    //                         visualMedia.optLong(viewType == RavenMediaViewType.PERMANENT ? "url_expire_at_secs" : "replay_expiring_at_us"),
    //                         visualMedia.optInt("playback_duration_secs"),
    //                         visualMedia.getInt("seen_count"),
    //                         seenUserIds,
    //                         viewType,
    //                         getDirectMediaModel(visualMedia.optJSONObject("media")),
    //                         expiringSummaryModel);
    //
    //             }
    //             break;
    //
    //             case VIDEO_CALL_EVENT: {
    //                 final JSONObject videoCallEvent = itemObject.getJSONObject("video_call_event");
    //                 videoCallEventModel = new DirectItemModel.DirectItemVideoCallEventModel(videoCallEvent.optLong("vc_id"),
    //                                                                                         videoCallEvent.optBoolean("thread_has_audio_only_call"),
    //                                                                                         videoCallEvent.getString("action"),
    //                                                                                         videoCallEvent.getString("description"));
    //             }
    //             break;
    //
    //             case PROFILE: {
    //                 final JSONObject profile = itemObject.getJSONObject("profile");
    //                 profileModel = new ProfileModel(profile.getBoolean("is_private"),
    //                                                 false,
    //                                                 profile.getBoolean("is_verified"),
    //                                                 Long.toString(profile.getLong("pk")),
    //                                                 profile.getString("username"),
    //                                                 profile.getString("full_name"),
    //                                                 null, null,
    //                                                 profile.getString("profile_pic_url"),
    //                                                 null, 0, 0, 0, false, false, false, false, false);
    //             }
    //             break;
    //
    //             case PLACEHOLDER:
    //                 final JSONObject placeholder = itemObject.getJSONObject("placeholder");
    //                 text = placeholder.getString("title") + "<br><small>" + placeholder.getString("message") + "</small>";
    //                 break;
    //
    //             case ACTION_LOG:
    //                 if (inThreadView && itemObject.optInt("hide_in_thread", 0) != 0)
    //                     continue;
    //                 final JSONObject actionLog = itemObject.getJSONObject("action_log");
    //                 String desc = actionLog.getString("description");
    //                 JSONArray bold = actionLog.getJSONArray("bold");
    //                 for (int q = 0; q < bold.length(); ++q) {
    //                     JSONObject boldItem = bold.getJSONObject(q);
    //                     desc = desc.substring(0, boldItem.getInt("start") + q * 7) + "<b>"
    //                             + desc.substring(boldItem.getInt("start") + q * 7, boldItem.getInt("end") + q * 7)
    //                             + "</b>" + desc.substring(boldItem.getInt("end") + q * 7);
    //                 }
    //                 actionLogModel = new DirectItemModel.DirectItemActionLogModel(desc);
    //                 break;
    //
    //             case MEDIA_SHARE:
    //                 directMedia = getDirectMediaModel(itemObject.getJSONObject("media_share"));
    //                 break;
    //
    //             case CLIP:
    //                 directMedia = getDirectMediaModel(itemObject.getJSONObject("clip").getJSONObject("clip"));
    //                 break;
    //
    //             case FELIX_SHARE:
    //                 directMedia = getDirectMediaModel(itemObject.getJSONObject("felix_share").getJSONObject("video"));
    //                 break;
    //
    //             case MEDIA:
    //                 directMedia = getDirectMediaModel(itemObject.optJSONObject("media"));
    //                 break;
    //
    //             case LIKE:
    //                 text = itemObject.getString("like");
    //                 break;
    //
    //             case STORY_SHARE:
    //                 final JSONObject storyShare = itemObject.getJSONObject("story_share");
    //                 if (!storyShare.has("media"))
    //                     text = "<small>" + storyShare.optString("message") + "</small>";
    //                 else {
    //                     reelShareModel = new DirectItemModel.DirectItemReelShareModel(
    //                             storyShare.optBoolean("is_reel_persisted"),
    //                             storyShare.getJSONObject("media").getJSONObject("user").getLong("pk"),
    //                             storyShare.getJSONObject("media").getJSONObject("user").getString("username"),
    //                             storyShare.getString("text"),
    //                             storyShare.getString("story_share_type"),
    //                             storyShare.getString("reel_type"),
    //                             storyShare.optString("reel_name"),
    //                             storyShare.optString("reel_id"),
    //                             getDirectMediaModel(storyShare.optJSONObject("media")));
    //                 }
    //                 break;
    //
    //             case TEXT:
    //                 if (!itemObject.has("text"))
    //                     Log.d("AWAISKING_APP", "itemObject: " + itemObject); // todo
    //                 text = itemObject.optString("text");
    //                 break;
    //         }
    //
    //         String[] liked = null;
    //         if (!itemObject.isNull("reactions") && !itemObject.getJSONObject("reactions").isNull("likes")) {
    //             JSONArray rawLiked = itemObject.getJSONObject("reactions").getJSONArray("likes");
    //             liked = new String[rawLiked.length()];
    //             for (int l = 0; l < rawLiked.length(); ++l) {
    //                 liked[l] = String.valueOf(rawLiked.getJSONObject(l).getLong("sender_id"));
    //             }
    //         }
    //
    //         itemModels.add(new DirectItemModel(
    //                 itemObject.getLong("user_id"),
    //                 itemObject.getLong("timestamp"),
    //                 itemObject.getString("item_id"),
    //                 liked,
    //                 itemType,
    //                 text,
    //                 linkModel,
    //                 profileModel,
    //                 reelShareModel,
    //                 directMedia,
    //                 actionLogModel,
    //                 voiceMediaModel,
    //                 ravenMediaModel,
    //                 videoCallEventModel,
    //                 animatedMediaModel));
    //     }
    //
    //     itemModels.trimToSize();
    //
    //     return new InboxThreadModel(readState, threadId, threadV2Id, threadType, threadTitle,
    //                                 threadNewestCursor, threadOldestCursor, threadNextCursor, threadPrevCursor,
    //                                 null, // todo
    //                                 userModels, leftuserModels, adminIDs,
    //                                 itemModels.toArray(new DirectItemModel[0]),
    //                                 muted, isPin, named, canonical,
    //                                 pending, threadHasOlder, unreadCount, isSpam, isGroup, archived, lastActivityAt);
    // }
    //
    // private static RavenExpiringMediaType getExpiringMediaType(final String type) {
    //     if ("raven_sent".equals(type)) return RavenExpiringMediaType.RAVEN_SENT;
    //     if ("raven_opened".equals(type)) return RavenExpiringMediaType.RAVEN_OPENED;
    //     if ("raven_blocked".equals(type)) return RavenExpiringMediaType.RAVEN_BLOCKED;
    //     if ("raven_sending".equals(type)) return RavenExpiringMediaType.RAVEN_SENDING;
    //     if ("raven_replayed".equals(type)) return RavenExpiringMediaType.RAVEN_REPLAYED;
    //     if ("raven_delivered".equals(type)) return RavenExpiringMediaType.RAVEN_DELIVERED;
    //     if ("raven_suggested".equals(type)) return RavenExpiringMediaType.RAVEN_SUGGESTED;
    //     if ("raven_screenshot".equals(type)) return RavenExpiringMediaType.RAVEN_SCREENSHOT;
    //     if ("raven_cannot_deliver".equals(type)) return RavenExpiringMediaType.RAVEN_CANNOT_DELIVER;
    //     //if ("raven_unknown".equals(type)) [default?]
    //     return RavenExpiringMediaType.RAVEN_UNKNOWN;
    // }

    // public static FeedModel parseItem(final JSONObject itemJson) throws JSONException {
    //     if (itemJson == null) {
    //         return null;
    //     }
    //     ProfileModel profileModel = null;
    //     if (itemJson.has("user")) {
    //         final JSONObject user = itemJson.getJSONObject("user");
    //         final JSONObject friendshipStatus = user.optJSONObject("friendship_status");
    //         boolean following = false;
    //         boolean restricted = false;
    //         boolean requested = false;
    //         if (friendshipStatus != null) {
    //             following = friendshipStatus.optBoolean("following");
    //             requested = friendshipStatus.optBoolean("outgoing_request");
    //             restricted = friendshipStatus.optBoolean("is_restricted");
    //         }
    //         profileModel = new ProfileModel(
    //                 user.optBoolean("is_private"),
    //                 false, // if you can see it then you def follow
    //                 user.optBoolean("is_verified"),
    //                 user.getString("pk"),
    //                 user.getString(Constants.EXTRAS_USERNAME),
    //                 user.optString("full_name"),
    //                 null,
    //                 null,
    //                 user.getString("profile_pic_url"),
    //                 null,
    //                 0,
    //                 0,
    //                 0,
    //                 following,
    //                 false,
    //                 restricted,
    //                 false,
    //                 requested);
    //     }
    //     final JSONObject captionJson = itemJson.optJSONObject("caption");
    //     final JSONObject locationJson = itemJson.optJSONObject("location");
    //     final MediaItemType mediaType = ResponseBodyUtils.getMediaItemType(itemJson.optInt("media_type"));
    //     if (mediaType == null) {
    //         return null;
    //     }
    //     final FeedModel.Builder feedModelBuilder = new FeedModel.Builder()
    //             .setItemType(mediaType)
    //             .setProfileModel(profileModel)
    //             .setPostId(itemJson.getString(Constants.EXTRAS_ID))
    //             .setThumbnailUrl(mediaType != MediaItemType.MEDIA_TYPE_SLIDER ? ResponseBodyUtils.getLowQualityImage(itemJson) : null)
    //             .setShortCode(itemJson.getString("code"))
    //             .setPostCaption(captionJson != null ? captionJson.optString("text") : "")
    //             .setCaptionId(captionJson != null ? captionJson.optString("pk") : null)
    //             .setCommentsCount(itemJson.optInt("comment_count"))
    //             .setTimestamp(itemJson.optLong("taken_at", -1))
    //             .setLiked(itemJson.optBoolean("has_liked"))
    //             .setBookmarked(itemJson.optBoolean("has_viewer_saved"))
    //             .setLikesCount(itemJson.optInt("like_count"))
    //             .setLocationName(locationJson != null ? locationJson.optString("name") : null)
    //             .setLocationId(locationJson != null ? String.valueOf(locationJson.optLong("pk")) : null)
    //             .setImageHeight(itemJson.optInt("original_height"))
    //             .setImageWidth(itemJson.optInt("original_width"));
    //     switch (mediaType) {
    //         case MEDIA_TYPE_VIDEO:
    //             final long videoViews = itemJson.optLong("view_count", 0);
    //             feedModelBuilder.setViewCount(videoViews)
    //                             .setDisplayUrl(ResponseBodyUtils.getVideoUrl(itemJson));
    //             break;
    //         case MEDIA_TYPE_IMAGE:
    //             feedModelBuilder.setDisplayUrl(ResponseBodyUtils.getHighQualityImage(itemJson));
    //             break;
    //         case MEDIA_TYPE_SLIDER:
    //             final List<PostChild> childPosts = getChildPosts(itemJson);
    //             feedModelBuilder.setSliderItems(childPosts)
    //                             .setImageHeight(childPosts.get(0).getHeight())
    //                             .setImageWidth(childPosts.get(0).getWidth());
    //             break;
    //     }
    //     return feedModelBuilder.build();
    // }

    // the "user" argument can be null, it's used because instagram redacts user details from responses
    public static Media parseGraphQLItem(final JSONObject itemJson, final User backup) throws JSONException {
        if (itemJson == null) {
            return null;
        }
        final JSONObject feedItem = itemJson.has("node") ? itemJson.getJSONObject("node") : itemJson;
        final String mediaType = feedItem.optString("__typename");
        if ("GraphSuggestedUserFeedUnit".equals(mediaType)) return null;

        final boolean isVideo = feedItem.optBoolean("is_video");
        final long videoViews = feedItem.optLong("video_view_count", 0);

        final String displayUrl = feedItem.optString("display_url");
        if (TextUtils.isEmpty(displayUrl)) return null;
        final String resourceUrl;
        if (isVideo && feedItem.has("video_url")) {
            resourceUrl = feedItem.getString("video_url");
        } else {
            resourceUrl = feedItem.has("display_resources") ? ResponseBodyUtils.getHighQualityImage(feedItem) : displayUrl;
        }
        JSONObject tempJsonObject = feedItem.optJSONObject("edge_media_preview_comment");
        final long commentsCount = tempJsonObject != null ? tempJsonObject.optLong("count") : 0;
        tempJsonObject = feedItem.optJSONObject("edge_media_preview_like");
        final long likesCount = tempJsonObject != null ? tempJsonObject.optLong("count") : 0;
        tempJsonObject = feedItem.optJSONObject("edge_media_to_caption");
        final JSONArray captions = tempJsonObject != null ? tempJsonObject.getJSONArray("edges") : null;
        String captionText = null;
        if (captions != null && captions.length() > 0) {
            if ((tempJsonObject = captions.optJSONObject(0)) != null &&
                    (tempJsonObject = tempJsonObject.optJSONObject("node")) != null) {
                captionText = tempJsonObject.getString("text");
            }
        }
        final JSONObject locationJson = feedItem.optJSONObject("location");
        // Log.d(TAG, "location: " + (location == null ? null : location.toString()));
        long locationId = 0;
        String locationName = null;
        if (locationJson != null) {
            locationName = locationJson.optString("name");
            if (locationJson.has("id")) {
                locationId = locationJson.optLong("id");
            } else if (locationJson.has("pk")) {
                locationId = locationJson.optLong("pk");
            }
            // Log.d(TAG, "locationId: " + locationId);
        }
        int height = 0;
        int width = 0;
        final JSONObject dimensions = feedItem.optJSONObject("dimensions");
        if (dimensions != null) {
            height = dimensions.optInt("height");
            width = dimensions.optInt("width");
        }
        String thumbnailUrl = null;
        final List<MediaCandidate> candidates = new ArrayList<MediaCandidate>();
        if (feedItem.has("display_resources") || feedItem.has("thumbnail_resources")) {
            final JSONArray displayResources = feedItem.has("display_resources")
                    ? feedItem.getJSONArray("display_resources")
                    : feedItem.getJSONArray("thumbnail_resources");
            for (int i = 0; i < displayResources.length(); i++) {
                final JSONObject displayResource = displayResources.getJSONObject(i);
                candidates.add(new MediaCandidate(
                        displayResource.getInt("config_width"),
                        displayResource.getInt("config_height"),
                        displayResource.getString("src")
                ));
            }
        }
        final ImageVersions2 imageVersions2 = new ImageVersions2(candidates);

        User user = backup;
        long userId = -1;
        if (feedItem.has("owner") && user == null) {
            final JSONObject owner = feedItem.getJSONObject("owner");
            final FriendshipStatus friendshipStatus = new FriendshipStatus(
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
            userId = owner.optLong(Constants.EXTRAS_ID, -1);
            user = new User(
                    userId,
                    owner.optString(Constants.EXTRAS_USERNAME),
                    owner.optString("full_name"),
                    false,
                    owner.optString("profile_pic_url"),
                    owner.optBoolean("is_verified"));
        }
        final String id = feedItem.getString(Constants.EXTRAS_ID);
        VideoVersion videoVersion = null;
        if (isVideo) {
            videoVersion = new VideoVersion(
                    null,
                    null,
                    width,
                    height,
                    resourceUrl
            );
        }
        final Caption caption = new Caption(
                userId,
                captionText
        );

        final boolean isSlider = "GraphSidecar".equals(mediaType) && feedItem.has("edge_sidecar_to_children");
        List<Media> childItems = null;
        if (isSlider) {
            childItems = new ArrayList<>();
            // feedModelBuilder.setItemType(MediaItemType.MEDIA_TYPE_SLIDER);
            final JSONObject sidecar = feedItem.optJSONObject("edge_sidecar_to_children");
            if (sidecar != null) {
                final JSONArray children = sidecar.optJSONArray("edges");
                if (children != null) {
                    // final List<PostChild> sliderItems = getSliderItems(children);
                    // feedModelBuilder.setSliderItems(sliderItems)
                    //                 .setImageHeight(sliderItems.get(0).getHeight())
                    //                 .setImageWidth(sliderItems.get(0).getWidth());
                    for (int i = 0; i < children.length(); i++) {
                        final JSONObject child = children.optJSONObject(i);
                        if (child == null) continue;
                        final Media media = parseGraphQLItem(child, null);
                        media.setIsSidecarChild(true);
                        childItems.add(media);
                    }
                }
            }
        }
        MediaItemType mediaItemType = MediaItemType.MEDIA_TYPE_IMAGE;
        if (isSlider) {
            mediaItemType = MediaItemType.MEDIA_TYPE_SLIDER;
        } else if (isVideo) {
            mediaItemType = MediaItemType.MEDIA_TYPE_VIDEO;
        }
        final Location location = new Location(
                locationId,
                locationName,
                locationName,
                null,
                null,
                -1,
                -1
        );
        return new Media(
                id,
                id,
                feedItem.optString(Constants.EXTRAS_SHORTCODE),
                feedItem.optLong("taken_at_timestamp", -1),
                user,
                false,
                imageVersions2,
                width,
                height,
                mediaItemType,
                false,
                feedItem.optBoolean("comments_disabled"),
                -1,
                commentsCount,
                likesCount,
                false,
                false,
                isVideo ? Collections.singletonList(videoVersion) : null,
                feedItem.optBoolean("has_audio"),
                feedItem.optDouble("video_duration"),
                videoViews,
                caption,
                false,
                null,
                null,
                childItems,
                location,
                null,
                false,
                false,
                null,
                null
        );
    }

    // private static List<PostChild> getChildPosts(final JSONObject mediaJson) throws JSONException {
    //     if (mediaJson == null) {
    //         return Collections.emptyList();
    //     }
    //     final JSONArray carouselMedia = mediaJson.optJSONArray("carousel_media");
    //     if (carouselMedia == null) {
    //         return Collections.emptyList();
    //     }
    //     final List<PostChild> children = new ArrayList<>();
    //     for (int i = 0; i < carouselMedia.length(); i++) {
    //         final JSONObject childJson = carouselMedia.optJSONObject(i);
    //         final PostChild childPost = getChildPost(childJson);
    //         if (childPost != null) {
    //             children.add(childPost);
    //         }
    //     }
    //     return children;
    // }

    // private static PostChild getChildPost(final JSONObject childJson) throws JSONException {
    //     if (childJson == null) {
    //         return null;
    //     }
    //     final MediaItemType mediaType = ResponseBodyUtils.getMediaItemType(childJson.optInt("media_type"));
    //     if (mediaType == null) {
    //         return null;
    //     }
    //     final PostChild.Builder builder = new PostChild.Builder();
    //     switch (mediaType) {
    //         case MEDIA_TYPE_VIDEO:
    //             builder.setDisplayUrl(ResponseBodyUtils.getVideoUrl(childJson));
    //             break;
    //         case MEDIA_TYPE_IMAGE:
    //             builder.setDisplayUrl(ResponseBodyUtils.getHighQualityImage(childJson));
    //             break;
    //     }
    //     return builder.setItemType(mediaType)
    //                   .setPostId(childJson.getString("id"))
    //                   .setThumbnailUrl(ResponseBodyUtils.getLowQualityImage(childJson))
    //                   .setHeight(childJson.optInt("original_height"))
    //                   .setWidth(childJson.optInt("original_width"))
    //                   .build();
    // }

    // this is for graphql
    // @NonNull
    // private static List<PostChild> getSliderItems(final JSONArray children) throws JSONException {
    //     final List<PostChild> sliderItems = new ArrayList<>();
    //     for (int j = 0; j < children.length(); ++j) {
    //         final JSONObject childNode = children.optJSONObject(j).getJSONObject("node");
    //         final boolean isChildVideo = childNode.optBoolean("is_video");
    //         int height = 0;
    //         int width = 0;
    //         final JSONObject dimensions = childNode.optJSONObject("dimensions");
    //         if (dimensions != null) {
    //             height = dimensions.optInt("height");
    //             width = dimensions.optInt("width");
    //         }
    //         String thumbnailUrl = null;
    //         try {
    //             thumbnailUrl = childNode.getJSONArray("display_resources")
    //                                     .getJSONObject(0)
    //                                     .getString("src");
    //         } catch (JSONException ignored) {}
    //         final PostChild sliderItem = new PostChild.Builder()
    //                 .setItemType(isChildVideo ? MediaItemType.MEDIA_TYPE_VIDEO
    //                                           : MediaItemType.MEDIA_TYPE_IMAGE)
    //                 .setPostId(childNode.getString(Constants.EXTRAS_ID))
    //                 .setDisplayUrl(isChildVideo ? childNode.getString("video_url")
    //                                             : childNode.getString("display_url"))
    //                 .setThumbnailUrl(thumbnailUrl != null ? thumbnailUrl
    //                                                       : childNode.getString("display_url"))
    //                 .setVideoViews(childNode.optLong("video_view_count", 0))
    //                 .setHeight(height)
    //                 .setWidth(width)
    //                 .build();
    //         // Log.d(TAG, "getSliderItems: sliderItem: " + sliderItem);
    //         sliderItems.add(sliderItem);
    //     }
    //     return sliderItems;
    // }

    public static StoryModel parseStoryItem(final JSONObject data,
                                            final boolean isLocOrHashtag,
                                            final String username) throws JSONException {
        final boolean isVideo = data.has("video_duration");
        final StoryModel model = new StoryModel(data.getString("id"),
                                                data.getJSONObject("image_versions2").getJSONArray("candidates").getJSONObject(0)
                                                    .getString("url"), null,
                                                isVideo ? MediaItemType.MEDIA_TYPE_VIDEO : MediaItemType.MEDIA_TYPE_IMAGE,
                                                data.optLong("taken_at", 0),
                                                isLocOrHashtag ? data.getJSONObject("user").getString("username") : username,
                                                data.getJSONObject("user").getLong("pk"),
                                                data.optBoolean("can_reply"));

        if (data.getJSONObject("image_versions2").getJSONArray("candidates").length() > 1) {
            model.setThumbnail(data.getJSONObject("image_versions2").getJSONArray("candidates").getJSONObject(1)
                                   .getString("url"));
        }

        final JSONArray videoResources = data.optJSONArray("video_versions");
        if (isVideo && videoResources != null)
            model.setVideoUrl(ResponseBodyUtils.getHighQualityPost(videoResources, true, true, false));

        if (data.has("story_feed_media")) {
            model.setTappableShortCode(data.getJSONArray("story_feed_media").getJSONObject(0).optString("media_id"));
        }

        // TODO: this may not be limited to spotify
        if (!data.isNull("story_app_attribution"))
            model.setSpotify(data.getJSONObject("story_app_attribution").optString("content_url").split("\\?")[0]);

        if (data.has("story_polls")) {
            final JSONArray storyPolls = data.optJSONArray("story_polls");
            JSONObject tappableObject = null;
            if (storyPolls != null) {
                tappableObject = storyPolls.getJSONObject(0).optJSONObject("poll_sticker");
            }
            if (tappableObject != null) model.setPoll(new PollModel(
                    String.valueOf(tappableObject.getLong("poll_id")),
                    tappableObject.getString("question"),
                    tappableObject.getJSONArray("tallies").getJSONObject(0).getString("text"),
                    tappableObject.getJSONArray("tallies").getJSONObject(0).getInt("count"),
                    tappableObject.getJSONArray("tallies").getJSONObject(1).getString("text"),
                    tappableObject.getJSONArray("tallies").getJSONObject(1).getInt("count"),
                    tappableObject.optInt("viewer_vote", -1)
            ));
        }
        if (data.has("story_questions")) {
            final JSONObject tappableObject = data.getJSONArray("story_questions").getJSONObject(0)
                                                  .optJSONObject("question_sticker");
            if (tappableObject != null && !tappableObject.getString("question_type").equals("music"))
                model.setQuestion(new QuestionModel(
                        String.valueOf(tappableObject.getLong("question_id")),
                        tappableObject.getString("question")
                ));
        }
        if (data.has("story_quizs")) {
            JSONObject tappableObject = data.getJSONArray("story_quizs").getJSONObject(0).optJSONObject("quiz_sticker");
            if (tappableObject != null) {
                String[] choices = new String[tappableObject.getJSONArray("tallies").length()];
                Long[] counts = new Long[choices.length];
                for (int q = 0; q < choices.length; ++q) {
                    JSONObject tempchoice = tappableObject.getJSONArray("tallies").getJSONObject(q);
                    choices[q] = (q == tappableObject.getInt("correct_answer") ? "*** " : "")
                            + tempchoice.getString("text");
                    counts[q] = tempchoice.getLong("count");
                }
                model.setQuiz(new QuizModel(
                        String.valueOf(tappableObject.getLong("quiz_id")),
                        tappableObject.getString("question"),
                        choices,
                        counts,
                        tappableObject.optInt("viewer_answer", -1)
                ));
            }
        }
        if (data.has("story_cta") && data.has("link_text")) {
            JSONObject tappableObject = data.getJSONArray("story_cta").getJSONObject(0).getJSONArray("links").getJSONObject(0);
            String swipeUpUrl = tappableObject.optString("webUri");
            final String backupSwipeUpUrl = swipeUpUrl;
            if (swipeUpUrl != null && swipeUpUrl.startsWith("https://l.instagram.com/")) {
                swipeUpUrl = Uri.parse(swipeUpUrl).getQueryParameter("u");
            }
            if (swipeUpUrl != null && swipeUpUrl.startsWith("http"))
                model.setSwipeUp(new SwipeUpModel(swipeUpUrl, data.getString("link_text")));
            else if (backupSwipeUpUrl != null && backupSwipeUpUrl.startsWith("http"))
                model.setSwipeUp(new SwipeUpModel(backupSwipeUpUrl, data.getString("link_text")));
        }
        if (data.has("story_sliders")) {
            final JSONObject tappableObject = data.getJSONArray("story_sliders").getJSONObject(0)
                                                  .optJSONObject("slider_sticker");
            if (tappableObject != null)
                model.setSlider(new SliderModel(
                        String.valueOf(tappableObject.getLong("slider_id")),
                        tappableObject.getString("question"),
                        tappableObject.getString("emoji"),
                        tappableObject.getBoolean("viewer_can_vote"),
                        tappableObject.optDouble("slider_vote_average"),
                        tappableObject.getInt("slider_vote_count"),
                        tappableObject.optDouble("viewer_vote")
                ));
        }
        JSONArray hashtags = data.optJSONArray("story_hashtags");
        JSONArray locations = data.optJSONArray("story_locations");
        JSONArray atmarks = data.optJSONArray("reel_mentions");
        String[] mentions = new String[(hashtags == null ? 0 : hashtags.length())
                + (atmarks == null ? 0 : atmarks.length())
                + (locations == null ? 0 : locations.length())];
        if (hashtags != null) {
            for (int h = 0; h < hashtags.length(); ++h) {
                mentions[h] = "#" + hashtags.getJSONObject(h).getJSONObject("hashtag").getString("name");
            }
        }
        if (atmarks != null) {
            for (int h = 0; h < atmarks.length(); ++h) {
                mentions[h + (hashtags == null ? 0 : hashtags.length())] =
                        "@" + atmarks.getJSONObject(h).getJSONObject("user").getString("username");
            }
        }
        if (locations != null) {
            for (int h = 0; h < locations.length(); ++h) {
                mentions[h + (hashtags == null ? 0 : hashtags.length()) + (atmarks == null ? 0 : atmarks.length())] =
                        locations.getJSONObject(h).getJSONObject("location").getString("short_name")
                                + " (" + locations.getJSONObject(h).getJSONObject("location").getLong("pk") + ")";
            }
        }
        if (mentions.length != 0) model.setMentions(mentions);

        return model;
    }

    public static String getThumbUrl(final Media media) {
        return getImageCandidate(media, CandidateType.THUMBNAIL);
    }

    public static String getImageUrl(final Media media) {
        return getImageCandidate(media, CandidateType.DOWNLOAD);
    }

    private static String getImageCandidate(final Media media, final CandidateType type) {
        if (media == null) return null;
        final ImageVersions2 imageVersions2 = media.getImageVersions2();
        if (imageVersions2 == null) return null;
        final List<MediaCandidate> candidates = imageVersions2.getCandidates();
        if (candidates == null || candidates.isEmpty()) return null;
        final boolean isSquare = Integer.compare(media.getOriginalWidth(), media.getOriginalHeight()) == 0;
        final List<MediaCandidate> sortedCandidates = candidates.stream()
                .sorted((c1, c2) -> Integer.compare(c2.getWidth(), c1.getWidth()))
                .collect(Collectors.toList());
        final List<MediaCandidate> filteredCandidates = sortedCandidates.stream()
                .filter(c ->
                        c.getWidth() <= media.getOriginalWidth()
                                && c.getWidth() <= type.getValue()
                                && (isSquare || Integer.compare(c.getWidth(), c.getHeight()) != 0)
                )
                .collect(Collectors.toList());
        if (filteredCandidates.size() == 0) return sortedCandidates.get(0).getUrl();
        final MediaCandidate candidate = filteredCandidates.get(0);
        if (candidate == null) return null;
        return candidate.getUrl();
    }

    public static StoryModel parseBroadcastItem(final JSONObject data) throws JSONException {
        final StoryModel model = new StoryModel(data.getString("id"),
                                                data.getString("cover_frame_url"),
                                                data.getString("cover_frame_url"),
                                                MediaItemType.MEDIA_TYPE_LIVE,
                                                data.optLong("published_time", 0),
                                                data.getJSONObject("broadcast_owner").getString("username"),
                                                data.getJSONObject("broadcast_owner").getLong("pk"),
                                                false);
        model.setVideoUrl(data.getString("dash_playback_url"));
        return model;
    }

    private enum CandidateType {
        THUMBNAIL(1000),
        DOWNLOAD(10000);

        private final int value;

        CandidateType(final int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }
    }
}
