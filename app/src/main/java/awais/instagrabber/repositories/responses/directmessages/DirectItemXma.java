package awais.instagrabber.repositories.responses.directmessages;

import androidx.annotation.NonNull;

import java.io.Serializable;
import java.util.Objects;

public class DirectItemXma {
    private final XmaUrlInfo previewUrlInfo;
    private final XmaUrlInfo playableUrlInfo;

    public DirectItemXma(final XmaUrlInfo previewUrlInfo, final XmaUrlInfo playableUrlInfo) {
        this.previewUrlInfo = previewUrlInfo;
        this.playableUrlInfo = playableUrlInfo;
    }

    public XmaUrlInfo getPreviewUrlInfo() {
        return previewUrlInfo;
    }

    public XmaUrlInfo getPlayableUrlInfo() {
        return playableUrlInfo;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemXma that = (DirectItemXma) o;
        return Objects.equals(previewUrlInfo, that.previewUrlInfo) &&
                Objects.equals(playableUrlInfo, that.playableUrlInfo);
    }

    @Override
    public int hashCode() {
        return Objects.hash(previewUrlInfo, playableUrlInfo);
    }

    @NonNull
    @Override
    public String toString() {
        return "DirectItemXma{" +
                "previewUrlInfo=" + previewUrlInfo +
                ", playableUrlInfo=" + playableUrlInfo +
                '}';
    }

    public static class XmaUrlInfo implements Serializable {
        private final String url;
        private final long urlExpirationTimestampUs;
        private final int width;
        private final int height;

        public XmaUrlInfo(final String url, final long urlExpirationTimestampUs, final int width, final int height) {
            this.url = url;
            this.urlExpirationTimestampUs = urlExpirationTimestampUs;
            this.width = width;
            this.height = height;
        }

        public String getUrl() {
            return url;
        }

        public long getUrlExpirationTimestampUs() {
            return urlExpirationTimestampUs;
        }

        public int getWidth() {
            return width;
        }

        public int getHeight() {
            return height;
        }

        @Override
        public boolean equals(final Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            final XmaUrlInfo that = (XmaUrlInfo) o;
            return urlExpirationTimestampUs == that.urlExpirationTimestampUs &&
                    width == that.width &&
                    height == that.height &&
                    Objects.equals(url, that.url);
        }

        @Override
        public int hashCode() {
            return Objects.hash(url, urlExpirationTimestampUs, width, height);
        }

        @NonNull
        @Override
        public String toString() {
            return "XmaUrlInfo{" +
                    "url='" + url + '\'' +
                    ", urlExpirationTimestampUs=" + urlExpirationTimestampUs +
                    ", width=" + width +
                    ", height=" + height +
                    '}';
        }
    }
}
