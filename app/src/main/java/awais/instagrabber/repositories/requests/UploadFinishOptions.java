package awais.instagrabber.repositories.requests;

import com.google.common.collect.ImmutableMap;

import java.util.List;
import java.util.Map;

public class UploadFinishOptions {
    private String uploadId;
    private String sourceType;
    private VideoOptions videoOptions;

    public String getUploadId() {
        return uploadId;
    }

    public UploadFinishOptions setUploadId(final String uploadId) {
        this.uploadId = uploadId;
        return this;
    }

    public String getSourceType() {
        return sourceType;
    }

    public UploadFinishOptions setSourceType(final String sourceType) {
        this.sourceType = sourceType;
        return this;
    }

    public VideoOptions getVideoOptions() {
        return videoOptions;
    }

    public UploadFinishOptions setVideoOptions(final VideoOptions videoOptions) {
        this.videoOptions = videoOptions;
        return this;
    }

    public static class VideoOptions {
        private float length;
        private List<Clip> clips;
        private int posterFrameIndex;
        private boolean audioMuted;

        public float getLength() {
            return length;
        }

        public VideoOptions setLength(final float length) {
            this.length = length;
            return this;
        }

        public List<Clip> getClips() {
            return clips;
        }

        public VideoOptions setClips(final List<Clip> clips) {
            this.clips = clips;
            return this;
        }

        public int getPosterFrameIndex() {
            return posterFrameIndex;
        }

        public VideoOptions setPosterFrameIndex(final int posterFrameIndex) {
            this.posterFrameIndex = posterFrameIndex;
            return this;
        }

        public boolean isAudioMuted() {
            return audioMuted;
        }

        public VideoOptions setAudioMuted(final boolean audioMuted) {
            this.audioMuted = audioMuted;
            return this;
        }

        public Map<String, Object> getMap() {
            return ImmutableMap.of(
                    "length", length,
                    "clips", clips,
                    "poster_frame_index", posterFrameIndex,
                    "audio_muted", audioMuted
            );
        }
    }

    public static class Clip {
        private float length;
        private String sourceType;

        public float getLength() {
            return length;
        }

        public Clip setLength(final float length) {
            this.length = length;
            return this;
        }

        public String getSourceType() {
            return sourceType;
        }

        public Clip setSourceType(final String sourceType) {
            this.sourceType = sourceType;
            return this;
        }
    }
}
