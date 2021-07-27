package awais.instagrabber.managers;

import android.content.ContentResolver;

import androidx.annotation.NonNull;

import com.google.common.collect.Iterables;

import java.util.List;

import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;

public final class DirectMessagesManager {
    private static final String TAG = DirectMessagesManager.class.getSimpleName();
    private static final Object LOCK = new Object();

    private static DirectMessagesManager instance;

    private final InboxManager inboxManager;
    private final InboxManager pendingInboxManager;

    public static DirectMessagesManager getInstance() {
        if (instance == null) {
            synchronized (LOCK) {
                if (instance == null) {
                    instance = new DirectMessagesManager();
                }
            }
        }
        return instance;
    }

    private DirectMessagesManager() {
        inboxManager = InboxManager.getInstance(false);
        pendingInboxManager = InboxManager.getInstance(true);
    }

    public void moveThreadFromPending(@NonNull final String threadId) {
        final List<DirectThread> pendingThreads = pendingInboxManager.getThreads().getValue();
        if (pendingThreads == null) return;
        final int index = Iterables.indexOf(pendingThreads, t -> t.getThreadId().equals(threadId));
        if (index < 0) return;
        final DirectThread thread = pendingThreads.get(index);
        final DirectItem threadFirstDirectItem = thread.getFirstDirectItem();
        if (threadFirstDirectItem == null) return;
        final List<DirectThread> threads = inboxManager.getThreads().getValue();
        int insertIndex = 0;
        for (final DirectThread tempThread : threads) {
            final DirectItem firstDirectItem = tempThread.getFirstDirectItem();
            if (firstDirectItem == null) continue;
            final long timestamp = firstDirectItem.getTimestamp();
            if (timestamp < threadFirstDirectItem.getTimestamp()) {
                break;
            }
            insertIndex++;
        }
        thread.setPending(false);
        inboxManager.addThread(thread, insertIndex);
        pendingInboxManager.removeThread(threadId);
        final Integer currentTotal = inboxManager.getPendingRequestsTotal().getValue();
        if (currentTotal == null) return;
        inboxManager.setPendingRequestsTotal(currentTotal - 1);
    }

    public InboxManager getInboxManager() {
        return inboxManager;
    }

    public InboxManager getPendingInboxManager() {
        return pendingInboxManager;
    }

    public ThreadManager getThreadManager(@NonNull final String threadId,
                                          final boolean pending,
                                          @NonNull final User currentUser,
                                          @NonNull final ContentResolver contentResolver) {
        return ThreadManager.getInstance(threadId, pending, currentUser, contentResolver);
    }
}
