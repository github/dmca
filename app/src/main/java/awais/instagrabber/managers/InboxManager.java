package awais.instagrabber.managers;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Transformations;

import com.google.common.cache.CacheBuilder;
import com.google.common.cache.CacheLoader;
import com.google.common.cache.LoadingCache;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.Iterables;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.models.Resource;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectBadgeCount;
import awais.instagrabber.repositories.responses.directmessages.DirectInbox;
import awais.instagrabber.repositories.responses.directmessages.DirectInboxResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.webservices.DirectMessagesService;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import static androidx.lifecycle.Transformations.distinctUntilChanged;
import static awais.instagrabber.utils.Utils.settingsHelper;

public final class InboxManager {
    private static final String TAG = InboxManager.class.getSimpleName();
    private static final LoadingCache<String, Object> THREAD_LOCKS = CacheBuilder
            .newBuilder()
            .expireAfterAccess(1, TimeUnit.MINUTES) // max lock time ever expected
            .build(CacheLoader.from(Object::new));
    private static final Comparator<DirectThread> THREAD_COMPARATOR = (t1, t2) -> {
        final DirectItem t1FirstDirectItem = t1.getFirstDirectItem();
        final DirectItem t2FirstDirectItem = t2.getFirstDirectItem();
        if (t1FirstDirectItem == null && t2FirstDirectItem == null) return 0;
        if (t1FirstDirectItem == null) return 1;
        if (t2FirstDirectItem == null) return -1;
        return Long.compare(t2FirstDirectItem.getTimestamp(), t1FirstDirectItem.getTimestamp());
    };

    private final MutableLiveData<Resource<DirectInbox>> inbox = new MutableLiveData<>();
    private final MutableLiveData<Resource<Integer>> unseenCount = new MutableLiveData<>();
    private final MutableLiveData<Integer> pendingRequestsTotal = new MutableLiveData<>(0);

    private final LiveData<List<DirectThread>> threads;
    private final DirectMessagesService service;
    private final boolean pending;

    private Call<DirectInboxResponse> inboxRequest;
    private Call<DirectBadgeCount> unseenCountRequest;
    private long seqId;
    private String cursor;
    private boolean hasOlder = true;
    private User viewer;

    @NonNull
    public static InboxManager getInstance(final boolean pending) {
        return new InboxManager(pending);
    }

    private InboxManager(final boolean pending) {
        this.pending = pending;
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        final long userId = CookieUtils.getUserIdFromCookie(cookie);
        final String deviceUuid = settingsHelper.getString(Constants.DEVICE_UUID);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        if (TextUtils.isEmpty(csrfToken)) {
            throw new IllegalArgumentException("csrfToken is empty!");
        } else if (userId == 0) {
            throw new IllegalArgumentException("user id invalid");
        } else if (TextUtils.isEmpty(deviceUuid)) {
            throw new IllegalArgumentException("device uuid is empty!");
        }
        service = DirectMessagesService.getInstance(csrfToken, userId, deviceUuid);

        // Transformations
        threads = distinctUntilChanged(Transformations.map(inbox, inboxResource -> {
            if (inboxResource == null) {
                return Collections.emptyList();
            }
            final DirectInbox inbox = inboxResource.data;
            if (inbox == null) {
                return Collections.emptyList();
            }
            return ImmutableList.sortedCopyOf(THREAD_COMPARATOR, inbox.getThreads());
        }));

        fetchInbox();
        if (!pending) {
            fetchUnseenCount();
        }
    }

    public LiveData<Resource<DirectInbox>> getInbox() {
        return distinctUntilChanged(inbox);
    }

    public LiveData<List<DirectThread>> getThreads() {
        return threads;
    }

    public LiveData<Resource<Integer>> getUnseenCount() {
        return distinctUntilChanged(unseenCount);
    }

    public LiveData<Integer> getPendingRequestsTotal() {
        return distinctUntilChanged(pendingRequestsTotal);
    }

    public User getViewer() {
        return viewer;
    }

    public void fetchInbox() {
        final Resource<DirectInbox> inboxResource = inbox.getValue();
        if ((inboxResource != null && inboxResource.status == Resource.Status.LOADING) || !hasOlder) return;
        stopCurrentInboxRequest();
        inbox.postValue(Resource.loading(getCurrentDirectInbox()));
        inboxRequest = pending ? service.fetchPendingInbox(cursor, seqId) : service.fetchInbox(cursor, seqId);
        inboxRequest.enqueue(new Callback<DirectInboxResponse>() {
            @Override
            public void onResponse(@NonNull final Call<DirectInboxResponse> call, @NonNull final Response<DirectInboxResponse> response) {
                parseInboxResponse(response.body());
            }

            @Override
            public void onFailure(@NonNull final Call<DirectInboxResponse> call, @NonNull final Throwable t) {
                Log.e(TAG, "Failed fetching dm inbox", t);
                inbox.postValue(Resource.error(t.getMessage(), getCurrentDirectInbox()));
                hasOlder = false;
            }
        });
    }

    public void fetchUnseenCount() {
        final Resource<Integer> unseenCountResource = unseenCount.getValue();
        if ((unseenCountResource != null && unseenCountResource.status == Resource.Status.LOADING)) return;
        stopCurrentUnseenCountRequest();
        unseenCount.postValue(Resource.loading(getCurrentUnseenCount()));
        unseenCountRequest = service.fetchUnseenCount();
        unseenCountRequest.enqueue(new Callback<DirectBadgeCount>() {
            @Override
            public void onResponse(@NonNull final Call<DirectBadgeCount> call, @NonNull final Response<DirectBadgeCount> response) {
                final DirectBadgeCount directBadgeCount = response.body();
                if (directBadgeCount == null) {
                    Log.e(TAG, "onResponse: directBadgeCount Response is null");
                    unseenCount.postValue(Resource.error(R.string.dms_inbox_error_null_count, getCurrentUnseenCount()));
                    return;
                }
                unseenCount.postValue(Resource.success(directBadgeCount.getBadgeCount()));
            }

            @Override
            public void onFailure(@NonNull final Call<DirectBadgeCount> call, @NonNull final Throwable t) {
                Log.e(TAG, "Failed fetching unseen count", t);
                unseenCount.postValue(Resource.error(t.getMessage(), getCurrentUnseenCount()));
            }
        });
    }

    public void refresh() {
        cursor = null;
        seqId = 0;
        hasOlder = true;
        fetchInbox();
        if (!pending) {
            fetchUnseenCount();
        }
    }

    private DirectInbox getCurrentDirectInbox() {
        final Resource<DirectInbox> inboxResource = inbox.getValue();
        return inboxResource != null ? inboxResource.data : null;
    }

    private void parseInboxResponse(final DirectInboxResponse response) {
        if (response == null) {
            Log.e(TAG, "parseInboxResponse: Response is null");
            inbox.postValue(Resource.error(R.string.generic_null_response, getCurrentDirectInbox()));
            hasOlder = false;
            return;
        }
        if (!response.getStatus().equals("ok")) {
            Log.e(TAG, "DM inbox fetch response: status not ok");
            inbox.postValue(Resource.error(R.string.generic_not_ok_response, getCurrentDirectInbox()));
            hasOlder = false;
            return;
        }
        seqId = response.getSeqId();
        if (viewer == null) {
            viewer = response.getViewer();
        }
        final DirectInbox inbox = response.getInbox();
        if (!TextUtils.isEmpty(cursor)) {
            final DirectInbox currentDirectInbox = getCurrentDirectInbox();
            if (currentDirectInbox != null) {
                List<DirectThread> threads = currentDirectInbox.getThreads();
                threads = threads == null ? new LinkedList<>() : new LinkedList<>(threads);
                threads.addAll(inbox.getThreads());
                inbox.setThreads(threads);
            }
        }
        this.inbox.postValue(Resource.success(inbox));
        cursor = inbox.getOldestCursor();
        hasOlder = inbox.hasOlder();
        pendingRequestsTotal.postValue(response.getPendingRequestsTotal());
    }

    public void setThread(@NonNull final String threadId,
                          @NonNull final DirectThread thread) {
        final DirectInbox inbox = getCurrentDirectInbox();
        if (inbox == null) return;
        final int index = getThreadIndex(threadId, inbox);
        setThread(inbox, index, thread);
    }

    private void setThread(@NonNull final DirectInbox inbox,
                           final int index,
                           @NonNull final DirectThread thread) {
        if (index < 0) return;
        synchronized (this.inbox) {
            final List<DirectThread> threadsCopy = new LinkedList<>(inbox.getThreads());
            threadsCopy.set(index, thread);
            try {
                final DirectInbox clone = (DirectInbox) inbox.clone();
                clone.setThreads(threadsCopy);
                this.inbox.postValue(Resource.success(clone));
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "setThread: ", e);
            }
        }
    }

    public void addItemsToThread(@NonNull final String threadId,
                                 final int insertIndex,
                                 @NonNull final Collection<DirectItem> items) {
        final DirectInbox inbox = getCurrentDirectInbox();
        if (inbox == null) return;
        synchronized (THREAD_LOCKS.getUnchecked(threadId)) {
            final int index = getThreadIndex(threadId, inbox);
            if (index < 0) return;
            final List<DirectThread> threads = inbox.getThreads();
            final DirectThread thread = threads.get(index);
            List<DirectItem> list = thread.getItems();
            list = list == null ? new LinkedList<>() : new LinkedList<>(list);
            if (insertIndex >= 0) {
                list.addAll(insertIndex, items);
            } else {
                list.addAll(items);
            }
            try {
                final DirectThread threadClone = (DirectThread) thread.clone();
                threadClone.setItems(list);
                setThread(inbox, index, threadClone);
            } catch (Exception e) {
                Log.e(TAG, "addItemsToThread: ", e);
            }
        }
    }

    public void setItemsToThread(@NonNull final String threadId,
                                 @NonNull final List<DirectItem> updatedItems) {
        final DirectInbox inbox = getCurrentDirectInbox();
        if (inbox == null) return;
        synchronized (THREAD_LOCKS.getUnchecked(threadId)) {
            final int index = getThreadIndex(threadId, inbox);
            if (index < 0) return;
            final List<DirectThread> threads = inbox.getThreads();
            final DirectThread thread = threads.get(index);
            try {
                final DirectThread threadClone = (DirectThread) thread.clone();
                threadClone.setItems(updatedItems);
                setThread(inbox, index, threadClone);
            } catch (Exception e) {
                Log.e(TAG, "setItemsToThread: ", e);
            }
        }
    }

    private int getThreadIndex(@NonNull final String threadId,
                               @NonNull final DirectInbox inbox) {
        final List<DirectThread> threads = inbox.getThreads();
        if (threads == null || threads.isEmpty()) {
            return -1;
        }
        return Iterables.indexOf(threads, t -> {
            if (t == null) return false;
            return t.getThreadId().equals(threadId);
        });
    }

    private Integer getCurrentUnseenCount() {
        final Resource<Integer> unseenCountResource = unseenCount.getValue();
        return unseenCountResource != null ? unseenCountResource.data : null;
    }

    private void stopCurrentInboxRequest() {
        if (inboxRequest == null || inboxRequest.isCanceled() || inboxRequest.isExecuted()) return;
        inboxRequest.cancel();
        inboxRequest = null;
    }

    private void stopCurrentUnseenCountRequest() {
        if (unseenCountRequest == null || unseenCountRequest.isCanceled() || unseenCountRequest.isExecuted()) return;
        unseenCountRequest.cancel();
        unseenCountRequest = null;
    }

    public void onDestroy() {
        stopCurrentInboxRequest();
        stopCurrentUnseenCountRequest();
    }

    public void addThread(@NonNull final DirectThread thread, final int insertIndex) {
        if (insertIndex < 0) return;
        synchronized (this.inbox) {
            final DirectInbox currentDirectInbox = getCurrentDirectInbox();
            if (currentDirectInbox == null) return;
            final List<DirectThread> threadsCopy = new LinkedList<>(currentDirectInbox.getThreads());
            threadsCopy.add(insertIndex, thread);
            try {
                final DirectInbox clone = (DirectInbox) currentDirectInbox.clone();
                clone.setThreads(threadsCopy);
                this.inbox.setValue(Resource.success(clone));
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "setThread: ", e);
            }
        }
    }

    public void removeThread(@NonNull final String threadId) {
        synchronized (this.inbox) {
            final DirectInbox currentDirectInbox = getCurrentDirectInbox();
            if (currentDirectInbox == null) return;
            final List<DirectThread> threadsCopy = currentDirectInbox.getThreads()
                                                                     .stream()
                                                                     .filter(t -> !t.getThreadId().equals(threadId))
                                                                     .collect(Collectors.toList());
            try {
                final DirectInbox clone = (DirectInbox) currentDirectInbox.clone();
                clone.setThreads(threadsCopy);
                this.inbox.postValue(Resource.success(clone));
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "setThread: ", e);
            }
        }
    }

    public void setPendingRequestsTotal(final int total) {
        pendingRequestsTotal.postValue(total);
    }

    public boolean containsThread(final String threadId) {
        if (threadId == null) return false;
        synchronized (this.inbox) {
            final DirectInbox currentDirectInbox = getCurrentDirectInbox();
            if (currentDirectInbox == null) return false;
            final List<DirectThread> threads = currentDirectInbox.getThreads();
            if (threads == null) return false;
            return threads.stream().anyMatch(thread -> Objects.equals(thread.getThreadId(), threadId));
        }
    }
}
