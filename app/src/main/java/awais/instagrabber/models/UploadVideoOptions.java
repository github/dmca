package awais.instagrabber.models;

import awais.instagrabber.models.enums.MediaItemType;

public class UploadVideoOptions {
    private final String uploadId;
    private final String name;
    private final long byteLength;
    private final long duration;
    private final int width;
    private final int height;
    private final boolean isSideCar;
    private final boolean forAlbum; // Stories
    private final boolean isDirect;
    private final boolean isDirectVoice;
    private final boolean forDirectStory;
    private final boolean isIgtvVideo;
    private final String waterfallId;
    private final long offset;
    private final MediaItemType mediaType;

    public static class Builder {
        private String uploadId;
        private String name;
        private long byteLength;
        private long duration;
        private int width;
        private int height;
        private boolean isSideCar;
        private boolean forAlbum; // Stories
        private boolean isDirect;
        private boolean isDirectVoice;
        private boolean forDirectStory;
        private boolean isIgtvVideo;
        private String waterfallId;
        private long offset;
        private MediaItemType mediaType;

        private Builder() {}

        public Builder setUploadId(final String uploadId) {
            this.uploadId = uploadId;
            return this;
        }

        public Builder setName(final String name) {
            this.name = name;
            return this;
        }

        public Builder setByteLength(final long byteLength) {
            this.byteLength = byteLength;
            return this;
        }

        public Builder setDuration(final long duration) {
            this.duration = duration;
            return this;
        }

        public Builder setWidth(final int width) {
            this.width = width;
            return this;
        }

        public Builder setHeight(final int height) {
            this.height = height;
            return this;
        }

        public Builder setIsSideCar(final boolean isSideCar) {
            this.isSideCar = isSideCar;
            return this;
        }

        public Builder setForAlbum(final boolean forAlbum) {
            this.forAlbum = forAlbum;
            return this;
        }

        public Builder setIsDirect(final boolean isDirect) {
            this.isDirect = isDirect;
            return this;
        }

        public Builder setIsDirectVoice(final boolean isDirectVoice) {
            this.isDirectVoice = isDirectVoice;
            return this;
        }

        public Builder setForDirectStory(final boolean forDirectStory) {
            this.forDirectStory = forDirectStory;
            return this;
        }

        public Builder setIsIgtvVideo(final boolean isIgtvVideo) {
            this.isIgtvVideo = isIgtvVideo;
            return this;
        }

        public Builder setWaterfallId(final String waterfallId) {
            this.waterfallId = waterfallId;
            return this;
        }

        public Builder setOffset(final long offset) {
            this.offset = offset;
            return this;
        }

        public Builder setMediaType(final MediaItemType mediaType) {
            this.mediaType = mediaType;
            return this;
        }

        public UploadVideoOptions build() {
            return new UploadVideoOptions(uploadId, name, byteLength, duration, width, height, isSideCar, forAlbum, isDirect, isDirectVoice,
                                          forDirectStory, isIgtvVideo, waterfallId, offset, mediaType);
        }
    }

    public static Builder builder() {
        return new Builder();
    }

    private UploadVideoOptions(final String uploadId,
                               final String name,
                               final long byteLength,
                               final long duration,
                               final int width,
                               final int height,
                               final boolean isSideCar,
                               final boolean forAlbum,
                               final boolean isDirect,
                               final boolean isDirectVoice,
                               final boolean forDirectStory,
                               final boolean isIgtvVideo,
                               final String waterfallId,
                               final long offset,
                               final MediaItemType mediaType) {
        this.uploadId = uploadId;
        this.name = name;
        this.byteLength = byteLength;
        this.duration = duration;
        this.width = width;
        this.height = height;
        this.isSideCar = isSideCar;
        this.forAlbum = forAlbum;
        this.isDirect = isDirect;
        this.isDirectVoice = isDirectVoice;
        this.forDirectStory = forDirectStory;
        this.isIgtvVideo = isIgtvVideo;
        this.waterfallId = waterfallId;
        this.offset = offset;
        this.mediaType = mediaType;
    }

    public String getUploadId() {
        return uploadId;
    }

    public String getName() {
        return name;
    }

    public long getByteLength() {
        return byteLength;
    }

    public long getDuration() {
        return duration;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public boolean isSideCar() {
        return isSideCar;
    }

    public boolean isForAlbum() {
        return forAlbum;
    }

    public boolean isDirect() {
        return isDirect;
    }

    public boolean isDirectVoice() {
        return isDirectVoice;
    }

    public boolean isForDirectStory() {
        return forDirectStory;
    }

    public boolean isIgtvVideo() {
        return isIgtvVideo;
    }

    public String getWaterfallId() {
        return waterfallId;
    }

    public long getOffset() {
        return offset;
    }

    public MediaItemType getMediaType() {
        return mediaType;
    }
}
