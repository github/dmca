package awais.instagrabber.utils.emoji;

import com.google.gson.JsonArray;
import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;

import java.lang.reflect.Type;
import java.util.LinkedList;
import java.util.List;

import awais.instagrabber.customviews.emoji.Emoji;

public class EmojiDeserializer implements JsonDeserializer<Emoji> {
    @Override
    public Emoji deserialize(final JsonElement json,
                             final Type typeOfT,
                             final JsonDeserializationContext context) throws JsonParseException {
        final JsonObject jsonObject = json.getAsJsonObject();
        final JsonElement unicodeElement = jsonObject.get("unicode");
        final JsonElement nameElement = jsonObject.get("name");
        if (unicodeElement == null || nameElement == null) {
            throw new JsonParseException("Invalid json for Emoji class");
        }
        final JsonElement variantsElement = jsonObject.get("variants");
        final List<Emoji> variants = new LinkedList<>();
        if (variantsElement != null) {
            final JsonArray variantsArray = variantsElement.getAsJsonArray();
            for (final JsonElement variantElement : variantsArray) {
                final Emoji variant = context.deserialize(variantElement, Emoji.class);
                if (variant != null) {
                    variants.add(variant);
                }
            }
        }
        return new Emoji(
                unicodeElement.getAsString(),
                nameElement.getAsString(),
                variants
        );
    }
}
