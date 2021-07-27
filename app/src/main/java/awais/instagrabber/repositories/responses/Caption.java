package awais.instagrabber.repositories.responses;

import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;

import java.io.Serializable;
import java.lang.reflect.Type;
import java.util.Objects;

public class Caption implements Serializable {
    private long mPk;
    private final long userId;
    private String text;

    public Caption(final long userId, final String text) {
        this.userId = userId;
        this.text = text;
    }

    public long getPk() {
        return mPk;
    }

    public void setPk(final long pk) {
        this.mPk = pk;
    }

    public long getUserId() {
        return userId;
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Caption caption = (Caption) o;
        return mPk == caption.mPk &&
                userId == caption.userId &&
                Objects.equals(text, caption.text);
    }

    @Override
    public int hashCode() {
        return Objects.hash(mPk, userId, text);
    }

    public static class CaptionDeserializer implements JsonDeserializer<Caption> {

        private static final String TAG = CaptionDeserializer.class.getSimpleName();

        @Override
        public Caption deserialize(JsonElement json, Type typeOfT, JsonDeserializationContext context) throws JsonParseException {
            final Caption caption = new Gson().fromJson(json, Caption.class);
            final JsonObject jsonObject = json.getAsJsonObject();
            if (jsonObject.has("pk")) {
                JsonElement elem = jsonObject.get("pk");
                if (elem != null && !elem.isJsonNull()) {
                    if (!elem.isJsonPrimitive()) return caption;
                    String pkString = elem.getAsString();
                    if (pkString.contains("_")) {
                        pkString = pkString.substring(0, pkString.indexOf("_"));
                    }
                    try {
                        caption.setPk(Long.parseLong(pkString));
                    } catch (NumberFormatException e) {
                        Log.e(TAG, "deserialize: ", e);
                    }
                }
            }
            return caption;
        }
    }

}
