package awais.instagrabber.services;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;
import androidx.lifecycle.LifecycleService;

import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableMap;

import java.time.LocalDateTime;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Function;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.db.datasources.DMLastNotifiedDataSource;
import awais.instagrabber.db.entities.DMLastNotified;
import awais.instagrabber.db.repositories.DMLastNotifiedRepository;
import awais.instagrabber.db.repositories.RepositoryCallback;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.managers.DirectMessagesManager;
import awais.instagrabber.managers.InboxManager;
import awais.instagrabber.models.Resource;
import awais.instagrabber.repositories.responses.directmessages.DirectInbox;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadLastSeenAt;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.DMUtils;
import awais.instagrabber.utils.DateUtils;
import awais.instagrabber.utils.TextUtils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class DMSyncService extends LifecycleService {
    private static final String TAG = DMSyncService.class.getSimpleName();

    private InboxManager inboxManager;
    private DMLastNotifiedRepository dmLastNotifiedRepository;
    private Map<String, DMLastNotified> dmLastNotifiedMap;

    @Override
    public void onCreate() {
        super.onCreate();
        startForeground(Constants.DM_CHECK_NOTIFICATION_ID, buildForegroundNotification());
        Log.d(TAG, "onCreate: Service created");
        final DirectMessagesManager directMessagesManager = DirectMessagesManager.getInstance();
        inboxManager = directMessagesManager.getInboxManager();
        dmLastNotifiedRepository = DMLastNotifiedRepository.getInstance(DMLastNotifiedDataSource.getInstance(getApplicationContext()));
    }

    private void parseUnread(@NonNull final DirectInbox directInbox) {
        dmLastNotifiedRepository.getAllDMDmLastNotified(new RepositoryCallback<List<DMLastNotified>>() {
            @Override
            public void onSuccess(final List<DMLastNotified> result) {
                dmLastNotifiedMap = result != null
                                    ? result.stream().collect(Collectors.toMap(DMLastNotified::getThreadId, Function.identity()))
                                    : Collections.emptyMap();
                parseUnreadActual(directInbox);
            }

            @Override
            public void onDataNotAvailable() {
                dmLastNotifiedMap = Collections.emptyMap();
                parseUnreadActual(directInbox);
            }
        });
        // Log.d(TAG, "inbox observer: " + directInbox);
    }

    private void parseUnreadActual(@NonNull final DirectInbox directInbox) {
        final List<DirectThread> threads = directInbox.getThreads();
        final ImmutableMap.Builder<String, List<DirectItem>> unreadMessagesMapBuilder = ImmutableMap.builder();
        if (threads == null) {
            stopSelf();
            return;
        }
        for (final DirectThread thread : threads) {
            if (thread.isMuted()) continue;
            final boolean read = DMUtils.isRead(thread);
            if (read) continue;
            final List<DirectItem> unreadMessages = getUnreadMessages(thread);
            if (unreadMessages.isEmpty()) continue;
            unreadMessagesMapBuilder.put(thread.getThreadId(), unreadMessages);
        }
        final Map<String, List<DirectItem>> unreadMessagesMap = unreadMessagesMapBuilder.build();
        if (unreadMessagesMap.isEmpty()) {
            stopSelf();
            return;
        }
        showNotification(directInbox, unreadMessagesMap);
        final LocalDateTime now = LocalDateTime.now();
        // Update db
        final ImmutableList.Builder<DMLastNotified> lastNotifiedListBuilder = ImmutableList.builder();
        for (final Map.Entry<String, List<DirectItem>> unreadMessagesEntry : unreadMessagesMap.entrySet()) {
            final List<DirectItem> unreadItems = unreadMessagesEntry.getValue();
            final DirectItem latestItem = unreadItems.get(unreadItems.size() - 1);
            lastNotifiedListBuilder.add(new DMLastNotified(0,
                                                           unreadMessagesEntry.getKey(),
                                                           latestItem.getLocalDateTime(),
                                                           now));
        }
        dmLastNotifiedRepository.insertOrUpdateDMLastNotified(
                lastNotifiedListBuilder.build(),
                new RepositoryCallback<Void>() {
                    @Override
                    public void onSuccess(final Void result) {
                        stopSelf();
                    }

                    @Override
                    public void onDataNotAvailable() {
                        stopSelf();
                    }
                }
        );
    }

    @NonNull
    private List<DirectItem> getUnreadMessages(@NonNull final DirectThread thread) {
        final List<DirectItem> items = thread.getItems();
        if (items == null) return Collections.emptyList();
        final DMLastNotified dmLastNotified = dmLastNotifiedMap.get(thread.getThreadId());
        final long viewerId = thread.getViewerId();
        final Map<Long, DirectThreadLastSeenAt> lastSeenAt = thread.getLastSeenAt();
        final ImmutableList.Builder<DirectItem> unreadListBuilder = ImmutableList.builder();
        int count = 0;
        for (final DirectItem item : items) {
            if (item == null) continue;
            if (item.getUserId() == viewerId) break; // Reached a message from the viewer, it is assumed the viewer has read the next messages
            final boolean read = DMUtils.isRead(item, lastSeenAt, Collections.singletonList(viewerId));
            if (read) break;
            if (dmLastNotified != null && dmLastNotified.getLastNotifiedMsgTs() != null) {
                if (count == 0 && DateUtils.isBeforeOrEqual(item.getLocalDateTime(), dmLastNotified.getLastNotifiedMsgTs())) {
                    // The first unread item has been notified and hence all subsequent items can be ignored
                    // since the items are in desc timestamp order
                    break;
                }
            }
            unreadListBuilder.add(item);
            count++;
            // Inbox style notification only allows 6 lines
            if (count >= 6) break;
        }
        // Reversing, so that oldest messages are on top
        return unreadListBuilder.build().reverse();
    }

    private void showNotification(final DirectInbox directInbox,
                                  final Map<String, List<DirectItem>> unreadMessagesMap) {
        final NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (notificationManager == null) return;
        for (final Map.Entry<String, List<DirectItem>> unreadMessagesEntry : unreadMessagesMap.entrySet()) {
            final Optional<DirectThread> directThreadOptional = getThread(directInbox, unreadMessagesEntry.getKey());
            if (!directThreadOptional.isPresent()) continue;
            final DirectThread thread = directThreadOptional.get();
            final DirectItem firstDirectItem = thread.getFirstDirectItem();
            if (firstDirectItem == null) continue;
            final List<DirectItem> unreadMessages = unreadMessagesEntry.getValue();
            final NotificationCompat.InboxStyle inboxStyle = new NotificationCompat.InboxStyle();
            inboxStyle.setBigContentTitle(thread.getThreadTitle());
            for (final DirectItem item : unreadMessages) {
                inboxStyle.addLine(DMUtils.getMessageString(thread, getResources(), thread.getViewerId(), item));
            }
            final Notification notification = new NotificationCompat.Builder(this, Constants.DM_UNREAD_CHANNEL_ID)
                    .setStyle(inboxStyle)
                    .setSmallIcon(R.drawable.ic_round_mode_comment_24)
                    .setContentTitle(thread.getThreadTitle())
                    .setContentText(DMUtils.getMessageString(thread, getResources(), thread.getViewerId(), firstDirectItem))
                    .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                    .setDefaults(NotificationCompat.DEFAULT_ALL)
                    .setGroup(Constants.GROUP_KEY_DM)
                    .setAutoCancel(true)
                    .setContentIntent(getThreadPendingIntent(thread.getThreadId(), thread.getThreadTitle()))
                    .build();
            notificationManager.notify(Constants.DM_UNREAD_PARENT_NOTIFICATION_ID, notification);
        }
    }

    private Optional<DirectThread> getThread(@NonNull final DirectInbox directInbox, final String threadId) {
        return directInbox.getThreads()
                          .stream()
                          .filter(thread -> Objects.equals(thread.getThreadId(), threadId))
                          .findFirst();
    }

    @NonNull
    private PendingIntent getThreadPendingIntent(final String threadId, final String threadTitle) {
        final Intent intent = new Intent(getApplicationContext(), MainActivity.class)
                .setAction(Constants.ACTION_SHOW_DM_THREAD)
                .putExtra(Constants.DM_THREAD_ACTION_EXTRA_THREAD_ID, threadId)
                .putExtra(Constants.DM_THREAD_ACTION_EXTRA_THREAD_TITLE, threadTitle)
                .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        return PendingIntent.getActivity(getApplicationContext(), Constants.SHOW_DM_THREAD, intent, PendingIntent.FLAG_UPDATE_CURRENT);
    }

    @Override
    public int onStartCommand(final Intent intent, final int flags, final int startId) {
        super.onStartCommand(intent, flags, startId);
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        final boolean isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) != 0;
        if (!isLoggedIn) {
            stopSelf();
            return START_NOT_STICKY;
        }
        // Need to setup here if service was started by the boot completed receiver
        CookieUtils.setupCookies(cookie);
        final boolean notificationsEnabled = settingsHelper.getBoolean(PreferenceKeys.PREF_ENABLE_DM_NOTIFICATIONS);
        inboxManager.getInbox().observe(this, inboxResource -> {
            if (!notificationsEnabled || inboxResource == null || inboxResource.status != Resource.Status.SUCCESS) {
                stopSelf();
                return;
            }
            final DirectInbox directInbox = inboxResource.data;
            if (directInbox == null) {
                stopSelf();
                return;
            }
            parseUnread(directInbox);
        });
        Log.d(TAG, "onStartCommand: refreshing inbox");
        inboxManager.refresh();
        return START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(@NonNull final Intent intent) {
        super.onBind(intent);
        return null;
    }

    private Notification buildForegroundNotification() {
        final Resources resources = getResources();
        return new NotificationCompat.Builder(this, Constants.SILENT_NOTIFICATIONS_CHANNEL_ID)
                .setOngoing(true)
                .setSound(null)
                .setContentTitle(resources.getString(R.string.app_name))
                .setContentText(resources.getString(R.string.checking_for_new_messages))
                .setSmallIcon(R.mipmap.ic_launcher)
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setGroup(Constants.GROUP_KEY_SILENT_NOTIFICATIONS)
                .build();
    }
}