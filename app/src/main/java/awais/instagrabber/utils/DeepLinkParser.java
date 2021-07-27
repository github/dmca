package awais.instagrabber.utils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.common.collect.ImmutableMap;

import java.util.Map;
import java.util.regex.Pattern;

public final class DeepLinkParser {
    private static final Map<DeepLink.Type, DeepLinkPattern> TYPE_PATTERN_MAP = ImmutableMap
            .<DeepLink.Type, DeepLinkPattern>builder()
            .put(DeepLink.Type.USER, new DeepLinkPattern("instagram://user?username="))
            .build();

    @Nullable
    public static DeepLink parse(@NonNull final String text) {
        for (final Map.Entry<DeepLink.Type, DeepLinkPattern> entry : TYPE_PATTERN_MAP.entrySet()) {
            if (text.startsWith(entry.getValue().getPatternText())) {
                final String value = entry.getValue().getPattern().matcher(text).replaceAll("");
                return new DeepLink(entry.getKey(), value);
            }
        }
        return null;
    }

    public static class DeepLinkPattern {
        private final String patternText;
        private final Pattern pattern;

        public DeepLinkPattern(final String patternText) {
            this.patternText = patternText;
            pattern = Pattern.compile(patternText, Pattern.LITERAL);
        }

        public String getPatternText() {
            return patternText;
        }

        public Pattern getPattern() {
            return pattern;
        }
    }

    public static class DeepLink {
        private final Type type;
        private final String value;

        public DeepLink(final Type type, final String value) {
            this.type = type;
            this.value = value;
        }

        public Type getType() {
            return type;
        }

        public String getValue() {
            return value;
        }

        public enum Type {
            USER,
        }
    }
}
