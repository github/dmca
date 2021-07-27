package awais.instagrabber.models.enums;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

public enum MediaItemType implements Serializable {
    @SerializedName("1")
    MEDIA_TYPE_IMAGE(1),
    @SerializedName("2")
    MEDIA_TYPE_VIDEO(2),
    @SerializedName("8")
    MEDIA_TYPE_SLIDER(8),
    @SerializedName("11")
    MEDIA_TYPE_VOICE(11),
    // 5 is arbitrary
    @SerializedName("5")
    MEDIA_TYPE_LIVE(5);

    private final int id;
    private static final Map<Integer, MediaItemType> map = new HashMap<>();

    static {
        for (MediaItemType type : MediaItemType.values()) {
            map.put(type.id, type);
        }
    }

    MediaItemType(final int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    public static MediaItemType valueOf(final int id) {
        return map.get(id);
    }
}