package awais.instagrabber.repositories.responses;

import java.io.Serializable;
import java.util.Objects;

public class AnimatedMediaImages implements Serializable {
    private final AnimatedMediaFixedHeight fixedHeight;

    public AnimatedMediaImages(final AnimatedMediaFixedHeight fixedHeight) {
        this.fixedHeight = fixedHeight;
    }

    public AnimatedMediaFixedHeight getFixedHeight() {
        return fixedHeight;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final AnimatedMediaImages that = (AnimatedMediaImages) o;
        return Objects.equals(fixedHeight, that.fixedHeight);
    }

    @Override
    public int hashCode() {
        return Objects.hash(fixedHeight);
    }
}
