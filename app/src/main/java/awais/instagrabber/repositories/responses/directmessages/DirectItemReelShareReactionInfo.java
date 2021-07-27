package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

public class DirectItemReelShareReactionInfo implements Serializable {
    private final String emoji;
    private final String intensity;

    public DirectItemReelShareReactionInfo(final String emoji, final String intensity) {
        this.emoji = emoji;
        this.intensity = intensity;
    }

    public String getEmoji() {
        return emoji;
    }

    public String getIntensity() {
        return intensity;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemReelShareReactionInfo that = (DirectItemReelShareReactionInfo) o;
        return Objects.equals(emoji, that.emoji) &&
                Objects.equals(intensity, that.intensity);
    }

    @Override
    public int hashCode() {
        return Objects.hash(emoji, intensity);
    }
}
