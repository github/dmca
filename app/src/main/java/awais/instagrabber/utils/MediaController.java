package awais.instagrabber.utils;

import android.content.Context;
import android.database.Cursor;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.util.SparseArray;

import java.util.ArrayList;
import java.util.Collections;

import awais.instagrabber.R;

/*
 * This is the source code of Telegram for Android v. 1.3.x.
 * It is licensed under GNU GPL v. 2 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright Nikolai Kudashov, 2013-2018.
 */
public class MediaController {
    private static final String TAG = MediaController.class.getSimpleName();
    private static final String[] PROJECTION_PHOTOS = {
            MediaStore.Images.Media._ID,
            MediaStore.Images.Media.BUCKET_ID,
            MediaStore.Images.Media.BUCKET_DISPLAY_NAME,
            MediaStore.Images.Media.DATA,
            Build.VERSION.SDK_INT > 28 ? MediaStore.Images.Media.DATE_TAKEN : MediaStore.Images.Media.DATE_MODIFIED,
            MediaStore.Images.Media.ORIENTATION,
            MediaStore.Images.Media.WIDTH,
            MediaStore.Images.Media.HEIGHT,
            MediaStore.Images.Media.SIZE
    };
    private static final String[] PROJECTION_VIDEO = {
            MediaStore.Video.Media._ID,
            MediaStore.Video.Media.BUCKET_ID,
            MediaStore.Video.Media.BUCKET_DISPLAY_NAME,
            MediaStore.Video.Media.DATA,
            Build.VERSION.SDK_INT > 28 ? MediaStore.Video.Media.DATE_TAKEN : MediaStore.Images.Media.DATE_MODIFIED,
            MediaStore.Video.Media.DURATION,
            MediaStore.Video.Media.WIDTH,
            MediaStore.Video.Media.HEIGHT,
            MediaStore.Video.Media.SIZE
    };

    private final Context context;
    private final OnLoadListener onLoadListener;
    private final AppExecutors appExecutors;

    private static Runnable broadcastPhotosRunnable;

    private ArrayList<AlbumEntry> allMediaAlbums;
    private ArrayList<AlbumEntry> allPhotoAlbums;
    private AlbumEntry allPhotosAlbumEntry;
    private AlbumEntry allMediaAlbumEntry;
    private AlbumEntry allVideosAlbumEntry;

    public MediaController(final Context context, final OnLoadListener onLoadListener) {
        this.context = context;
        this.onLoadListener = onLoadListener;
        appExecutors = AppExecutors.getInstance();
    }

    public void load() {
        loadGalleryAlbums();
    }

    private void loadGalleryAlbums() {
        final Thread thread = new Thread(() -> {
            final ArrayList<AlbumEntry> mediaAlbumsSorted = new ArrayList<>();
            final ArrayList<AlbumEntry> photoAlbumsSorted = new ArrayList<>();
            SparseArray<AlbumEntry> mediaAlbums = new SparseArray<>();
            SparseArray<AlbumEntry> photoAlbums = new SparseArray<>();
            AlbumEntry allPhotosAlbum = null;
            AlbumEntry allVideosAlbum = null;
            AlbumEntry allMediaAlbum = null;
            String cameraFolder = null;
            try {
                cameraFolder = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).getAbsolutePath() + "/" + "Camera/";
            } catch (Exception e) {
                Log.e(TAG, "loadGalleryAlbums: ", e);
            }
            Integer mediaCameraAlbumId = null;
            Integer photoCameraAlbumId = null;

            Cursor cursor = null;
            try {
                if (PermissionUtils.hasAttachMediaPerms(context)) {
                    cursor = MediaStore.Images.Media.query(context.getContentResolver(),
                                                           MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                                                           PROJECTION_PHOTOS,
                                                           null,
                                                           null,
                                                           (Build.VERSION.SDK_INT > 28
                                                            ? MediaStore.Images.Media.DATE_TAKEN
                                                            : MediaStore.Images.Media.DATE_MODIFIED) + " DESC");
                    if (cursor != null) {
                        int imageIdColumn = cursor.getColumnIndex(MediaStore.Images.Media._ID);
                        int bucketIdColumn = cursor.getColumnIndex(MediaStore.Images.Media.BUCKET_ID);
                        int bucketNameColumn = cursor.getColumnIndex(MediaStore.Images.Media.BUCKET_DISPLAY_NAME);
                        int dataColumn = cursor.getColumnIndex(MediaStore.Images.Media.DATA);
                        int dateColumn = cursor.getColumnIndex(Build.VERSION.SDK_INT > 28 ? MediaStore.Images.Media.DATE_TAKEN
                                                                                          : MediaStore.Images.Media.DATE_MODIFIED);
                        int orientationColumn = cursor.getColumnIndex(MediaStore.Images.Media.ORIENTATION);
                        int widthColumn = cursor.getColumnIndex(MediaStore.Images.Media.WIDTH);
                        int heightColumn = cursor.getColumnIndex(MediaStore.Images.Media.HEIGHT);
                        int sizeColumn = cursor.getColumnIndex(MediaStore.Images.Media.SIZE);

                        while (cursor.moveToNext()) {
                            String path = cursor.getString(dataColumn);
                            if (TextUtils.isEmpty(path)) {
                                continue;
                            }

                            int imageId = cursor.getInt(imageIdColumn);
                            int bucketId = cursor.getInt(bucketIdColumn);
                            String bucketName = cursor.getString(bucketNameColumn);
                            long dateTaken = cursor.getLong(dateColumn);
                            int orientation = cursor.getInt(orientationColumn);
                            int width = cursor.getInt(widthColumn);
                            int height = cursor.getInt(heightColumn);
                            long size = cursor.getLong(sizeColumn);

                            MediaEntry mediaEntry = new MediaEntry(bucketId, imageId, dateTaken, path, orientation, -1, false, width, height, size);

                            if (allPhotosAlbum == null) {
                                allPhotosAlbum = new AlbumEntry(0, context.getString(R.string.all_photos), mediaEntry);
                                photoAlbumsSorted.add(0, allPhotosAlbum);
                            }
                            if (allMediaAlbum == null) {
                                allMediaAlbum = new AlbumEntry(0, context.getString(R.string.all_media), mediaEntry);
                                mediaAlbumsSorted.add(0, allMediaAlbum);
                            }
                            allPhotosAlbum.addPhoto(mediaEntry);
                            allMediaAlbum.addPhoto(mediaEntry);

                            AlbumEntry albumEntry = mediaAlbums.get(bucketId);
                            if (albumEntry == null) {
                                albumEntry = new AlbumEntry(bucketId, bucketName, mediaEntry);
                                mediaAlbums.put(bucketId, albumEntry);
                                if (mediaCameraAlbumId == null && cameraFolder != null && path.startsWith(cameraFolder)) {
                                    mediaAlbumsSorted.add(0, albumEntry);
                                    mediaCameraAlbumId = bucketId;
                                } else {
                                    mediaAlbumsSorted.add(albumEntry);
                                }
                            }
                            albumEntry.addPhoto(mediaEntry);

                            albumEntry = photoAlbums.get(bucketId);
                            if (albumEntry == null) {
                                albumEntry = new AlbumEntry(bucketId, bucketName, mediaEntry);
                                photoAlbums.put(bucketId, albumEntry);
                                if (photoCameraAlbumId == null && cameraFolder != null && path.startsWith(cameraFolder)) {
                                    photoAlbumsSorted.add(0, albumEntry);
                                    photoCameraAlbumId = bucketId;
                                } else {
                                    photoAlbumsSorted.add(albumEntry);
                                }
                            }
                            albumEntry.addPhoto(mediaEntry);
                        }
                    }
                }
            } catch (Throwable e) {
                Log.e(TAG, "loadGalleryAlbums: ", e);
            } finally {
                if (cursor != null) {
                    try {
                        cursor.close();
                    } catch (Exception e) {
                        Log.e(TAG, "loadGalleryAlbums: ", e);
                    }
                }
            }

            try {
                if (PermissionUtils.hasAttachMediaPerms(context)) {
                    cursor = MediaStore.Images.Media.query(context.getContentResolver(),
                                                           MediaStore.Video.Media.EXTERNAL_CONTENT_URI,
                                                           PROJECTION_VIDEO,
                                                           MediaStore.Video.Media.MIME_TYPE + "=?",
                                                           new String[]{"video/mp4"},
                                                           (Build.VERSION.SDK_INT > 28
                                                            ? MediaStore.Video.Media.DATE_TAKEN
                                                            : MediaStore.Video.Media.DATE_MODIFIED) + " DESC");
                    if (cursor != null) {
                        int imageIdColumn = cursor.getColumnIndex(MediaStore.Video.Media._ID);
                        int bucketIdColumn = cursor.getColumnIndex(MediaStore.Video.Media.BUCKET_ID);
                        int bucketNameColumn = cursor.getColumnIndex(MediaStore.Video.Media.BUCKET_DISPLAY_NAME);
                        int dataColumn = cursor.getColumnIndex(MediaStore.Video.Media.DATA);
                        int dateColumn = cursor.getColumnIndex(Build.VERSION.SDK_INT > 28 ? MediaStore.Video.Media.DATE_TAKEN
                                                                                          : MediaStore.Video.Media.DATE_MODIFIED);
                        int durationColumn = cursor.getColumnIndex(MediaStore.Video.Media.DURATION);
                        int widthColumn = cursor.getColumnIndex(MediaStore.Video.Media.WIDTH);
                        int heightColumn = cursor.getColumnIndex(MediaStore.Video.Media.HEIGHT);
                        int sizeColumn = cursor.getColumnIndex(MediaStore.Video.Media.SIZE);

                        while (cursor.moveToNext()) {
                            String path = cursor.getString(dataColumn);
                            if (TextUtils.isEmpty(path)) {
                                continue;
                            }

                            int imageId = cursor.getInt(imageIdColumn);
                            int bucketId = cursor.getInt(bucketIdColumn);
                            String bucketName = cursor.getString(bucketNameColumn);
                            long dateTaken = cursor.getLong(dateColumn);
                            long duration = cursor.getLong(durationColumn);
                            int width = cursor.getInt(widthColumn);
                            int height = cursor.getInt(heightColumn);
                            long size = cursor.getLong(sizeColumn);

                            MediaEntry mediaEntry = new MediaEntry(bucketId, imageId, dateTaken, path, -1, duration, true, width, height, size);

                            if (allVideosAlbum == null) {
                                allVideosAlbum = new AlbumEntry(0, context.getString(R.string.all_videos), mediaEntry);
                                allVideosAlbum.videoOnly = true;
                                int index = 0;
                                if (allMediaAlbum != null) {
                                    index++;
                                }
                                if (allPhotosAlbum != null) {
                                    index++;
                                }
                                mediaAlbumsSorted.add(index, allVideosAlbum);
                            }
                            if (allMediaAlbum == null) {
                                allMediaAlbum = new AlbumEntry(0, context.getString(R.string.all_media), mediaEntry);
                                mediaAlbumsSorted.add(0, allMediaAlbum);
                            }
                            allVideosAlbum.addPhoto(mediaEntry);
                            allMediaAlbum.addPhoto(mediaEntry);

                            AlbumEntry albumEntry = mediaAlbums.get(bucketId);
                            if (albumEntry == null) {
                                albumEntry = new AlbumEntry(bucketId, bucketName, mediaEntry);
                                mediaAlbums.put(bucketId, albumEntry);
                                if (mediaCameraAlbumId == null && cameraFolder != null && path.startsWith(cameraFolder)) {
                                    mediaAlbumsSorted.add(0, albumEntry);
                                    mediaCameraAlbumId = bucketId;
                                } else {
                                    mediaAlbumsSorted.add(albumEntry);
                                }
                            }

                            albumEntry.addPhoto(mediaEntry);
                        }
                    }
                }
            } catch (Throwable e) {
                Log.e(TAG, "loadGalleryAlbums: ", e);
            } finally {
                if (cursor != null) {
                    try {
                        cursor.close();
                    } catch (Exception e) {
                        Log.e(TAG, "loadGalleryAlbums: ", e);
                    }
                }
            }
            for (int a = 0; a < mediaAlbumsSorted.size(); a++) {
                Collections.sort(mediaAlbumsSorted.get(a).photos, (o1, o2) -> {
                    if (o1.dateTaken < o2.dateTaken) {
                        return 1;
                    } else if (o1.dateTaken > o2.dateTaken) {
                        return -1;
                    }
                    return 0;
                });
            }
            broadcastNewPhotos(mediaAlbumsSorted, photoAlbumsSorted, mediaCameraAlbumId, allMediaAlbum, allPhotosAlbum, allVideosAlbum, 0);
        });
        thread.setPriority(Thread.MIN_PRIORITY);
        thread.start();
    }

    private void broadcastNewPhotos(final ArrayList<AlbumEntry> mediaAlbumsSorted,
                                    final ArrayList<AlbumEntry> photoAlbumsSorted,
                                    final Integer cameraAlbumIdFinal,
                                    final AlbumEntry allMediaAlbumFinal,
                                    final AlbumEntry allPhotosAlbumFinal,
                                    final AlbumEntry allVideosAlbumFinal,
                                    int delay) {
        if (broadcastPhotosRunnable != null) {
            appExecutors.mainThread().cancel(broadcastPhotosRunnable);
        }
        appExecutors.mainThread().execute(broadcastPhotosRunnable = () -> {
            allMediaAlbums = mediaAlbumsSorted;
            allPhotoAlbums = photoAlbumsSorted;
            broadcastPhotosRunnable = null;
            allPhotosAlbumEntry = allPhotosAlbumFinal;
            allMediaAlbumEntry = allMediaAlbumFinal;
            allVideosAlbumEntry = allVideosAlbumFinal;
            if (onLoadListener != null) {
                onLoadListener.onLoad();
            }
        }, delay);
    }

    public AlbumEntry getAllMediaAlbumEntry() {
        return allMediaAlbumEntry;
    }

    public AlbumEntry getAllPhotosAlbumEntry() {
        return allPhotosAlbumEntry;
    }

    public AlbumEntry getAllVideosAlbumEntry() {
        return allVideosAlbumEntry;
    }

    public ArrayList<AlbumEntry> getAllMediaAlbums() {
        return allMediaAlbums;
    }

    public ArrayList<AlbumEntry> getAllPhotoAlbums() {
        return allPhotoAlbums;
    }

    public static class AlbumEntry {
        public int bucketId;
        public boolean videoOnly;
        public String bucketName;
        public MediaEntry coverPhoto;
        public ArrayList<MediaEntry> photos = new ArrayList<>();
        public SparseArray<MediaEntry> photosByIds = new SparseArray<>();

        public AlbumEntry(int bucketId, String bucketName, MediaEntry coverPhoto) {
            this.bucketId = bucketId;
            this.bucketName = bucketName;
            this.coverPhoto = coverPhoto;
        }

        public void addPhoto(MediaEntry mediaEntry) {
            photos.add(mediaEntry);
            photosByIds.put(mediaEntry.imageId, mediaEntry);
        }
    }

    public static class MediaEntry {
        public int bucketId;
        public int imageId;
        public long dateTaken;
        public long duration;
        public int width;
        public int height;
        public long size;
        public String path;
        public int orientation;
        public boolean isVideo;
        public boolean isMuted;
        public boolean canDeleteAfter;

        public MediaEntry(int bucketId,
                          int imageId,
                          long dateTaken,
                          String path,
                          int orientation,
                          long duration,
                          boolean isVideo,
                          int width,
                          int height,
                          long size) {
            this.bucketId = bucketId;
            this.imageId = imageId;
            this.dateTaken = dateTaken;
            this.path = path;
            this.width = width;
            this.height = height;
            this.size = size;
            if (isVideo) {
                this.duration = duration;
            } else {
                this.orientation = orientation;
            }
            this.isVideo = isVideo;
        }
    }

    public interface OnLoadListener {
        void onLoad();
    }
}
