package awais.instagrabber.services;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.text.TextUtils;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.repositories.responses.notification.NotificationCounts;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.webservices.NewsService;
import awais.instagrabber.webservices.ServiceCallback;

public class ActivityCheckerService extends Service {
    private static final String TAG = "ActivityCheckerService";
    private static final int INITIAL_DELAY_MILLIS = 200;
    private static final int DELAY_MILLIS = 60000;

    private Handler handler;
    private NewsService newsService;
    private ServiceCallback<NotificationCounts> cb;
    private NotificationManagerCompat notificationManager;

    private final IBinder binder = new LocalBinder();
    private final Runnable runnable = () -> {
        newsService.fetchActivityCounts(cb);
    };

    public class LocalBinder extends Binder {
        public ActivityCheckerService getService() {
            return ActivityCheckerService.this;
        }
    }

    @Override
    public void onCreate() {
        notificationManager = NotificationManagerCompat.from(getApplicationContext());
        newsService = NewsService.getInstance();
        handler = new Handler();
        cb = new ServiceCallback<NotificationCounts>() {
            @Override
            public void onSuccess(final NotificationCounts result) {
                try {
                    if (result == null) return;
                    final String notification = getNotificationString(result);
                    if (notification == null) return;
                    final String notificationString = getString(R.string.activity_count_prefix) + " " + notification + ".";
                    showNotification(notificationString);
                } finally {
                    handler.postDelayed(runnable, DELAY_MILLIS);
                }
            }

            @Override
            public void onFailure(final Throwable t) {}
        };
    }

    @Override
    public IBinder onBind(Intent intent) {
        startChecking();
        return binder;
    }

    @Override
    public boolean onUnbind(final Intent intent) {
        stopChecking();
        return super.onUnbind(intent);
    }

    private void startChecking() {
        handler.postDelayed(runnable, INITIAL_DELAY_MILLIS);
    }

    private void stopChecking() {
        handler.removeCallbacks(runnable);
    }

    private String getNotificationString(final NotificationCounts result) {
        final List<String> list = new ArrayList<>();
        if (result.getRelationshipsCount() != 0) {
            list.add(getString(R.string.activity_count_relationship, result.getRelationshipsCount()));
        }
        if (result.getRequestsCount() != 0) {
            list.add(getString(R.string.activity_count_requests, result.getRequestsCount()));
        }
        if (result.getUserTagsCount() != 0) {
            list.add(getString(R.string.activity_count_usertags, result.getUserTagsCount()));
        }
        if (result.getPOYCount() != 0) {
            list.add(getString(R.string.activity_count_poy, result.getPOYCount()));
        }
        if (result.getCommentsCount() != 0) {
            list.add(getString(R.string.activity_count_comments, result.getCommentsCount()));
        }
        if (result.getCommentLikesCount() != 0) {
            list.add(getString(R.string.activity_count_commentlikes, result.getCommentLikesCount()));
        }
        if (result.getLikesCount() != 0) {
            list.add(getString(R.string.activity_count_likes, result.getLikesCount()));
        }
        if (list.isEmpty()) return null;
        return TextUtils.join(", ", list);
    }

    private void showNotification(final String notificationString) {
        final Notification notification = new NotificationCompat.Builder(this, Constants.ACTIVITY_CHANNEL_ID)
                .setCategory(NotificationCompat.CATEGORY_STATUS)
                .setSmallIcon(R.drawable.ic_notif)
                .setAutoCancel(true)
                .setOnlyAlertOnce(true)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                .setContentTitle(getString(R.string.action_notif))
                .setContentText(notificationString)
                .setContentIntent(getPendingIntent())
                .build();
        notificationManager.notify(Constants.ACTIVITY_NOTIFICATION_ID, notification);
    }

    @NonNull
    private PendingIntent getPendingIntent() {
        final Intent intent = new Intent(getApplicationContext(), MainActivity.class)
                .setAction(Constants.ACTION_SHOW_ACTIVITY)
                .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        return PendingIntent.getActivity(getApplicationContext(), Constants.SHOW_ACTIVITY_REQUEST_CODE, intent, PendingIntent.FLAG_UPDATE_CURRENT);
    }
}
