package awais.instagrabber.models.enums;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

public enum DirectItemType implements Serializable {
    UNKNOWN(0),
    @SerializedName("text")
    TEXT(1),
    @SerializedName("like")
    LIKE(2),
    @SerializedName("link")
    LINK(3),
    @SerializedName("media")
    MEDIA(4),
    @SerializedName("raven_media")
    RAVEN_MEDIA(5),
    @SerializedName("profile")
    PROFILE(6),
    @SerializedName("video_call_event")
    VIDEO_CALL_EVENT(7),
    @SerializedName("animated_media")
    ANIMATED_MEDIA(8),
    @SerializedName("voice_media")
    VOICE_MEDIA(9),
    @SerializedName("media_share")
    MEDIA_SHARE(10),
    @SerializedName("reel_share")
    REEL_SHARE(11),
    @SerializedName("action_log")
    ACTION_LOG(12),
    @SerializedName("placeholder")
    PLACEHOLDER(13),
    @SerializedName("story_share")
    STORY_SHARE(14),
    @SerializedName("clip")
    CLIP(15),        // media_share but reel
    @SerializedName("felix_share")
    FELIX_SHARE(16), // media_share but igtv
    @SerializedName("location")
    LOCATION(17),
    @SerializedName("xma")
    XMA(18); // self avatar stickers

    private final int id;
    private static final Map<Integer, DirectItemType> map = new HashMap<>();

    static {
        for (DirectItemType type : DirectItemType.values()) {
            map.put(type.id, type);
        }
    }

    DirectItemType(final int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    public static DirectItemType valueOf(final int id) {
        if (!map.containsKey(id)) return DirectItemType.UNKNOWN;
        return map.get(id);
    }

    public String getName() {
        switch (this) {
            case TEXT:
                return "text";
            case LIKE:
                return "like";
            case LINK:
                return "link";
            case MEDIA:
                return "media";
            case RAVEN_MEDIA:
                return "raven_media";
            case PROFILE:
                return "profile";
            case VIDEO_CALL_EVENT:
                return "video_call_event";
            case ANIMATED_MEDIA:
                return "animated_media";
            case VOICE_MEDIA:
                return "voice_media";
            case MEDIA_SHARE:
                return "media_share";
            case REEL_SHARE:
                return "reel_share";
            case ACTION_LOG:
                return "action_log";
            case PLACEHOLDER:
                return "placeholder";
            case STORY_SHARE:
                return "story_share";
            case CLIP:
                return "clip";
            case FELIX_SHARE:
                return "felix_share";
            case LOCATION:
                return "location";
        }
        return null;
    }
}