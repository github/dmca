package awais.instagrabber.services;

import android.app.IntentService;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationManagerCompat;

import java.io.File;
import java.util.Random;

import awais.instagrabber.utils.TextUtils;

public class DeleteImageIntentService extends IntentService {
    private final static String TAG = "DeleteImageIntent";
    private static final int DELETE_IMAGE_SERVICE_REQUEST_CODE = 9010;
    private static final Random random = new Random();

    public static final String EXTRA_IMAGE_PATH = "extra_image_path";
    public static final String EXTRA_NOTIFICATION_ID = "extra_notification_id";
    public static final String DELETE_IMAGE_SERVICE = "delete_image_service";

    public DeleteImageIntentService() {
        super(DELETE_IMAGE_SERVICE);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        startService(new Intent(this, DeleteImageIntentService.class));
    }

    @Override
    protected void onHandleIntent(@Nullable Intent intent) {
        if (intent != null && Intent.ACTION_DELETE.equals(intent.getAction()) && intent.hasExtra(EXTRA_IMAGE_PATH)) {
            final String path = intent.getStringExtra(EXTRA_IMAGE_PATH);
            if (TextUtils.isEmpty(path)) return;
            final File file = new File(path);
            boolean deleted;
            if (file.exists()) {
                deleted = file.delete();
                if (!deleted) {
                    Log.w(TAG, "onHandleIntent: file not deleted!");
                }
            } else {
                deleted = true;
            }
            if (deleted) {
                final int notificationId = intent.getIntExtra(EXTRA_NOTIFICATION_ID, -1);
                NotificationManagerCompat.from(this).cancel(notificationId);
            }
        }
    }

    @NonNull
    public static PendingIntent pendingIntent(@NonNull final Context context,
                                              @NonNull final String imagePath,
                                              final int notificationId) {
        final Intent intent = new Intent(context, DeleteImageIntentService.class);
        intent.setAction(Intent.ACTION_DELETE);
        intent.putExtra(EXTRA_IMAGE_PATH, imagePath);
        intent.putExtra(EXTRA_NOTIFICATION_ID, notificationId);
        return PendingIntent.getService(context, random.nextInt(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
    }
}
