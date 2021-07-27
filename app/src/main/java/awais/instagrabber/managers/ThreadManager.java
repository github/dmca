package awais.instagrabber.managers;

import android.content.ContentResolver;
import android.net.Uri;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.util.Pair;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.google.common.collect.ImmutableList;
import com.google.common.collect.Iterables;

import org.json.JSONObject;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.HttpURLConnection;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.customviews.emoji.Emoji;
import awais.instagrabber.models.Resource;
import awais.instagrabber.models.Resource.Status;
import awais.instagrabber.models.UploadVideoOptions;
import awais.instagrabber.models.enums.DirectItemType;
import awais.instagrabber.repositories.requests.UploadFinishOptions;
import awais.instagrabber.repositories.requests.directmessages.BroadcastOptions.ThreadIdOrUserIds;
import awais.instagrabber.repositories.responses.FriendshipChangeResponse;
import awais.instagrabber.repositories.responses.FriendshipRestrictResponse;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectInbox;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemEmojiReaction;
import awais.instagrabber.repositories.responses.directmessages.DirectItemReactions;
import awais.instagrabber.repositories.responses.directmessages.DirectItemSeenResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectItemSeenResponse.DirectItemSeenResponsePayload;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadBroadcastResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadBroadcastResponseMessageMetadata;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadBroadcastResponsePayload;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadDetailsChangeResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadFeedResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadLastSeenAt;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadParticipantRequestsResponse;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipient;
import awais.instagrabber.repositories.responses.giphy.GiphyGif;
import awais.instagrabber.utils.BitmapUtils;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.DirectItemFactory;
import awais.instagrabber.utils.MediaController;
import awais.instagrabber.utils.MediaUploadHelper;
import awais.instagrabber.utils.MediaUploader;
import awais.instagrabber.utils.MediaUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.webservices.DirectMessagesService;
import awais.instagrabber.webservices.FriendshipService;
import awais.instagrabber.webservices.MediaService;
import awais.instagrabber.webservices.ServiceCallback;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import static androidx.lifecycle.Transformations.distinctUntilChanged;
import static androidx.lifecycle.Transformations.map;
import static awais.instagrabber.utils.Utils.settingsHelper;

public final class ThreadManager {
    private static final String TAG = ThreadManager.class.getSimpleName();
    private static final Object LOCK = new Object();
    private static final Map<String, ThreadManager> INSTANCE_MAP = new ConcurrentHashMap<>();

    private final MutableLiveData<Resource<Object>> fetching = new MutableLiveData<>();
    private final MutableLiveData<DirectItem> replyToItem = new MutableLiveData<>();
    private final MutableLiveData<DirectThreadParticipantRequestsResponse> pendingRequests = new MutableLiveData<>(null);

    private final String threadId;
    private final long viewerId;
    private final ThreadIdOrUserIds threadIdOrUserIds;
    private final User currentUser;
    private final ContentResolver contentResolver;

    private DirectMessagesService service;
    private MediaService mediaService;
    private FriendshipService friendshipService;
    private InboxManager inboxManager;
    private LiveData<DirectThread> thread;
    private LiveData<Integer> inputMode;
    private LiveData<String> threadTitle;
    private LiveData<List<User>> users;
    private LiveData<List<User>> usersWithCurrent;
    private LiveData<List<User>> leftUsers;
    private LiveData<Pair<List<User>, List<User>>> usersAndLeftUsers;
    private LiveData<Boolean> pending;
    private LiveData<List<Long>> adminUserIds;
    private LiveData<List<DirectItem>> items;
    private LiveData<Boolean> isViewerAdmin;
    private LiveData<Boolean> isGroup;
    private LiveData<Boolean> isMuted;
    private LiveData<Boolean> isApprovalRequiredToJoin;
    private LiveData<Boolean> isMentionsMuted;
    private LiveData<Integer> pendingRequestsCount;
    private LiveData<User> inviter;
    private boolean hasOlder = true;
    private String cursor;
    private Call<DirectThreadFeedResponse> chatsRequest;

    public static ThreadManager getInstance(@NonNull final String threadId,
                                            final boolean pending,
                                            @NonNull final User currentUser,
                                            @NonNull final ContentResolver contentResolver) {
        ThreadManager instance = INSTANCE_MAP.get(threadId);
        if (instance == null) {
            synchronized (LOCK) {
                instance = INSTANCE_MAP.get(threadId);
                if (instance == null) {
                    instance = new ThreadManager(threadId, pending, currentUser, contentResolver);
                    INSTANCE_MAP.put(threadId, instance);
                }
            }
        }
        return instance;
    }

    private String getThreadId() {
        return threadId;
    }

    private ThreadManager(@NonNull final String threadId,
                          final boolean pending,
                          @NonNull final User currentUser,
                          @NonNull final ContentResolver contentResolver) {
        final DirectMessagesManager messagesManager = DirectMessagesManager.getInstance();
        this.inboxManager = pending ? messagesManager.getPendingInboxManager() : messagesManager.getInboxManager();
        this.threadId = threadId;
        this.threadIdOrUserIds = ThreadIdOrUserIds.of(threadId);
        this.currentUser = currentUser;
        this.contentResolver = contentResolver;
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        viewerId = CookieUtils.getUserIdFromCookie(cookie);
        final String deviceUuid = settingsHelper.getString(Constants.DEVICE_UUID);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        if (csrfToken == null) return;
        // if (TextUtils.isEmpty(csrfToken) || viewerId <= 0 || TextUtils.isEmpty(deviceUuid)) {
        //     throw new IllegalArgumentException("User is not logged in!");
        // }
        service = DirectMessagesService.getInstance(csrfToken, viewerId, deviceUuid);
        mediaService = MediaService.getInstance(deviceUuid, csrfToken, viewerId);
        friendshipService = FriendshipService.getInstance(deviceUuid, csrfToken, viewerId);
        setupTransformations();
        // fetchChats();
    }

    public void moveFromPending() {
        final DirectMessagesManager messagesManager = DirectMessagesManager.getInstance();
        this.inboxManager = messagesManager.getInboxManager();
        setupTransformations();
    }

    private void setupTransformations() {
        // Transformations
        thread = distinctUntilChanged(map(inboxManager.getInbox(), inboxResource -> {
            if (inboxResource == null) {
                return null;
            }
            final DirectInbox inbox = inboxResource.data;
            if (inbox == null) {
                return null;
            }
            final List<DirectThread> threads = inbox.getThreads();
            if (threads == null || threads.isEmpty()) {
                return null;
            }
            final DirectThread thread = threads.stream()
                                               .filter(t -> Objects.equals(t.getThreadId(), threadId))
                                               .findFirst()
                                               .orElse(null);
            if (thread != null) {
                cursor = thread.getOldestCursor();
                hasOlder = thread.hasOlder();
            }
            return thread;
        }));
        inputMode = distinctUntilChanged(map(thread, t -> {
            if (t == null) return 1;
            return t.getInputMode();
        }));
        threadTitle = distinctUntilChanged(map(thread, t -> {
            if (t == null) return null;
            return t.getThreadTitle();
        }));
        users = distinctUntilChanged(map(thread, t -> {
            if (t == null) return Collections.emptyList();
            return t.getUsers();
        }));
        usersWithCurrent = distinctUntilChanged(map(thread, t -> {
            if (t == null) return Collections.emptyList();
            return getUsersWithCurrentUser(t);
        }));
        leftUsers = distinctUntilChanged(map(thread, t -> {
            if (t == null) return Collections.emptyList();
            return t.getLeftUsers();
        }));
        usersAndLeftUsers = distinctUntilChanged(map(thread, t -> {
            if (t == null) {
                return new Pair<>(Collections.emptyList(), Collections.emptyList());
            }
            final List<User> users = getUsersWithCurrentUser(t);
            final List<User> leftUsers = t.getLeftUsers();
            return new Pair<>(users, leftUsers);
        }));
        pending = distinctUntilChanged(map(thread, t -> {
            if (t == null) return true;
            return t.isPending();
        }));
        adminUserIds = distinctUntilChanged(map(thread, t -> {
            if (t == null) return Collections.emptyList();
            return t.getAdminUserIds();
        }));
        items = distinctUntilChanged(map(thread, t -> {
            if (t == null) return Collections.emptyList();
            return t.getItems();
        }));
        isViewerAdmin = distinctUntilChanged(map(thread, t -> {
            if (t == null) return false;
            return t.getAdminUserIds().contains(viewerId);
        }));
        isGroup = distinctUntilChanged(map(thread, t -> {
            if (t == null) return false;
            return t.isGroup();
        }));
        isMuted = distinctUntilChanged(map(thread, t -> {
            if (t == null) return false;
            return t.isMuted();
        }));
        isApprovalRequiredToJoin = distinctUntilChanged(map(thread, t -> {
            if (t == null) return false;
            return t.isApprovalRequiredForNewMembers();
        }));
        isMentionsMuted = distinctUntilChanged(map(thread, t -> {
            if (t == null) return false;
            return t.isMentionsMuted();
        }));
        pendingRequestsCount = distinctUntilChanged(map(pendingRequests, p -> {
            if (p == null) return 0;
            return p.getTotalParticipantRequests();
        }));
        inviter = distinctUntilChanged(map(thread, t -> {
            if (t == null) return null;
            return t.getInviter();
        }));
    }

    private List<User> getUsersWithCurrentUser(final DirectThread t) {
        final ImmutableList.Builder<User> builder = ImmutableList.builder();
        if (currentUser != null) {
            builder.add(currentUser);
        }
        final List<User> users = t.getUsers();
        if (users != null) {
            builder.addAll(users);
        }
        return builder.build();
    }

    public LiveData<DirectThread> getThread() {
        return thread;
    }

    public LiveData<Integer> getInputMode() {
        return inputMode;
    }

    public LiveData<String> getThreadTitle() {
        return threadTitle;
    }

    public LiveData<List<User>> getUsers() {
        return users;
    }

    public LiveData<List<User>> getUsersWithCurrent() {
        return usersWithCurrent;
    }

    public LiveData<List<User>> getLeftUsers() {
        return leftUsers;
    }

    public LiveData<Pair<List<User>, List<User>>> getUsersAndLeftUsers() {
        return usersAndLeftUsers;
    }

    public LiveData<Boolean> isPending() {
        return pending;
    }

    public LiveData<List<Long>> getAdminUserIds() {
        return adminUserIds;
    }

    public LiveData<List<DirectItem>> getItems() {
        return items;
    }

    public LiveData<Resource<Object>> isFetching() {
        return fetching;
    }

    public LiveData<DirectItem> getReplyToItem() {
        return replyToItem;
    }

    public LiveData<Integer> getPendingRequestsCount() {
        return pendingRequestsCount;
    }

    public LiveData<DirectThreadParticipantRequestsResponse> getPendingRequests() {
        return pendingRequests;
    }

    public LiveData<Boolean> isGroup() {
        return isGroup;
    }

    public LiveData<Boolean> isMuted() {
        return isMuted;
    }

    public LiveData<Boolean> isApprovalRequiredToJoin() {
        return isApprovalRequiredToJoin;
    }

    public LiveData<Boolean> isViewerAdmin() {
        return isViewerAdmin;
    }

    public LiveData<Boolean> isMentionsMuted() {
        return isMentionsMuted;
    }

    public void fetchChats() {
        final Resource<Object> fetchingValue = fetching.getValue();
        if ((fetchingValue != null && fetchingValue.status == Status.LOADING) || !hasOlder) return;
        fetching.postValue(Resource.loading(null));
        chatsRequest = service.fetchThread(threadId, cursor);
        chatsRequest.enqueue(new Callback<DirectThreadFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<DirectThreadFeedResponse> call, @NonNull final Response<DirectThreadFeedResponse> response) {
                final DirectThreadFeedResponse feedResponse = response.body();
                if (feedResponse == null) {
                    fetching.postValue(Resource.error(R.string.generic_null_response, null));
                    Log.e(TAG, "onResponse: response was null!");
                    return;
                }
                if (!feedResponse.getStatus().equals("ok")) {
                    fetching.postValue(Resource.error(R.string.generic_not_ok_response, null));
                    return;
                }
                final DirectThread thread = feedResponse.getThread();
                setThread(thread);
                fetching.postValue(Resource.success(new Object()));
            }

            @Override
            public void onFailure(@NonNull final Call<DirectThreadFeedResponse> call, @NonNull final Throwable t) {
                Log.e(TAG, "Failed fetching dm chats", t);
                fetching.postValue(Resource.error(t.getMessage(), null));
                hasOlder = false;
            }
        });
        if (cursor == null) {
            fetchPendingRequests();
        }
    }

    public void fetchPendingRequests() {
        final Boolean isGroup = this.isGroup.getValue();
        if (isGroup == null || !isGroup) return;
        final Call<DirectThreadParticipantRequestsResponse> request = service.participantRequests(threadId, 1, null);
        request.enqueue(new Callback<DirectThreadParticipantRequestsResponse>() {

            @Override
            public void onResponse(@NonNull final Call<DirectThreadParticipantRequestsResponse> call,
                                   @NonNull final Response<DirectThreadParticipantRequestsResponse> response) {
                if (!response.isSuccessful()) {
                    if (response.errorBody() != null) {
                        try {
                            final String string = response.errorBody().string();
                            final String msg = String.format(Locale.US,
                                                             "onResponse: url: %s, responseCode: %d, errorBody: %s",
                                                             call.request().url().toString(),
                                                             response.code(),
                                                             string);
                            Log.e(TAG, msg);
                        } catch (IOException e) {
                            Log.e(TAG, "onResponse: ", e);
                        }
                        return;
                    }
                    Log.e(TAG, "onResponse: request was not successful and response error body was null");
                    return;
                }
                final DirectThreadParticipantRequestsResponse body = response.body();
                if (body == null) {
                    Log.e(TAG, "onResponse: response body was null");
                    return;
                }
                pendingRequests.postValue(body);
            }

            @Override
            public void onFailure(@NonNull final Call<DirectThreadParticipantRequestsResponse> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    private void setThread(@NonNull final DirectThread thread, final boolean skipItems) {
        // if (thread.getInputMode() != 1 && thread.isGroup() && viewerIsAdmin) {
        //     fetchPendingRequests();
        // }
        final List<DirectItem> items = thread.getItems();
        if (skipItems) {
            final DirectThread currentThread = this.thread.getValue();
            if (currentThread != null) {
                thread.setItems(currentThread.getItems());
            }
        }
        if (!skipItems && !TextUtils.isEmpty(cursor)) {
            final DirectThread currentThread = this.thread.getValue();
            if (currentThread != null) {
                List<DirectItem> list = currentThread.getItems();
                list = list == null ? new LinkedList<>() : new LinkedList<>(list);
                list.addAll(items);
                thread.setItems(list);
            }
        }
        inboxManager.setThread(threadId, thread);
    }

    private void setThread(@NonNull final DirectThread thread) {
        setThread(thread, false);
    }

    private void setThreadUsers(final List<User> users, final List<User> leftUsers) {
        final DirectThread currentThread = this.thread.getValue();
        if (currentThread == null) return;
        final DirectThread thread;
        try {
            thread = (DirectThread) currentThread.clone();
        } catch (CloneNotSupportedException e) {
            Log.e(TAG, "setThreadUsers: ", e);
            return;
        }
        if (users != null) {
            thread.setUsers(users);
        }
        if (leftUsers != null) {
            thread.setLeftUsers(leftUsers);
        }
        inboxManager.setThread(threadId, thread);
    }

    private void addItems(final int index, final Collection<DirectItem> items) {
        if (items == null) return;
        inboxManager.addItemsToThread(threadId, index, items);
    }

    private void addReaction(final DirectItem item, final Emoji emoji) {
        if (item == null || emoji == null || currentUser == null) return;
        final boolean isLike = emoji.getUnicode().equals("❤️");
        DirectItemReactions reactions = item.getReactions();
        if (reactions == null) {
            reactions = new DirectItemReactions(null, null);
        } else {
            try {
                reactions = (DirectItemReactions) reactions.clone();
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "addReaction: ", e);
                return;
            }
        }
        if (isLike) {
            final List<DirectItemEmojiReaction> likes = addEmoji(reactions.getLikes(), null, false);
            reactions.setLikes(likes);
        }
        final List<DirectItemEmojiReaction> emojis = addEmoji(reactions.getEmojis(), emoji.getUnicode(), true);
        reactions.setEmojis(emojis);
        List<DirectItem> list = this.items.getValue();
        list = list == null ? new LinkedList<>() : new LinkedList<>(list);
        int index = getItemIndex(item, list);
        if (index >= 0) {
            try {
                final DirectItem clone = (DirectItem) list.get(index).clone();
                clone.setReactions(reactions);
                list.set(index, clone);
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "addReaction: error cloning", e);
            }
        }
        inboxManager.setItemsToThread(threadId, list);
    }

    private void removeReaction(final DirectItem item) {
        try {
            final DirectItem itemClone = (DirectItem) item.clone();
            final DirectItemReactions reactions = itemClone.getReactions();
            final DirectItemReactions reactionsClone = (DirectItemReactions) reactions.clone();
            final List<DirectItemEmojiReaction> likes = reactionsClone.getLikes();
            if (likes != null) {
                final List<DirectItemEmojiReaction> updatedLikes = likes.stream()
                                                                        .filter(like -> like.getSenderId() != viewerId)
                                                                        .collect(Collectors.toList());
                reactionsClone.setLikes(updatedLikes);
            }
            final List<DirectItemEmojiReaction> emojis = reactionsClone.getEmojis();
            if (emojis != null) {
                final List<DirectItemEmojiReaction> updatedEmojis = emojis.stream()
                                                                          .filter(emoji -> emoji.getSenderId() != viewerId)
                                                                          .collect(Collectors.toList());
                reactionsClone.setEmojis(updatedEmojis);
            }
            itemClone.setReactions(reactionsClone);
            List<DirectItem> list = this.items.getValue();
            list = list == null ? new LinkedList<>() : new LinkedList<>(list);
            int index = getItemIndex(item, list);
            if (index >= 0) {
                list.set(index, itemClone);
            }
            inboxManager.setItemsToThread(threadId, list);
        } catch (Exception e) {
            Log.e(TAG, "removeReaction: ", e);
        }
    }

    private int removeItem(final DirectItem item) {
        if (item == null) return 0;
        List<DirectItem> list = this.items.getValue();
        list = list == null ? new LinkedList<>() : new LinkedList<>(list);
        int index = getItemIndex(item, list);
        if (index >= 0) {
            list.remove(index);
            inboxManager.setItemsToThread(threadId, list);
        }
        return index;
    }

    private List<DirectItemEmojiReaction> addEmoji(final List<DirectItemEmojiReaction> reactionList,
                                                   final String emoji,
                                                   final boolean shouldReplaceIfAlreadyReacted) {
        if (currentUser == null) return reactionList;
        final List<DirectItemEmojiReaction> temp = reactionList == null ? new ArrayList<>() : new ArrayList<>(reactionList);
        int index = -1;
        for (int i = 0; i < temp.size(); i++) {
            final DirectItemEmojiReaction directItemEmojiReaction = temp.get(i);
            if (directItemEmojiReaction.getSenderId() == currentUser.getPk()) {
                index = i;
                break;
            }
        }
        final DirectItemEmojiReaction reaction = new DirectItemEmojiReaction(
                currentUser.getPk(),
                System.currentTimeMillis() * 1000,
                emoji,
                "none"
        );
        if (index < 0) {
            temp.add(0, reaction);
        } else if (shouldReplaceIfAlreadyReacted) {
            temp.set(index, reaction);
        }
        return temp;
    }

    public LiveData<Resource<Object>> sendText(final String text) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final Long userId = getCurrentUserId(data);
        if (userId == null) return data;
        final String clientContext = UUID.randomUUID().toString();
        final DirectItem replyToItemValue = replyToItem.getValue();
        final DirectItem directItem = DirectItemFactory.createText(userId, clientContext, text, replyToItemValue);
        // Log.d(TAG, "sendText: sending: itemId: " + directItem.getItemId());
        directItem.setPending(true);
        addItems(0, Collections.singletonList(directItem));
        data.postValue(Resource.loading(directItem));
        final String repliedToItemId = replyToItemValue != null ? replyToItemValue.getItemId() : null;
        final String repliedToClientContext = replyToItemValue != null ? replyToItemValue.getClientContext() : null;
        final Call<DirectThreadBroadcastResponse> request = service.broadcastText(
                clientContext,
                threadIdOrUserIds,
                text,
                repliedToItemId,
                repliedToClientContext
        );
        enqueueRequest(request, data, directItem);
        return data;
    }

    public LiveData<Resource<Object>> sendUri(final MediaController.MediaEntry entry) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (entry == null) {
            data.postValue(Resource.error("Entry is null", null));
            return data;
        }
        final Uri uri = Uri.fromFile(new File(entry.path));
        if (!entry.isVideo) {
            sendPhoto(data, uri, entry.width, entry.height);
            return data;
        }
        sendVideo(data, uri, entry.size, entry.duration, entry.width, entry.height);
        return data;
    }

    public LiveData<Resource<Object>> sendUri(final Uri uri) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (uri == null) {
            data.postValue(Resource.error("Uri is null", null));
            return data;
        }
        final String mimeType = Utils.getMimeType(uri, contentResolver);
        if (TextUtils.isEmpty(mimeType)) {
            data.postValue(Resource.error("Unknown MediaType", null));
            return data;
        }
        final boolean isPhoto = mimeType.startsWith("image");
        if (isPhoto) {
            sendPhoto(data, uri);
            return data;
        }
        if (mimeType.startsWith("video")) {
            sendVideo(data, uri);
        }
        return data;
    }

    public LiveData<Resource<Object>> sendAnimatedMedia(@NonNull final GiphyGif giphyGif) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final Long userId = getCurrentUserId(data);
        if (userId == null) return data;
        final String clientContext = UUID.randomUUID().toString();
        final DirectItem directItem = DirectItemFactory.createAnimatedMedia(userId, clientContext, giphyGif);
        directItem.setPending(true);
        addItems(0, Collections.singletonList(directItem));
        data.postValue(Resource.loading(directItem));
        final Call<DirectThreadBroadcastResponse> request = service.broadcastAnimatedMedia(
                clientContext,
                threadIdOrUserIds,
                giphyGif
        );
        enqueueRequest(request, data, directItem);
        return data;
    }

    public void sendVoice(@NonNull final MutableLiveData<Resource<Object>> data,
                          @NonNull final Uri uri,
                          @NonNull final List<Float> waveform,
                          final int samplingFreq,
                          final long duration,
                          final long byteLength) {
        if (duration > 60000) {
            // instagram does not allow uploading audio longer than 60 secs for Direct messages
            data.postValue(Resource.error(R.string.dms_ERROR_AUDIO_TOO_LONG, null));
            return;
        }
        final Long userId = getCurrentUserId(data);
        if (userId == null) return;
        final String clientContext = UUID.randomUUID().toString();
        final DirectItem directItem = DirectItemFactory.createVoice(userId, clientContext, uri, duration, waveform, samplingFreq);
        directItem.setPending(true);
        addItems(0, Collections.singletonList(directItem));
        data.postValue(Resource.loading(directItem));
        final UploadVideoOptions uploadDmVoiceOptions = MediaUploadHelper.createUploadDmVoiceOptions(byteLength, duration);
        MediaUploader.uploadVideo(uri, contentResolver, uploadDmVoiceOptions, new MediaUploader.OnMediaUploadCompleteListener() {
            @Override
            public void onUploadComplete(final MediaUploader.MediaUploadResponse response) {
                // Log.d(TAG, "onUploadComplete: " + response);
                if (handleInvalidResponse(data, response)) return;
                final UploadFinishOptions uploadFinishOptions = new UploadFinishOptions()
                        .setUploadId(uploadDmVoiceOptions.getUploadId())
                        .setSourceType("4");
                final Call<String> uploadFinishRequest = mediaService.uploadFinish(uploadFinishOptions);
                uploadFinishRequest.enqueue(new Callback<String>() {
                    @Override
                    public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                        if (response.isSuccessful()) {
                            final Call<DirectThreadBroadcastResponse> request = service.broadcastVoice(
                                    clientContext,
                                    threadIdOrUserIds,
                                    uploadDmVoiceOptions.getUploadId(),
                                    waveform,
                                    samplingFreq
                            );
                            enqueueRequest(request, data, directItem);
                            return;
                        }
                        if (response.errorBody() != null) {
                            handleErrorBody(call, response, data);
                            return;
                        }
                        data.postValue(Resource.error("uploadFinishRequest was not successful and response error body was null", directItem));
                        Log.e(TAG, "uploadFinishRequest was not successful and response error body was null");
                    }

                    @Override
                    public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                        data.postValue(Resource.error(t.getMessage(), directItem));
                        Log.e(TAG, "onFailure: ", t);
                    }
                });
            }

            @Override
            public void onFailure(final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), directItem));
                Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    @NonNull
    public LiveData<Resource<Object>> sendReaction(final DirectItem item, final Emoji emoji) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final Long userId = getCurrentUserId(data);
        if (userId == null) {
            data.postValue(Resource.error("userId is null", null));
            return data;
        }
        final String clientContext = UUID.randomUUID().toString();
        // Log.d(TAG, "sendText: sending: itemId: " + directItem.getItemId());
        data.postValue(Resource.loading(item));
        addReaction(item, emoji);
        String emojiUnicode = null;
        if (!emoji.getUnicode().equals("❤️")) {
            emojiUnicode = emoji.getUnicode();
        }
        final Call<DirectThreadBroadcastResponse> request = service.broadcastReaction(
                clientContext, threadIdOrUserIds, item.getItemId(), emojiUnicode, false);
        handleBroadcastReactionRequest(data, item, request);
        return data;
    }

    public LiveData<Resource<Object>> sendDeleteReaction(final String itemId) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final DirectItem item = getItem(itemId);
        if (item == null) {
            data.postValue(Resource.error("Invalid item", null));
            return data;
        }
        final DirectItemReactions reactions = item.getReactions();
        if (reactions == null) {
            // already removed?
            data.postValue(Resource.success(item));
            return data;
        }
        removeReaction(item);
        final String clientContext = UUID.randomUUID().toString();
        final Call<DirectThreadBroadcastResponse> request = service.broadcastReaction(clientContext, threadIdOrUserIds, item.getItemId(), null, true);
        handleBroadcastReactionRequest(data, item, request);
        return data;
    }

    public LiveData<Resource<Object>> unsend(final DirectItem item) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (item == null) {
            data.postValue(Resource.error("item is null", null));
            return data;
        }
        final int index = removeItem(item);
        final Call<String> request = service.deleteItem(threadId, item.getItemId());
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (response.isSuccessful()) {
                    // Log.d(TAG, "onResponse: " + response.body());
                    return;
                }
                // add the item back if unsuccessful
                addItems(index, Collections.singletonList(item));
                if (response.errorBody() != null) {
                    handleErrorBody(call, response, data);
                    return;
                }
                data.postValue(Resource.error(R.string.generic_failed_request, item));
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), item));
                Log.e(TAG, "enqueueRequest: onFailure: ", t);
            }
        });
        return data;
    }

    public void forward(final Set<RankedRecipient> recipients, final DirectItem itemToForward) {
        if (recipients == null || itemToForward == null) return;
        for (final RankedRecipient recipient : recipients) {
            forward(recipient, itemToForward);
        }
    }

    public void forward(final RankedRecipient recipient, final DirectItem itemToForward) {
        if (recipient == null || itemToForward == null) return;
        if (recipient.getThread() == null && recipient.getUser() != null) {
            // create thread and forward
            final Call<DirectThread> createThreadRequest = service.createThread(Collections.singletonList(recipient.getUser().getPk()), null);
            createThreadRequest.enqueue(new Callback<DirectThread>() {
                @Override
                public void onResponse(@NonNull final Call<DirectThread> call, @NonNull final Response<DirectThread> response) {
                    if (!response.isSuccessful()) {
                        if (response.errorBody() != null) {
                            try {
                                final String string = response.errorBody().string();
                                final String msg = String.format(Locale.US,
                                                                 "onResponse: url: %s, responseCode: %d, errorBody: %s",
                                                                 call.request().url().toString(),
                                                                 response.code(),
                                                                 string);
                                Log.e(TAG, msg);
                            } catch (IOException e) {
                                Log.e(TAG, "onResponse: ", e);
                            }
                            return;
                        }
                        Log.e(TAG, "onResponse: request was not successful and response error body was null");
                        return;
                    }
                    final DirectThread thread = response.body();
                    if (thread == null) {
                        Log.e(TAG, "onResponse: thread is null");
                        return;
                    }
                    forward(thread, itemToForward);
                }

                @Override
                public void onFailure(@NonNull final Call<DirectThread> call, @NonNull final Throwable t) {

                }
            });
            return;
        }
        if (recipient.getThread() != null) {
            // just forward
            final DirectThread thread = recipient.getThread();
            forward(thread, itemToForward);
        }
    }

    public void setReplyToItem(final DirectItem item) {
        // Log.d(TAG, "setReplyToItem: " + item);
        replyToItem.postValue(item);
    }

    private void forward(@NonNull final DirectThread thread, @NonNull final DirectItem itemToForward) {
        final DirectItemType itemType = itemToForward.getItemType();
        final String itemTypeName = itemType.getName();
        if (itemTypeName == null) {
            Log.e(TAG, "forward: itemTypeName was null!");
            return;
        }
        final Call<DirectThreadBroadcastResponse> request = service.forward(thread.getThreadId(),
                                                                            itemTypeName,
                                                                            threadId,
                                                                            itemToForward.getItemId());
        request.enqueue(new Callback<DirectThreadBroadcastResponse>() {
            @Override
            public void onResponse(@NonNull final Call<DirectThreadBroadcastResponse> call,
                                   @NonNull final Response<DirectThreadBroadcastResponse> response) {
                if (response.isSuccessful()) return;
                if (response.errorBody() != null) {
                    try {
                        final String string = response.errorBody().string();
                        final String msg = String.format(Locale.US,
                                                         "onResponse: url: %s, responseCode: %d, errorBody: %s",
                                                         call.request().url().toString(),
                                                         response.code(),
                                                         string);
                        Log.e(TAG, msg);
                    } catch (IOException e) {
                        Log.e(TAG, "onResponse: ", e);
                    }
                    return;
                }
                Log.e(TAG, "onResponse: request was not successful and response error body was null");
            }

            @Override
            public void onFailure(@NonNull final Call<DirectThreadBroadcastResponse> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    public LiveData<Resource<Object>> acceptRequest() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final Call<String> request = service.approveRequest(threadId);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call,
                                   @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    try {
                        final String string = response.errorBody() != null ? response.errorBody().string() : "";
                        final String msg = String.format(Locale.US,
                                                         "onResponse: url: %s, responseCode: %d, errorBody: %s",
                                                         call.request().url().toString(),
                                                         response.code(),
                                                         string);
                        Log.e(TAG, msg);
                        data.postValue(Resource.error(msg, null));
                        return;
                    } catch (IOException e) {
                        Log.e(TAG, "onResponse: ", e);
                    }
                    return;
                }
                data.postValue(Resource.success(new Object()));
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> declineRequest() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final Call<String> request = service.declineRequest(threadId);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call,
                                   @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    try {
                        final String string = response.errorBody() != null ? response.errorBody().string() : "";
                        final String msg = String.format(Locale.US,
                                                         "onResponse: url: %s, responseCode: %d, errorBody: %s",
                                                         call.request().url().toString(),
                                                         response.code(),
                                                         string);
                        Log.e(TAG, msg);
                        data.postValue(Resource.error(msg, null));
                        return;
                    } catch (IOException e) {
                        Log.e(TAG, "onResponse: ", e);
                    }
                    return;
                }
                data.postValue(Resource.success(new Object()));
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public void refreshChats() {
        final Resource<Object> isFetching = fetching.getValue();
        if (isFetching != null && isFetching.status == Status.LOADING) {
            stopCurrentRequest();
        }
        cursor = null;
        hasOlder = true;
        fetchChats();
    }

    private void sendPhoto(@NonNull final MutableLiveData<Resource<Object>> data,
                           @NonNull final Uri uri) {
        try {
            final Pair<Integer, Integer> dimensions = BitmapUtils.decodeDimensions(contentResolver, uri);
            if (dimensions == null) {
                data.postValue(Resource.error("Decoding dimensions failed", null));
                return;
            }
            sendPhoto(data, uri, dimensions.first, dimensions.second);
        } catch (FileNotFoundException e) {
            data.postValue(Resource.error(e.getMessage(), null));
            Log.e(TAG, "sendPhoto: ", e);
        }
    }

    private void sendPhoto(@NonNull final MutableLiveData<Resource<Object>> data,
                           @NonNull final Uri uri,
                           final int width,
                           final int height) {
        final Long userId = getCurrentUserId(data);
        if (userId == null) return;
        final String clientContext = UUID.randomUUID().toString();
        final DirectItem directItem = DirectItemFactory.createImageOrVideo(userId, clientContext, uri, width, height, false);
        directItem.setPending(true);
        addItems(0, Collections.singletonList(directItem));
        data.postValue(Resource.loading(directItem));
        MediaUploader.uploadPhoto(uri, contentResolver, new MediaUploader.OnMediaUploadCompleteListener() {
            @Override
            public void onUploadComplete(final MediaUploader.MediaUploadResponse response) {
                if (handleInvalidResponse(data, response)) return;
                final String uploadId = response.getResponse().optString("upload_id");
                final Call<DirectThreadBroadcastResponse> request = service.broadcastPhoto(clientContext, threadIdOrUserIds, uploadId);
                enqueueRequest(request, data, directItem);
            }

            @Override
            public void onFailure(final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), directItem));
                Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    private void sendVideo(@NonNull final MutableLiveData<Resource<Object>> data,
                           @NonNull final Uri uri) {
        MediaUtils.getVideoInfo(contentResolver, uri, new MediaUtils.OnInfoLoadListener<MediaUtils.VideoInfo>() {
            @Override
            public void onLoad(@Nullable final MediaUtils.VideoInfo info) {
                if (info == null) {
                    data.postValue(Resource.error("Could not get the video info", null));
                    return;
                }
                sendVideo(data, uri, info.size, info.duration, info.width, info.height);
            }

            @Override
            public void onFailure(final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
    }

    private void sendVideo(@NonNull final MutableLiveData<Resource<Object>> data,
                           @NonNull final Uri uri,
                           final long byteLength,
                           final long duration,
                           final int width,
                           final int height) {
        if (duration > 60000) {
            // instagram does not allow uploading videos longer than 60 secs for Direct messages
            data.postValue(Resource.error(R.string.dms_ERROR_VIDEO_TOO_LONG, null));
            return;
        }
        final Long userId = getCurrentUserId(data);
        if (userId == null) return;
        final String clientContext = UUID.randomUUID().toString();
        final DirectItem directItem = DirectItemFactory.createImageOrVideo(userId, clientContext, uri, width, height, true);
        directItem.setPending(true);
        addItems(0, Collections.singletonList(directItem));
        data.postValue(Resource.loading(directItem));
        final UploadVideoOptions uploadDmVideoOptions = MediaUploadHelper.createUploadDmVideoOptions(byteLength, duration, width, height);
        MediaUploader.uploadVideo(uri, contentResolver, uploadDmVideoOptions, new MediaUploader.OnMediaUploadCompleteListener() {
            @Override
            public void onUploadComplete(final MediaUploader.MediaUploadResponse response) {
                // Log.d(TAG, "onUploadComplete: " + response);
                if (handleInvalidResponse(data, response)) return;
                final UploadFinishOptions uploadFinishOptions = new UploadFinishOptions()
                        .setUploadId(uploadDmVideoOptions.getUploadId())
                        .setSourceType("2")
                        .setVideoOptions(new UploadFinishOptions.VideoOptions().setLength(duration / 1000f));
                final Call<String> uploadFinishRequest = mediaService.uploadFinish(uploadFinishOptions);
                uploadFinishRequest.enqueue(new Callback<String>() {
                    @Override
                    public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                        if (response.isSuccessful()) {
                            final Call<DirectThreadBroadcastResponse> request = service.broadcastVideo(
                                    clientContext,
                                    threadIdOrUserIds,
                                    uploadDmVideoOptions.getUploadId(),
                                    "",
                                    true
                            );
                            enqueueRequest(request, data, directItem);
                            return;
                        }
                        if (response.errorBody() != null) {
                            handleErrorBody(call, response, data);
                            return;
                        }
                        data.postValue(Resource.error("uploadFinishRequest was not successful and response error body was null", directItem));
                        Log.e(TAG, "uploadFinishRequest was not successful and response error body was null");
                    }

                    @Override
                    public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                        data.postValue(Resource.error(t.getMessage(), directItem));
                        Log.e(TAG, "onFailure: ", t);
                    }
                });
            }

            @Override
            public void onFailure(final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), directItem));
                Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    private void enqueueRequest(@NonNull final Call<DirectThreadBroadcastResponse> request,
                                @NonNull final MutableLiveData<Resource<Object>> data,
                                @NonNull final DirectItem directItem) {
        request.enqueue(new Callback<DirectThreadBroadcastResponse>() {
            @Override
            public void onResponse(@NonNull final Call<DirectThreadBroadcastResponse> call,
                                   @NonNull final Response<DirectThreadBroadcastResponse> response) {
                if (response.isSuccessful()) {
                    final DirectThreadBroadcastResponse broadcastResponse = response.body();
                    if (broadcastResponse == null) {
                        data.postValue(Resource.error(R.string.generic_null_response, directItem));
                        Log.e(TAG, "enqueueRequest: onResponse: response body is null");
                        return;
                    }
                    final String payloadClientContext;
                    final long timestamp;
                    final String itemId;
                    final DirectThreadBroadcastResponsePayload payload = broadcastResponse.getPayload();
                    if (payload == null) {
                        final List<DirectThreadBroadcastResponseMessageMetadata> messageMetadata = broadcastResponse.getMessageMetadata();
                        if (messageMetadata == null || messageMetadata.isEmpty()) {
                            data.postValue(Resource.success(directItem));
                            return;
                        }
                        final DirectThreadBroadcastResponseMessageMetadata metadata = messageMetadata.get(0);
                        payloadClientContext = metadata.getClientContext();
                        itemId = metadata.getItemId();
                        timestamp = metadata.getTimestamp();
                    } else {
                        payloadClientContext = payload.getClientContext();
                        timestamp = payload.getTimestamp();
                        itemId = payload.getItemId();
                    }
                    updateItemSent(payloadClientContext, timestamp, itemId);
                    data.postValue(Resource.success(directItem));
                    return;
                }
                if (response.errorBody() != null) {
                    handleErrorBody(call, response, data);
                }
                data.postValue(Resource.error(R.string.generic_failed_request, directItem));
            }

            @Override
            public void onFailure(@NonNull final Call<DirectThreadBroadcastResponse> call,
                                  @NonNull final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), directItem));
                Log.e(TAG, "enqueueRequest: onFailure: ", t);
            }
        });
    }

    private void updateItemSent(final String clientContext, final long timestamp, final String itemId) {
        if (clientContext == null) return;
        List<DirectItem> list = this.items.getValue();
        list = list == null ? new LinkedList<>() : new LinkedList<>(list);
        final int index = Iterables.indexOf(list, item -> {
            if (item == null) return false;
            return item.getClientContext().equals(clientContext);
        });
        if (index < 0) return;
        final DirectItem directItem = list.get(index);
        try {
            final DirectItem itemClone = (DirectItem) directItem.clone();
            itemClone.setItemId(itemId);
            itemClone.setPending(false);
            itemClone.setTimestamp(timestamp);
            list.set(index, itemClone);
            inboxManager.setItemsToThread(threadId, list);
        } catch (CloneNotSupportedException e) {
            Log.e(TAG, "updateItemSent: ", e);
        }
    }

    private void handleErrorBody(@NonNull final Call<?> call,
                                 @NonNull final Response<?> response,
                                 final MutableLiveData<Resource<Object>> data) {
        try {
            final String string = response.errorBody() != null ? response.errorBody().string() : "";
            final String msg = String.format(Locale.US,
                                             "onResponse: url: %s, responseCode: %d, errorBody: %s",
                                             call.request().url().toString(),
                                             response.code(),
                                             string);
            if (data != null) {
                data.postValue(Resource.error(msg, null));
            }
            Log.e(TAG, msg);
        } catch (IOException e) {
            if (data != null) {
                data.postValue(Resource.error(e.getMessage(), null));
            }
            Log.e(TAG, "onResponse: ", e);
        }
    }

    private boolean handleInvalidResponse(final MutableLiveData<Resource<Object>> data,
                                          @NonNull final MediaUploader.MediaUploadResponse response) {
        final JSONObject responseJson = response.getResponse();
        if (responseJson == null || response.getResponseCode() != HttpURLConnection.HTTP_OK) {
            data.postValue(Resource.error(R.string.generic_not_ok_response, null));
            return true;
        }
        final String status = responseJson.optString("status");
        if (TextUtils.isEmpty(status) || !status.equals("ok")) {
            data.postValue(Resource.error(R.string.generic_not_ok_response, null));
            return true;
        }
        return false;
    }

    private int getItemIndex(final DirectItem item, final List<DirectItem> list) {
        return Iterables.indexOf(list, i -> i != null && i.getItemId().equals(item.getItemId()));
    }

    @Nullable
    private DirectItem getItem(final String itemId) {
        if (itemId == null) return null;
        final List<DirectItem> items = this.items.getValue();
        if (items == null) return null;
        return items.stream()
                    .filter(directItem -> directItem.getItemId().equals(itemId))
                    .findFirst()
                    .orElse(null);
    }

    private void handleBroadcastReactionRequest(final MutableLiveData<Resource<Object>> data,
                                                final DirectItem item,
                                                @NonNull final Call<DirectThreadBroadcastResponse> request) {
        request.enqueue(new Callback<DirectThreadBroadcastResponse>() {
            @Override
            public void onResponse(@NonNull final Call<DirectThreadBroadcastResponse> call,
                                   @NonNull final Response<DirectThreadBroadcastResponse> response) {
                if (!response.isSuccessful()) {
                    if (response.errorBody() != null) {
                        handleErrorBody(call, response, data);
                        return;
                    }
                    data.postValue(Resource.error(R.string.generic_failed_request, item));
                    return;
                }
                final DirectThreadBroadcastResponse body = response.body();
                if (body == null) {
                    data.postValue(Resource.error(R.string.generic_null_response, item));
                }
                // otherwise nothing to do? maybe update the timestamp in the emoji?
            }

            @Override
            public void onFailure(@NonNull final Call<DirectThreadBroadcastResponse> call, @NonNull final Throwable t) {
                data.postValue(Resource.error(t.getMessage(), item));
                Log.e(TAG, "enqueueRequest: onFailure: ", t);
            }
        });
    }

    private void stopCurrentRequest() {
        if (chatsRequest == null || chatsRequest.isExecuted() || chatsRequest.isCanceled()) {
            return;
        }
        chatsRequest.cancel();
        fetching.postValue(Resource.success(new Object()));
    }

    @Nullable
    private Long getCurrentUserId(final MutableLiveData<Resource<Object>> data) {
        if (currentUser == null || currentUser.getPk() <= 0) {
            data.postValue(Resource.error(R.string.dms_ERROR_INVALID_USER, null));
            return null;
        }
        return currentUser.getPk();
    }

    public void removeThread() {
        final Boolean pendingValue = pending.getValue();
        final boolean threadInPending = pendingValue != null && pendingValue;
        inboxManager.removeThread(threadId);
        if (threadInPending) {
            final Integer totalValue = inboxManager.getPendingRequestsTotal().getValue();
            if (totalValue == null) return;
            inboxManager.setPendingRequestsTotal(totalValue - 1);
        }
    }

    public LiveData<Resource<Object>> updateTitle(final String newTitle) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final Call<DirectThreadDetailsChangeResponse> addUsersRequest = service.updateTitle(threadId, newTitle.trim());
        handleDetailsChangeRequest(data, addUsersRequest);
        return data;
    }

    public LiveData<Resource<Object>> addMembers(final Set<User> users) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        final Call<DirectThreadDetailsChangeResponse> addUsersRequest = service.addUsers(threadId,
                                                                                         users.stream()
                                                                                              .filter(Objects::nonNull)
                                                                                              .map(User::getPk)
                                                                                              .collect(Collectors.toList()));
        handleDetailsChangeRequest(data, addUsersRequest);
        return data;
    }

    public LiveData<Resource<Object>> removeMember(final User user) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (user == null) {
            data.postValue(Resource.error("user is null!", null));
            return data;
        }
        final Call<String> request = service.removeUsers(threadId, Collections.singleton(user.getPk()));
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                data.postValue(Resource.success(new Object()));
                List<User> activeUsers = users.getValue();
                List<User> leftUsersValue = leftUsers.getValue();
                if (activeUsers == null) {
                    activeUsers = Collections.emptyList();
                }
                if (leftUsersValue == null) {
                    leftUsersValue = Collections.emptyList();
                }
                final List<User> updatedActiveUsers = activeUsers.stream()
                                                                 .filter(Objects::nonNull)
                                                                 .filter(u -> u.getPk() != user.getPk())
                                                                 .collect(Collectors.toList());
                final ImmutableList.Builder<User> updatedLeftUsersBuilder = ImmutableList.<User>builder().addAll(leftUsersValue);
                if (!leftUsersValue.contains(user)) {
                    updatedLeftUsersBuilder.add(user);
                }
                final ImmutableList<User> updatedLeftUsers = updatedLeftUsersBuilder.build();
                setThreadUsers(updatedActiveUsers, updatedLeftUsers);
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public boolean isAdmin(final User user) {
        if (user == null) return false;
        final List<Long> adminUserIdsValue = adminUserIds.getValue();
        return adminUserIdsValue != null && adminUserIdsValue.contains(user.getPk());
    }

    public LiveData<Resource<Object>> makeAdmin(final User user) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (user == null) return data;
        if (isAdmin(user)) return data;
        final Call<String> request = service.addAdmins(threadId, Collections.singleton(user.getPk()));
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                final List<Long> currentAdminIds = adminUserIds.getValue();
                final ImmutableList<Long> updatedAdminIds = ImmutableList.<Long>builder()
                        .addAll(currentAdminIds != null ? currentAdminIds : Collections.emptyList())
                        .add(user.getPk())
                        .build();
                final DirectThread currentThread = ThreadManager.this.thread.getValue();
                if (currentThread == null) return;
                try {
                    final DirectThread thread = (DirectThread) currentThread.clone();
                    thread.setAdminUserIds(updatedAdminIds);
                    inboxManager.setThread(threadId, thread);
                } catch (CloneNotSupportedException e) {
                    Log.e(TAG, "onResponse: ", e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> removeAdmin(final User user) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (user == null) return data;
        if (!isAdmin(user)) return data;
        final Call<String> request = service.removeAdmins(threadId, Collections.singleton(user.getPk()));
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                final List<Long> currentAdmins = adminUserIds.getValue();
                if (currentAdmins == null) return;
                final List<Long> updatedAdminUserIds = currentAdmins.stream()
                                                                    .filter(Objects::nonNull)
                                                                    .filter(userId1 -> userId1 != user.getPk())
                                                                    .collect(Collectors.toList());
                final DirectThread currentThread = ThreadManager.this.thread.getValue();
                if (currentThread == null) return;
                try {
                    final DirectThread thread = (DirectThread) currentThread.clone();
                    thread.setAdminUserIds(updatedAdminUserIds);
                    inboxManager.setThread(threadId, thread);
                } catch (CloneNotSupportedException e) {
                    Log.e(TAG, "onResponse: ", e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> mute() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Boolean muted = isMuted.getValue();
        if (muted != null && muted) {
            data.postValue(Resource.success(new Object()));
            return data;
        }
        final Call<String> request = service.mute(threadId);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                data.postValue(Resource.success(new Object()));
                final DirectThread currentThread = ThreadManager.this.thread.getValue();
                if (currentThread == null) return;
                try {
                    final DirectThread thread = (DirectThread) currentThread.clone();
                    thread.setMuted(true);
                    inboxManager.setThread(threadId, thread);
                } catch (CloneNotSupportedException e) {
                    Log.e(TAG, "onResponse: ", e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> unmute() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Boolean muted = isMuted.getValue();
        if (muted != null && !muted) {
            data.postValue(Resource.success(new Object()));
            return data;
        }
        final Call<String> request = service.unmute(threadId);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                data.postValue(Resource.success(new Object()));
                final DirectThread currentThread = ThreadManager.this.thread.getValue();
                if (currentThread == null) return;
                try {
                    final DirectThread thread = (DirectThread) currentThread.clone();
                    thread.setMuted(false);
                    inboxManager.setThread(threadId, thread);
                } catch (CloneNotSupportedException e) {
                    Log.e(TAG, "onResponse: ", e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> muteMentions() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Boolean mentionsMuted = isMentionsMuted.getValue();
        if (mentionsMuted != null && mentionsMuted) {
            data.postValue(Resource.success(new Object()));
            return data;
        }
        final Call<String> request = service.muteMentions(threadId);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                data.postValue(Resource.success(new Object()));
                final DirectThread currentThread = ThreadManager.this.thread.getValue();
                if (currentThread == null) return;
                try {
                    final DirectThread thread = (DirectThread) currentThread.clone();
                    thread.setMentionsMuted(true);
                    inboxManager.setThread(threadId, thread);
                } catch (CloneNotSupportedException e) {
                    Log.e(TAG, "onResponse: ", e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> unmuteMentions() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Boolean mentionsMuted = isMentionsMuted.getValue();
        if (mentionsMuted != null && !mentionsMuted) {
            data.postValue(Resource.success(new Object()));
            return data;
        }
        final Call<String> request = service.unmuteMentions(threadId);
        request.enqueue(new Callback<String>() {
            @Override
            public void onResponse(@NonNull final Call<String> call, @NonNull final Response<String> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                data.postValue(Resource.success(new Object()));
                final DirectThread currentThread = ThreadManager.this.thread.getValue();
                if (currentThread == null) return;
                try {
                    final DirectThread thread = (DirectThread) currentThread.clone();
                    thread.setMentionsMuted(false);
                    inboxManager.setThread(threadId, thread);
                } catch (CloneNotSupportedException e) {
                    Log.e(TAG, "onResponse: ", e);
                }
            }

            @Override
            public void onFailure(@NonNull final Call<String> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> blockUser(final User user) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (user == null) return data;
        friendshipService.changeBlock(false, user.getPk(), new ServiceCallback<FriendshipChangeResponse>() {
            @Override
            public void onSuccess(final FriendshipChangeResponse result) {
                refreshChats();
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> unblockUser(final User user) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (user == null) return data;
        friendshipService.changeBlock(true, user.getPk(), new ServiceCallback<FriendshipChangeResponse>() {
            @Override
            public void onSuccess(final FriendshipChangeResponse result) {
                refreshChats();
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> restrictUser(final User user) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (user == null) return data;
        friendshipService.toggleRestrict(user.getPk(), true, new ServiceCallback<FriendshipRestrictResponse>() {
            @Override
            public void onSuccess(final FriendshipRestrictResponse result) {
                refreshChats();
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> unRestrictUser(final User user) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        if (user == null) return data;
        friendshipService.toggleRestrict(user.getPk(), false, new ServiceCallback<FriendshipRestrictResponse>() {
            @Override
            public void onSuccess(final FriendshipRestrictResponse result) {
                refreshChats();
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> approveUsers(final List<User> users) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Call<DirectThreadDetailsChangeResponse> approveUsersRequest = service
                .approveParticipantRequests(threadId,
                                            users.stream()
                                                 .filter(Objects::nonNull)
                                                 .map(User::getPk)
                                                 .collect(Collectors.toList()));
        handleDetailsChangeRequest(data, approveUsersRequest, () -> pendingUserApproveDenySuccessAction(users));
        return data;
    }

    public LiveData<Resource<Object>> denyUsers(final List<User> users) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Call<DirectThreadDetailsChangeResponse> approveUsersRequest = service
                .declineParticipantRequests(threadId,
                                            users.stream().map(User::getPk).collect(Collectors.toList()));
        handleDetailsChangeRequest(data, approveUsersRequest, () -> pendingUserApproveDenySuccessAction(users));
        return data;
    }

    private void pendingUserApproveDenySuccessAction(final List<User> users) {
        final DirectThreadParticipantRequestsResponse pendingRequestsValue = pendingRequests.getValue();
        if (pendingRequestsValue == null) return;
        final List<User> pendingUsers = pendingRequestsValue.getUsers();
        if (pendingUsers == null || pendingUsers.isEmpty()) return;
        final List<User> filtered = pendingUsers.stream()
                                                .filter(o -> !users.contains(o))
                                                .collect(Collectors.toList());
        try {
            final DirectThreadParticipantRequestsResponse clone = (DirectThreadParticipantRequestsResponse) pendingRequestsValue.clone();
            clone.setUsers(filtered);
            final int totalParticipantRequests = clone.getTotalParticipantRequests();
            clone.setTotalParticipantRequests(totalParticipantRequests > 0 ? totalParticipantRequests - 1 : 0);
            pendingRequests.postValue(clone);
        } catch (CloneNotSupportedException e) {
            Log.e(TAG, "pendingUserApproveDenySuccessAction: ", e);
        }
    }

    public LiveData<Resource<Object>> approvalRequired() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Boolean approvalRequiredToJoin = isApprovalRequiredToJoin.getValue();
        if (approvalRequiredToJoin != null && approvalRequiredToJoin) {
            data.postValue(Resource.success(new Object()));
            return data;
        }
        final Call<DirectThreadDetailsChangeResponse> request = service.approvalRequired(threadId);
        handleDetailsChangeRequest(data, request, () -> {
            final DirectThread currentThread = ThreadManager.this.thread.getValue();
            if (currentThread == null) return;
            try {
                final DirectThread thread = (DirectThread) currentThread.clone();
                thread.setApprovalRequiredForNewMembers(true);
                inboxManager.setThread(threadId, thread);
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "onResponse: ", e);
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> approvalNotRequired() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Boolean approvalRequiredToJoin = isApprovalRequiredToJoin.getValue();
        if (approvalRequiredToJoin != null && !approvalRequiredToJoin) {
            data.postValue(Resource.success(new Object()));
            return data;
        }
        final Call<DirectThreadDetailsChangeResponse> request = service.approvalNotRequired(threadId);
        handleDetailsChangeRequest(data, request, () -> {
            final DirectThread currentThread = ThreadManager.this.thread.getValue();
            if (currentThread == null) return;
            try {
                final DirectThread thread = (DirectThread) currentThread.clone();
                thread.setApprovalRequiredForNewMembers(false);
                inboxManager.setThread(threadId, thread);
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "onResponse: ", e);
            }
        });
        return data;
    }

    public LiveData<Resource<Object>> leave() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Call<DirectThreadDetailsChangeResponse> request = service.leave(threadId);
        handleDetailsChangeRequest(data, request);
        return data;
    }

    public LiveData<Resource<Object>> end() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Call<DirectThreadDetailsChangeResponse> request = service.end(threadId);
        handleDetailsChangeRequest(data, request, () -> {
            final DirectThread currentThread = ThreadManager.this.thread.getValue();
            if (currentThread == null) return;
            try {
                final DirectThread thread = (DirectThread) currentThread.clone();
                thread.setInputMode(1);
                inboxManager.setThread(threadId, thread);
            } catch (CloneNotSupportedException e) {
                Log.e(TAG, "onResponse: ", e);
            }
        });
        return data;
    }

    private void handleDetailsChangeRequest(final MutableLiveData<Resource<Object>> data,
                                            final Call<DirectThreadDetailsChangeResponse> request) {
        handleDetailsChangeRequest(data, request, null);
    }

    private void handleDetailsChangeRequest(final MutableLiveData<Resource<Object>> data,
                                            final Call<DirectThreadDetailsChangeResponse> request,
                                            @Nullable final OnSuccessAction action) {
        request.enqueue(new Callback<DirectThreadDetailsChangeResponse>() {
            @Override
            public void onResponse(@NonNull final Call<DirectThreadDetailsChangeResponse> call,
                                   @NonNull final Response<DirectThreadDetailsChangeResponse> response) {
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                final DirectThreadDetailsChangeResponse changeResponse = response.body();
                if (changeResponse == null) {
                    data.postValue(Resource.error(R.string.generic_null_response, null));
                    return;
                }
                data.postValue(Resource.success(new Object()));
                final DirectThread thread = changeResponse.getThread();
                if (thread != null) {
                    setThread(thread, true);
                }
                if (action != null) {
                    action.onSuccess();
                }
            }

            @Override
            public void onFailure(@NonNull final Call<DirectThreadDetailsChangeResponse> call, @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
    }

    public LiveData<User> getInviter() {
        return inviter;
    }

    public LiveData<Resource<Object>> markAsSeen(@NonNull final DirectItem directItem) {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        final Call<DirectItemSeenResponse> request = service.markAsSeen(threadId, directItem);
        request.enqueue(new Callback<DirectItemSeenResponse>() {
            @Override
            public void onResponse(@NonNull final Call<DirectItemSeenResponse> call,
                                   @NonNull final Response<DirectItemSeenResponse> response) {
                if (currentUser == null) return;
                if (!response.isSuccessful()) {
                    handleErrorBody(call, response, data);
                    return;
                }
                final DirectItemSeenResponse seenResponse = response.body();
                if (seenResponse == null) {
                    data.postValue(Resource.error(R.string.generic_null_response, null));
                    return;
                }
                inboxManager.fetchUnseenCount();
                final DirectItemSeenResponsePayload payload = seenResponse.getPayload();
                if (payload == null) return;
                final String timestamp = payload.getTimestamp();
                final DirectThread thread = ThreadManager.this.thread.getValue();
                if (thread == null) return;
                Map<Long, DirectThreadLastSeenAt> lastSeenAt = thread.getLastSeenAt();
                lastSeenAt = lastSeenAt == null ? new HashMap<>() : new HashMap<>(lastSeenAt);
                lastSeenAt.put(currentUser.getPk(), new DirectThreadLastSeenAt(timestamp, directItem.getItemId()));
                thread.setLastSeenAt(lastSeenAt);
                setThread(thread, true);
                data.postValue(Resource.success(new Object()));
            }

            @Override
            public void onFailure(@NonNull final Call<DirectItemSeenResponse> call,
                                  @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
                data.postValue(Resource.error(t.getMessage(), null));
            }
        });
        return data;
    }

    private interface OnSuccessAction {
        void onSuccess();
    }
}
