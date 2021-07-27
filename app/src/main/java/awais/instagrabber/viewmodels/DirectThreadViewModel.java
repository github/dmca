package awais.instagrabber.viewmodels;

import android.app.Application;
import android.content.ContentResolver;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Transformations;

import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

import awais.instagrabber.customviews.emoji.Emoji;
import awais.instagrabber.managers.DirectMessagesManager;
import awais.instagrabber.managers.InboxManager;
import awais.instagrabber.managers.ThreadManager;
import awais.instagrabber.models.Resource;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadLastSeenAt;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipient;
import awais.instagrabber.repositories.responses.giphy.GiphyGif;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.DirectoryUtils;
import awais.instagrabber.utils.MediaController;
import awais.instagrabber.utils.MediaUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.VoiceRecorder;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class DirectThreadViewModel extends AndroidViewModel {
    private static final String TAG = DirectThreadViewModel.class.getSimpleName();
    // private static final String ERROR_INVALID_THREAD = "Invalid thread";

    private final ContentResolver contentResolver;
    private final File recordingsDir;
    private final Application application;
    private final long viewerId;
    private final String threadId;
    private final User currentUser;
    private final ThreadManager threadManager;

    private VoiceRecorder voiceRecorder;

    public DirectThreadViewModel(@NonNull final Application application,
                                 @NonNull final String threadId,
                                 final boolean pending,
                                 @NonNull final User currentUser) {
        super(application);
        this.application = application;
        this.threadId = threadId;
        this.currentUser = currentUser;
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        viewerId = CookieUtils.getUserIdFromCookie(cookie);
        final String deviceUuid = settingsHelper.getString(Constants.DEVICE_UUID);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        if (TextUtils.isEmpty(csrfToken) || viewerId <= 0 || TextUtils.isEmpty(deviceUuid)) {
            throw new IllegalArgumentException("User is not logged in!");
        }
        contentResolver = application.getContentResolver();
        recordingsDir = DirectoryUtils.getOutputMediaDirectory(application, "Recordings");
        final DirectMessagesManager messagesManager = DirectMessagesManager.getInstance();
        threadManager = messagesManager.getThreadManager(threadId, pending, currentUser, contentResolver);
        threadManager.fetchPendingRequests();
    }

    public void moveFromPending() {
        DirectMessagesManager.getInstance().moveThreadFromPending(threadId);
        threadManager.moveFromPending();
    }

    public void removeThread() {
        threadManager.removeThread();
    }

    public String getThreadId() {
        return threadId;
    }

    public LiveData<String> getThreadTitle() {
        return threadManager.getThreadTitle();
    }

    public LiveData<DirectThread> getThread() {
        return threadManager.getThread();
    }

    public LiveData<List<DirectItem>> getItems() {
        return Transformations.map(threadManager.getItems(), items -> items.stream()
                                                                           .filter(directItem -> directItem.getHideInThread() == 0)
                                                                           .collect(Collectors.toList()));
    }

    public LiveData<Resource<Object>> isFetching() {
        return threadManager.isFetching();
    }

    public LiveData<List<User>> getUsers() {
        return threadManager.getUsers();
    }

    public LiveData<List<User>> getLeftUsers() {
        return threadManager.getLeftUsers();
    }

    public LiveData<Integer> getPendingRequestsCount() {
        return threadManager.getPendingRequestsCount();
    }

    public LiveData<Integer> getInputMode() {
        return threadManager.getInputMode();
    }

    public LiveData<Boolean> isPending() {
        return threadManager.isPending();
    }

    public long getViewerId() {
        return viewerId;
    }

    public LiveData<DirectItem> getReplyToItem() {
        return threadManager.getReplyToItem();
    }

    public void fetchChats() {
        threadManager.fetchChats();
    }

    public void refreshChats() {
        threadManager.refreshChats();
    }

    public LiveData<Resource<Object>> sendText(final String text) {
        return threadManager.sendText(text);
    }

    public LiveData<Resource<Object>> sendUri(final MediaController.MediaEntry entry) {
        return threadManager.sendUri(entry);
    }

    public LiveData<Resource<Object>> sendUri(final Uri uri) {
        return threadManager.sendUri(uri);
    }

    public LiveData<Resource<Object>> startRecording() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        voiceRecorder = new VoiceRecorder(recordingsDir, new VoiceRecorder.VoiceRecorderCallback() {
            @Override
            public void onStart() {}

            @Override
            public void onComplete(final VoiceRecorder.VoiceRecordingResult result) {
                Log.d(TAG, "onComplete: recording complete. Scanning file...");
                MediaScannerConnection.scanFile(
                        application,
                        new String[]{result.getFile().getAbsolutePath()},
                        new String[]{result.getMimeType()},
                        (path, uri) -> {
                            if (uri == null) {
                                final String msg = "Scan failed!";
                                Log.e(TAG, msg);
                                data.postValue(Resource.error(msg, null));
                                return;
                            }
                            Log.d(TAG, "onComplete: scan complete");
                            MediaUtils.getVoiceInfo(contentResolver, uri, new MediaUtils.OnInfoLoadListener<MediaUtils.VideoInfo>() {
                                @Override
                                public void onLoad(@Nullable final MediaUtils.VideoInfo videoInfo) {
                                    if (videoInfo == null) return;
                                    threadManager.sendVoice(data,
                                                            uri,
                                                            result.getWaveform(),
                                                            result.getSamplingFreq(),
                                                            videoInfo == null ? 0 : videoInfo.duration,
                                                            videoInfo == null ? 0 : videoInfo.size);
                                }

                                @Override
                                public void onFailure(final Throwable t) {
                                    data.postValue(Resource.error(t.getMessage(), null));
                                }
                            });
                        }
                );
            }

            @Override
            public void onCancel() {

            }
        });
        voiceRecorder.startRecording();
        return data;
    }

    public void stopRecording(final boolean delete) {
        if (voiceRecorder == null) return;
        voiceRecorder.stopRecording(delete);
        voiceRecorder = null;
    }

    public LiveData<Resource<Object>> sendReaction(final DirectItem item, final Emoji emoji) {
        return threadManager.sendReaction(item, emoji);
    }

    public LiveData<Resource<Object>> sendDeleteReaction(final String itemId) {
        return threadManager.sendDeleteReaction(itemId);
    }

    public LiveData<Resource<Object>> unsend(final DirectItem item) {
        return threadManager.unsend(item);
    }

    public LiveData<Resource<Object>> sendAnimatedMedia(@NonNull final GiphyGif giphyGif) {
        return threadManager.sendAnimatedMedia(giphyGif);
    }

    public User getCurrentUser() {
        return currentUser;
    }

    @Nullable
    public User getUser(final long userId) {
        final LiveData<List<User>> users = getUsers();
        User match = null;
        if (users != null && users.getValue() != null) {
            final List<User> userList = users.getValue();
            match = userList.stream()
                            .filter(Objects::nonNull)
                            .filter(user -> user.getPk() == userId)
                            .findFirst()
                            .orElse(null);
        }
        if (match == null) {
            final LiveData<List<User>> leftUsers = getLeftUsers();
            if (leftUsers != null && leftUsers.getValue() != null) {
                final List<User> userList = leftUsers.getValue();
                match = userList.stream()
                                .filter(Objects::nonNull)
                                .filter(user -> user.getPk() == userId)
                                .findFirst()
                                .orElse(null);
            }
        }
        return match;
    }

    public void forward(final Set<RankedRecipient> recipients, final DirectItem itemToForward) {
        threadManager.forward(recipients, itemToForward);
    }

    public void forward(final RankedRecipient recipient, final DirectItem itemToForward) {
        threadManager.forward(recipient, itemToForward);
    }

    public void setReplyToItem(final DirectItem item) {
        // Log.d(TAG, "setReplyToItem: " + item);
        threadManager.setReplyToItem(item);
    }

    public LiveData<Resource<Object>> acceptRequest() {
        return threadManager.acceptRequest();
    }

    public LiveData<Resource<Object>> declineRequest() {
        return threadManager.declineRequest();
    }

    public LiveData<Resource<Object>> markAsSeen() {
        if (currentUser == null) {
            return getSuccessEventResObjectLiveData();
        }
        final DirectThread thread = getThread().getValue();
        if (thread == null) {
            return getSuccessEventResObjectLiveData();
        }
        final List<DirectItem> items = thread.getItems();
        if (items == null || items.isEmpty()) {
            return getSuccessEventResObjectLiveData();
        }
        final Optional<DirectItem> itemOptional = items.stream()
                                                       .filter(item -> item.getUserId() != currentUser.getPk())
                                                       .findFirst();
        if (!itemOptional.isPresent()) {
            return getSuccessEventResObjectLiveData();
        }
        final DirectItem directItem = itemOptional.get();
        final Map<Long, DirectThreadLastSeenAt> lastSeenAt = thread.getLastSeenAt();
        if (lastSeenAt != null) {
            final DirectThreadLastSeenAt seenAt = lastSeenAt.get(currentUser.getPk());
            try {
                if (seenAt != null
                        && (Objects.equals(seenAt.getItemId(), directItem.getItemId())
                        || Long.parseLong(seenAt.getTimestamp()) >= directItem.getTimestamp())) {
                    return getSuccessEventResObjectLiveData();
                }
            } catch (Exception ignored) {
                return getSuccessEventResObjectLiveData();
            }
        }
        return threadManager.markAsSeen(directItem);
    }

    @NonNull
    private MutableLiveData<Resource<Object>> getSuccessEventResObjectLiveData() {
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.success(new Object()));
        return data;
    }

    public void deleteThreadIfRequired() {
        final DirectThread thread = getThread().getValue();
        if (thread == null) return;
        if (thread.isTemp() && (thread.getItems() == null || thread.getItems().isEmpty())) {
            final InboxManager inboxManager = DirectMessagesManager.getInstance().getInboxManager();
            inboxManager.removeThread(threadId);
        }
    }

}
