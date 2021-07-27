package awais.instagrabber.utils.emoji;

import android.util.Log;

import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;

import java.lang.reflect.Type;
import java.util.LinkedHashMap;
import java.util.Map;

import awais.instagrabber.customviews.emoji.Emoji;
import awais.instagrabber.customviews.emoji.EmojiCategory;
import awais.instagrabber.customviews.emoji.EmojiCategoryType;

public class EmojiCategoryDeserializer implements JsonDeserializer<EmojiCategory> {
    private static final String TAG = EmojiCategoryDeserializer.class.getSimpleName();

    @Override
    public EmojiCategory deserialize(final JsonElement json,
                                     final Type typeOfT,
                                     final JsonDeserializationContext context) throws JsonParseException {
        final JsonObject jsonObject = json.getAsJsonObject();
        final JsonElement typeElement = jsonObject.get("type");
        final JsonObject emojisObject = jsonObject.getAsJsonObject("emojis");
        if (typeElement == null || emojisObject == null) {
            throw new JsonParseException("Invalid json for EmojiCategory");
        }
        final String typeString = typeElement.getAsString();
        EmojiCategoryType type;
        try {
            type = EmojiCategoryType.valueOf(typeString);
        } catch (IllegalArgumentException e) {
            Log.e(TAG, "deserialize: ", e);
            type = EmojiCategoryType.OTHERS;
        }
        final Map<String, Emoji> emojis = new LinkedHashMap<>();
        for (final Map.Entry<String, JsonElement> emojiObjectEntry : emojisObject.entrySet()) {
            final String unicode = emojiObjectEntry.getKey();
            final JsonElement value = emojiObjectEntry.getValue();
            if (unicode == null || value == null) {
                throw new JsonParseException("Invalid json for EmojiCategory");
            }
            final Emoji emoji = context.deserialize(value, Emoji.class);
            emojis.put(unicode, emoji);
        }
        return new EmojiCategory(type, emojis);
    }
}
