package awais.instagrabber.repositories.requests.directmessages;

import com.google.common.collect.ImmutableMap;

import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;

public class VideoBroadcastOptions extends BroadcastOptions {
    private final String videoResult;
    private final String uploadId;
    private final boolean sampled;

    public VideoBroadcastOptions(final String clientContext,
                                 final ThreadIdOrUserIds threadIdOrUserIds,
                                 final String videoResult,
                                 final String uploadId,
                                 final boolean sampled) {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.VIDEO);
        this.videoResult = videoResult;
        this.uploadId = uploadId;
        this.sampled = sampled;
    }

    @Override
    public Map<String, String> getFormMap() {
        return ImmutableMap.<String, String>of(
                "video_result", videoResult,
                "upload_id", uploadId,
                "sampled", String.valueOf(sampled)
        );
    }
}
