package awais.instagrabber.models;

import androidx.annotation.NonNull;

import java.io.Serializable;
import java.util.Date;

import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Utils;

public final class FeedStoryModel implements Serializable {
    private final String storyMediaId;
    private final User profileModel;
    private final StoryModel firstStoryModel;
    private Boolean fullyRead;
    private final boolean isLive, isBestie;
    private final long timestamp;
    private final int mediaCount;

    public FeedStoryModel(final String storyMediaId,
                          final User profileModel,
                          final boolean fullyRead,
                          final long timestamp,
                          final StoryModel firstStoryModel,
                          final int mediaCount,
                          final boolean isLive,
                          final boolean isBestie) {
        this.storyMediaId = storyMediaId;
        this.profileModel = profileModel;
        this.fullyRead = fullyRead;
        this.timestamp = timestamp;
        this.firstStoryModel = firstStoryModel;
        this.mediaCount = mediaCount;
        this.isLive = isLive;
        this.isBestie = isBestie;
    }

    public String getStoryMediaId() {
        return storyMediaId;
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

    public User getProfileModel() {
        return profileModel;
    }

    public StoryModel getFirstStoryModel() {
        return firstStoryModel;
    }

    public Boolean isFullyRead() {
        return fullyRead;
    }

    public void setFullyRead(final boolean fullyRead) {
        this.fullyRead = fullyRead;
    }

    public boolean isLive() {
        return isLive;
    }

    public boolean isBestie() {
        return isBestie;
    }
}