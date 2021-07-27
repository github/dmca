package awais.instagrabber.repositories.responses.saved;

import java.io.Serializable;
import java.util.List;

import awais.instagrabber.repositories.responses.Media;

public class SavedCollection implements Serializable {
    private final String collectionId;
    private final String collectionName;
    private final String collectionType;
    private final int collectionMediacount;
    private final Media coverMedia;
    private final List<Media> coverMediaList;

    public SavedCollection(final String collectionId,
                           final String collectionName,
                           final String collectionType,
                           final int collectionMediacount,
                           final Media coverMedia,
                           final List<Media> coverMediaList) {
        this.collectionId = collectionId;
        this.collectionName = collectionName;
        this.collectionType = collectionType;
        this.collectionMediacount = collectionMediacount;
        this.coverMedia = coverMedia;
        this.coverMediaList = coverMediaList;
    }

    public String getId() {
        return collectionId;
    }

    public String getTitle() {
        return collectionName;
    }

    public String getType() {
        return collectionType;
    }

    public int getMediaCount() {
        return collectionMediacount;
    }

    // check the list first, then the single
    // i have no idea what condition is required

    public Media getCoverMedia() { return coverMedia; }

    public List<Media> getCoverMedias() {
        return coverMediaList;
    }
}
