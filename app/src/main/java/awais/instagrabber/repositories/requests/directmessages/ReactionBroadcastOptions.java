package awais.instagrabber.repositories.requests.directmessages;

import java.util.HashMap;
import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;
import awais.instagrabber.utils.TextUtils;

public class ReactionBroadcastOptions extends BroadcastOptions {
    private final String itemId;
    private final String emoji;
    private final boolean delete;

    public ReactionBroadcastOptions(final String clientContext,
                                    final ThreadIdOrUserIds threadIdOrUserIds,
                                    final String itemId,
                                    final String emoji,
                                    final boolean delete) {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.REACTION);
        this.itemId = itemId;
        this.emoji = emoji;
        this.delete = delete;
    }

    @Override
    public Map<String, String> getFormMap() {
        final Map<String, String> form = new HashMap<>();
        form.put("item_id", itemId);
        form.put("reaction_status", delete ? "deleted" : "created");
        form.put("reaction_type", "like");
        if (!TextUtils.isEmpty(emoji)) {
            form.put("emoji", emoji);
        }
        return form;
    }
}
