package awais.instagrabber.models.enums;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

public enum FollowingType implements Serializable {
    @SerializedName("1")
    FOLLOWING(1),
    @SerializedName("0")
    NOT_FOLLOWING(0);

    private final int id;
    private static final Map<Integer, FollowingType> map = new HashMap<>();

    static {
        for (FollowingType type : FollowingType.values()) {
            map.put(type.id, type);
        }
    }

    FollowingType(final int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    public static FollowingType valueOf(final int id) {
        return map.get(id);
    }
}