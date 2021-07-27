package awais.instagrabber.models;

public class UploadPhotoOptions {
    private final String uploadId;
    private final String name;
    // private final Uri uri;
    private final long byteLength;
    private final boolean isSideCar;
    private final String waterfallId;

    public static class Builder {
        private String uploadId;
        private String name;
        // private Uri uri;
        private boolean isSideCar;
        private String waterfallId;
        private long byteLength;

        private Builder() {}

        public Builder setUploadId(final String uploadId) {
            this.uploadId = uploadId;
            return this;
        }

        public Builder setName(final String name) {
            this.name = name;
            return this;
        }

        // public Builder setUri(final Uri uri) {
        //     this.uri = uri;
        //     return this;
        // }

        public Builder setByteLength(final long byteLength) {
            this.byteLength = byteLength;
            return this;
        }

        public Builder setIsSideCar(final boolean isSideCar) {
            this.isSideCar = isSideCar;
            return this;
        }

        public Builder setWaterfallId(final String waterfallId) {
            this.waterfallId = waterfallId;
            return this;
        }

        public UploadPhotoOptions build() {
            return new UploadPhotoOptions(uploadId, name, /*uri,*/ byteLength, isSideCar, waterfallId);
        }
    }

    public static Builder builder() {
        return new Builder();
    }

    public UploadPhotoOptions(final String uploadId,
                              final String name,
                              // final Uri uri,
                              final long byteLength,
                              final boolean isSideCar,
                              final String waterfallId) {
        this.uploadId = uploadId;
        this.name = name;
        // this.uri = uri;
        this.byteLength = byteLength;
        this.isSideCar = isSideCar;
        this.waterfallId = waterfallId;
    }

    public String getUploadId() {
        return uploadId;
    }

    public String getName() {
        return name;
    }

    // public Uri getUri() {
    //     return uri;
    // }

    public long getByteLength() {
        return byteLength;
    }

    public boolean isSideCar() {
        return isSideCar;
    }

    public String getWaterfallId() {
        return waterfallId;
    }
}
