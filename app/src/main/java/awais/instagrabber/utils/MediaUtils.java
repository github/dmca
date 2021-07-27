package awais.instagrabber.utils;

import android.content.ContentResolver;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public final class MediaUtils {
    private static final String TAG = MediaUtils.class.getSimpleName();
    private static final String[] PROJECTION_VIDEO = {
            MediaStore.Video.Media.DURATION,
            MediaStore.Video.Media.WIDTH,
            MediaStore.Video.Media.HEIGHT,
            MediaStore.Video.Media.SIZE
    };
    private static final String[] PROJECTION_AUDIO = {
            MediaStore.Audio.Media.DURATION,
            MediaStore.Audio.Media.SIZE
    };

    public static void getVideoInfo(@NonNull final ContentResolver contentResolver,
                                    @NonNull final Uri uri,
                                    @NonNull final OnInfoLoadListener<VideoInfo> listener) {
        AppExecutors.getInstance().tasksThread().submit(() -> {
            try (Cursor cursor = MediaStore.Video.query(contentResolver, uri, PROJECTION_VIDEO)) {
                if (cursor == null) {
                    if (listener != null) {
                        listener.onLoad(null);
                    }
                    return;
                }
                int durationColumn = cursor.getColumnIndex(MediaStore.Video.Media.DURATION);
                int widthColumn = cursor.getColumnIndex(MediaStore.Video.Media.WIDTH);
                int heightColumn = cursor.getColumnIndex(MediaStore.Video.Media.HEIGHT);
                int sizeColumn = cursor.getColumnIndex(MediaStore.Video.Media.SIZE);
                if (cursor.moveToNext()) {
                    if (listener != null) {
                        listener.onLoad(new VideoInfo(
                                cursor.getLong(durationColumn),
                                cursor.getInt(widthColumn),
                                cursor.getInt(heightColumn),
                                cursor.getLong(sizeColumn)
                        ));
                    }
                }
            } catch (Exception e) {
                Log.e(TAG, "getVideoInfo: ", e);
                if (listener != null) {
                    listener.onFailure(e);
                }
                return;
            }
            if (listener != null) {
                listener.onLoad(null);
            }
        });
    }

    public static void getVoiceInfo(@NonNull final ContentResolver contentResolver,
                                    @NonNull final Uri uri,
                                    @NonNull final OnInfoLoadListener<VideoInfo> listener) {
        AppExecutors.getInstance().tasksThread().submit(() -> {
            try (Cursor cursor = MediaStore.Video.query(contentResolver, uri, PROJECTION_AUDIO)) {
                if (cursor == null) {
                    if (listener != null) {
                        listener.onLoad(null);
                    }
                    return;
                }
                int durationColumn = cursor.getColumnIndex(MediaStore.Audio.Media.DURATION);
                int sizeColumn = cursor.getColumnIndex(MediaStore.Audio.Media.SIZE);
                if (cursor.moveToNext()) {
                    if (listener != null) {
                        listener.onLoad(new VideoInfo(
                                cursor.getLong(durationColumn),
                                0,
                                0,
                                cursor.getLong(sizeColumn)
                        ));
                    }
                }
            } catch (Exception e) {
                Log.e(TAG, "getVoiceInfo: ", e);
                if (listener != null) {
                    listener.onFailure(e);
                }
                return;
            }
            if (listener != null) {
                listener.onLoad(null);
            }
        });
    }

    public static class VideoInfo {
        public long duration;
        public int width;
        public int height;
        public long size;

        public VideoInfo(final long duration, final int width, final int height, final long size) {
            this.duration = duration;
            this.width = width;
            this.height = height;
            this.size = size;
        }

    }

    public interface OnInfoLoadListener<T> {
        void onLoad(@Nullable T info);

        void onFailure(Throwable t);
    }
}
