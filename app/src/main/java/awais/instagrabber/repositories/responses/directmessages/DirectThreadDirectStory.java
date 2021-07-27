package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class DirectThreadDirectStory implements Serializable {
    private final List<DirectItem> items;
    private final int unseenCount;

    public DirectThreadDirectStory(final List<DirectItem> items, final int unseenCount) {
        this.items = items;
        this.unseenCount = unseenCount;
    }

    public List<DirectItem> getItems() {
        return items;
    }

    public int getUnseenCount() {
        return unseenCount;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectThreadDirectStory that = (DirectThreadDirectStory) o;
        return unseenCount == that.unseenCount &&
                Objects.equals(items, that.items);
    }

    @Override
    public int hashCode() {
        return Objects.hash(items, unseenCount);
    }
}
