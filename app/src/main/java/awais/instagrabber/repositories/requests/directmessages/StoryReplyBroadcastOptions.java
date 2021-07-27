package awais.instagrabber.repositories.requests.directmessages;

import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;
import awais.instagrabber.utils.TextUtils;

public class StoryReplyBroadcastOptions extends BroadcastOptions {
    private final String text, mediaId, reelId;

    public StoryReplyBroadcastOptions(final String clientContext,
                                      final ThreadIdOrUserIds threadIdOrUserIds,
                                      final String text,
                                      final String mediaId,
                                      final String reelId)
            throws UnsupportedEncodingException {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.REELSHARE);
        this.text = text;
        this.mediaId = mediaId;
        this.reelId = reelId; // or user id, usually same
    }

    @Override
    public Map<String, String> getFormMap() {
        final Map<String, String> form = new HashMap<>();
        form.put("text", text);
        form.put("media_id", mediaId);
        form.put("reel_id", reelId);
        form.put("entry", "reel");
        return form;
    }
}
