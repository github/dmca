package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class DirectItemActionLog implements Serializable {
    private final String description;
    private final List<TextRange> bold;
    private final List<TextRange> textAttributes;

    public DirectItemActionLog(final String description,
                               final List<TextRange> bold,
                               final List<TextRange> textAttributes) {
        this.description = description;
        this.bold = bold;
        this.textAttributes = textAttributes;
    }

    public String getDescription() {
        return description;
    }

    public List<TextRange> getBold() {
        return bold;
    }

    public List<TextRange> getTextAttributes() {
        return textAttributes;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemActionLog that = (DirectItemActionLog) o;
        return Objects.equals(description, that.description) &&
                Objects.equals(bold, that.bold) &&
                Objects.equals(textAttributes, that.textAttributes);
    }

    @Override
    public int hashCode() {
        return Objects.hash(description, bold, textAttributes);
    }

    public static class TextRange implements Serializable {
        private final int start;
        private final int end;
        private final String color;
        private final String intent;

        public TextRange(final int start, final int end, final String color, final String intent) {
            this.start = start;
            this.end = end;
            this.color = color;
            this.intent = intent;
        }

        public int getStart() {
            return start;
        }

        public int getEnd() {
            return end;
        }

        public String getColor() {
            return color;
        }

        public String getIntent() {
            return intent;
        }

        @Override
        public boolean equals(final Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            final TextRange textRange = (TextRange) o;
            return start == textRange.start &&
                    end == textRange.end &&
                    Objects.equals(color, textRange.color) &&
                    Objects.equals(intent, textRange.intent);
        }

        @Override
        public int hashCode() {
            return Objects.hash(start, end, color, intent);
        }
    }
}
