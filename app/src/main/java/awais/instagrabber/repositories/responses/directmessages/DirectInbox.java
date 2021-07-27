package awais.instagrabber.repositories.responses.directmessages;

import androidx.annotation.NonNull;

import java.util.List;

public class DirectInbox implements Cloneable {
    private List<DirectThread> threads;
    private final boolean hasOlder;
    private final int unseenCount;
    private final String unseenCountTs;
    private final String oldestCursor;
    private final boolean blendedInboxEnabled;

    public DirectInbox(final List<DirectThread> threads,
                       final boolean hasOlder,
                       final int unseenCount,
                       final String unseenCountTs,
                       final String oldestCursor,
                       final boolean blendedInboxEnabled) {
        this.threads = threads;
        this.hasOlder = hasOlder;
        this.unseenCount = unseenCount;
        this.unseenCountTs = unseenCountTs;
        this.oldestCursor = oldestCursor;
        this.blendedInboxEnabled = blendedInboxEnabled;
    }

    public List<DirectThread> getThreads() {
        return threads;
    }

    public void setThreads(final List<DirectThread> threads) {
        this.threads = threads;
    }

    public boolean hasOlder() {
        return hasOlder;
    }

    public int getUnseenCount() {
        return unseenCount;
    }

    public String getUnseenCountTs() {
        return unseenCountTs;
    }

    public String getOldestCursor() {
        return oldestCursor;
    }

    public boolean isBlendedInboxEnabled() {
        return blendedInboxEnabled;
    }

    @NonNull
    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }
}
