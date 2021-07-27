package awais.instagrabber.repositories.requests.directmessages;

import java.util.HashMap;
import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;

public class PhotoBroadcastOptions extends BroadcastOptions {
    final boolean allowFullAspectRatio;
    final String uploadId;

    public PhotoBroadcastOptions(final String clientContext,
                                 final ThreadIdOrUserIds threadIdOrUserIds,
                                 final boolean allowFullAspectRatio,
                                 final String uploadId) {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.IMAGE);
        this.allowFullAspectRatio = allowFullAspectRatio;
        this.uploadId = uploadId;
    }

    @Override
    public Map<String, String> getFormMap() {
        final Map<String, String> form = new HashMap<>();
        form.put("allow_full_aspect_ratio", String.valueOf(allowFullAspectRatio));
        form.put("upload_id", uploadId);
        return form;
    }
}
