package awais.instagrabber.viewmodels;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;

import awais.instagrabber.managers.DirectMessagesManager;
import awais.instagrabber.managers.InboxManager;
import awais.instagrabber.models.Resource;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectInbox;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;

public class DirectInboxViewModel extends ViewModel {
    private static final String TAG = DirectInboxViewModel.class.getSimpleName();

    private final InboxManager inboxManager;

    public DirectInboxViewModel() {
        final DirectMessagesManager messagesManager = DirectMessagesManager.getInstance();
        inboxManager = messagesManager.getInboxManager();
    }

    public LiveData<Resource<DirectInbox>> getInbox() {
        return inboxManager.getInbox();
    }

    public LiveData<List<DirectThread>> getThreads() {
        return inboxManager.getThreads();
    }

    public LiveData<Resource<Integer>> getUnseenCount() {
        return inboxManager.getUnseenCount();
    }

    public LiveData<Integer> getPendingRequestsTotal() {
        return inboxManager.getPendingRequestsTotal();
    }

    public User getViewer() {
        return inboxManager.getViewer();
    }

    public void fetchInbox() {
        inboxManager.fetchInbox();
    }

    public void refresh() {
        inboxManager.refresh();
    }

    public void onDestroy() {
        inboxManager.onDestroy();
    }
}
