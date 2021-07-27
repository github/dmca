package awais.instagrabber.utils;

import android.content.Context;
import android.content.pm.PackageManager;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.PermissionChecker;
import androidx.fragment.app.Fragment;

import static android.Manifest.permission.CAMERA;
import static android.Manifest.permission.READ_EXTERNAL_STORAGE;
import static android.Manifest.permission.RECORD_AUDIO;
import static android.Manifest.permission.WRITE_EXTERNAL_STORAGE;
import static androidx.core.content.PermissionChecker.checkSelfPermission;

public class PermissionUtils {
    public static final String[] AUDIO_RECORD_PERMS = new String[]{WRITE_EXTERNAL_STORAGE, RECORD_AUDIO};
    public static final String[] ATTACH_MEDIA_PERMS = new String[]{READ_EXTERNAL_STORAGE};
    public static final String[] CAMERA_PERMS = new String[]{CAMERA};

    public static boolean hasAudioRecordPerms(@NonNull final Context context) {
        return checkSelfPermission(context, WRITE_EXTERNAL_STORAGE) == PermissionChecker.PERMISSION_GRANTED
                && checkSelfPermission(context, RECORD_AUDIO) == PermissionChecker.PERMISSION_GRANTED;
    }

    public static void requestAudioRecordPerms(final Fragment fragment, final int requestCode) {
        fragment.requestPermissions(AUDIO_RECORD_PERMS, requestCode);
    }

    public static boolean hasAttachMediaPerms(@NonNull final Context context) {
        return checkSelfPermission(context, READ_EXTERNAL_STORAGE) == PermissionChecker.PERMISSION_GRANTED;
    }

    public static void requestAttachMediaPerms(final Fragment fragment, final int requestCode) {
        fragment.requestPermissions(ATTACH_MEDIA_PERMS, requestCode);
    }

    public static boolean hasCameraPerms(final Context context) {
        return ContextCompat.checkSelfPermission(context, CAMERA) == PackageManager.PERMISSION_GRANTED;
    }

    public static void requestCameraPerms(final AppCompatActivity activity, final int requestCode) {
        ActivityCompat.requestPermissions(activity, CAMERA_PERMS, requestCode);
    }
}
