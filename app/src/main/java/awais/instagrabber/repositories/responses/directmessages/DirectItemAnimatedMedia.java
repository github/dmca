package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

import awais.instagrabber.repositories.responses.AnimatedMediaImages;

public final class DirectItemAnimatedMedia implements Serializable {
    private final String id;
    private final AnimatedMediaImages images;
    private final boolean isRandom;
    private final boolean isSticker;

    public DirectItemAnimatedMedia(final String id, final AnimatedMediaImages images, final boolean isRandom, final boolean isSticker) {

        this.id = id;
        this.images = images;
        this.isRandom = isRandom;
        this.isSticker = isSticker;
    }

    public String getId() {
        return id;
    }

    public AnimatedMediaImages getImages() {
        return images;
    }

    public boolean isRandom() {
        return isRandom;
    }

    public boolean isSticker() {
        return isSticker;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemAnimatedMedia that = (DirectItemAnimatedMedia) o;
        return isRandom == that.isRandom &&
                isSticker == that.isSticker &&
                Objects.equals(id, that.id) &&
                Objects.equals(images, that.images);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, images, isRandom, isSticker);
    }
}
