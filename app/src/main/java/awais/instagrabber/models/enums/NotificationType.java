package awais.instagrabber.models.enums;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

public enum NotificationType implements Serializable {
    // story_type
    LIKE(60),
    FOLLOW(101),
    COMMENT(12), // NOT TESTED
    COMMENT_MENTION(66),
    TAGGED(102), // NOT TESTED
    COMMENT_LIKE(13),
    TAGGED_COMMENT(14),
    RESPONDED_STORY(213),
    REQUEST(75),
    // aymf - arbitrary, misspelled as ayml but eh
    AYML(9999);

    private final int itemType;
    private static final Map<Integer, NotificationType> map = new HashMap<>();

    static {
        for (NotificationType type : NotificationType.values()) {
            map.put(type.itemType, type);
        }
    }

    NotificationType(final int itemType) {
        this.itemType = itemType;
    }

    public int getItemType() {
        return itemType;
    }

    public static NotificationType valueOfType(final int itemType) {
        return map.get(itemType);
    }
}