package awais.instagrabber.repositories.requests.directmessages;

import com.google.common.collect.ImmutableMap;

import org.json.JSONArray;

import java.util.List;
import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;

public class VoiceBroadcastOptions extends BroadcastOptions {
    private final List<Float> waveform;
    private final String uploadId;
    private final int waveformSamplingFrequencyHz;

    public VoiceBroadcastOptions(final String clientContext,
                                 final ThreadIdOrUserIds threadIdOrUserIds,
                                 final String uploadId,
                                 final List<Float> waveform,
                                 final int waveformSamplingFrequencyHz) {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.VOICE);
        this.waveform = waveform;
        this.uploadId = uploadId;
        this.waveformSamplingFrequencyHz = waveformSamplingFrequencyHz;
    }

    @Override
    public Map<String, String> getFormMap() {
        return ImmutableMap.<String, String>of(
                "waveform", new JSONArray(waveform).toString(),
                "upload_id", uploadId,
                "waveform_sampling_frequency_hz", String.valueOf(waveformSamplingFrequencyHz)
        );
    }
}
