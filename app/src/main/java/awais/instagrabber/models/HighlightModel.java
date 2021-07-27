package awais.instagrabber.models;

import androidx.annotation.NonNull;

import java.util.Date;

import awais.instagrabber.utils.Utils;

public final class HighlightModel {
    private final String title;
    private final String id;
    private final String thumbnailUrl;
    private final long timestamp;
    private final int mediaCount;

    public HighlightModel(final String title,
                          final String id,
                          final String thumbnailUrl,
                          final long timestamp,
                          final int mediaCount) {
        this.title = title;
        this.id = id;
        this.thumbnailUrl = thumbnailUrl;
        this.timestamp = timestamp;
        this.mediaCount = mediaCount;
    }

    public String getTitle() {
        return title;
    }

    public String getId() {
        return id;
    }

    public String getThumbnailUrl() {
        return thumbnailUrl;
    }

    public long getTimestamp() {
        return timestamp;
    }

    @NonNull
    public String getDateTime() {
        return Utils.datetimeParser.format(new Date(timestamp * 1000L));
    }

    public int getMediaCount() {
        return mediaCount;
    }
}