package awais.instagrabber.customviews.emoji;

import androidx.annotation.DrawableRes;
import androidx.annotation.NonNull;

import java.util.Map;
import java.util.Objects;

import awais.instagrabber.R;

public class EmojiCategory {
    private final EmojiCategoryType type;
    private final Map<String, Emoji> emojis;
    @DrawableRes
    private int drawableRes;

    public EmojiCategory(final EmojiCategoryType type, final Map<String, Emoji> emojis) {
        this.type = type;
        this.emojis = emojis;
    }

    public EmojiCategoryType getType() {
        return type;
    }

    public Map<String, Emoji> getEmojis() {
        return emojis;
    }

    public int getDrawableRes() {
        if (drawableRes == 0) {
            switch (type) {
                case SMILEYS_AND_EMOTION:
                    drawableRes = R.drawable.ic_round_emoji_emotions_24;
                    break;
                case ANIMALS_AND_NATURE:
                    drawableRes = R.drawable.ic_round_emoji_nature_24;
                    break;
                case FOOD_AND_DRINK:
                    drawableRes = R.drawable.ic_round_emoji_food_beverage_24;
                    break;
                case TRAVEL_AND_PLACES:
                    drawableRes = R.drawable.ic_round_emoji_transportation_24;
                    break;
                case ACTIVITIES:
                    drawableRes = R.drawable.ic_round_emoji_events_24;
                    break;
                case OBJECTS:
                    drawableRes = R.drawable.ic_round_emoji_objects_24;
                    break;
                case SYMBOLS:
                    drawableRes = R.drawable.ic_round_emoji_symbols_24;
                    break;
                case FLAGS:
                    drawableRes = R.drawable.ic_round_emoji_flags_24;
                    break;
                case OTHERS:
                    drawableRes = R.drawable.ic_round_unknown_24;
                    break;
            }
        }
        return drawableRes;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final EmojiCategory that = (EmojiCategory) o;
        return type == that.type;
    }

    @Override
    public int hashCode() {
        return Objects.hash(type);
    }

    @NonNull
    @Override
    public String toString() {
        return "EmojiCategory{" +
                "type=" + type +
                ", emojis=" + emojis +
                '}';
    }
}
