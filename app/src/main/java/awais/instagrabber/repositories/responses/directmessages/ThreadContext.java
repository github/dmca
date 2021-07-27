package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

public class ThreadContext implements Serializable {
    private final int type;
    private final String text;

    public ThreadContext(final int type, final String text) {
        this.type = type;
        this.text = text;
    }

    public int getType() {
        return type;
    }

    public String getText() {
        return text;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final ThreadContext that = (ThreadContext) o;
        return type == that.type &&
                Objects.equals(text, that.text);
    }

    @Override
    public int hashCode() {
        return Objects.hash(type, text);
    }
}
