package awais.instagrabber.customviews.emoji;

import androidx.annotation.NonNull;

import java.util.List;
import java.util.Objects;

public class Emoji {
    private final String unicode;
    private final String name;
    private final List<Emoji> variants;
    private GoogleCompatEmojiDrawable drawable;

    public Emoji(final String unicode,
                 final String name,
                 final List<Emoji> variants) {
        this.unicode = unicode;
        this.name = name;
        this.variants = variants;
    }

    public String getUnicode() {
        return unicode;
    }

    public void addVariant(final Emoji emoji) {
        variants.add(emoji);
    }

    public String getName() {
        return name;
    }

    public List<Emoji> getVariants() {
        return variants;
    }

    public GoogleCompatEmojiDrawable getDrawable() {
        if (drawable == null && unicode != null) {
            drawable = new GoogleCompatEmojiDrawable(unicode);
        }
        return drawable;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Emoji emoji = (Emoji) o;
        return Objects.equals(unicode, emoji.unicode);
    }

    @Override
    public int hashCode() {
        return Objects.hash(unicode);
    }

    @NonNull
    @Override
    public String toString() {
        return "Emoji{" +
                "unicode='" + unicode + '\'' +
                ", name='" + name + '\'' +
                ", variants=" + variants +
                '}';
    }
}
