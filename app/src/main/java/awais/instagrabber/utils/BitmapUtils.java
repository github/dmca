package awais.instagrabber.utils;

import android.content.ContentResolver;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.util.Log;
import android.util.LruCache;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.util.Pair;

import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public final class BitmapUtils {
    private static final String TAG = BitmapUtils.class.getSimpleName();
    private static final LruCache<String, Bitmap> bitmapMemoryCache;
    private static final AppExecutors appExecutors = AppExecutors.getInstance();
    private static final ExecutorService callbackHandlers = Executors
            .newCachedThreadPool(r -> new Thread(r, "bm-load-callback-handler#" + NumberUtils.random(0, 100)));
    public static final float THUMBNAIL_SIZE = 200f;

    static {
        // Get max available VM memory, exceeding this amount will throw an
        // OutOfMemory exception. Stored in kilobytes as LruCache takes an
        // int in its constructor.
        final int maxMemory = (int) (Runtime.getRuntime().maxMemory() / 1024);
        // Use 1/8th of the available memory for this memory cache.
        final int cacheSize = maxMemory / 8;
        bitmapMemoryCache = new LruCache<String, Bitmap>(cacheSize) {
            @Override
            protected int sizeOf(String key, Bitmap bitmap) {
                // The cache size will be measured in kilobytes rather than
                // number of items.
                return bitmap.getByteCount() / 1024;
            }
        };

    }

    public static void addBitmapToMemoryCache(final String key, final Bitmap bitmap, final boolean force) {
        if (force || getBitmapFromMemCache(key) == null) {
            bitmapMemoryCache.put(key, bitmap);
        }
    }

    public static Bitmap getBitmapFromMemCache(final String key) {
        return bitmapMemoryCache.get(key);
    }

    public static void getThumbnail(final Context context, final Uri uri, final ThumbnailLoadCallback callback) {
        if (context == null || uri == null || callback == null) return;
        final String key = uri.toString();
        final Bitmap cachedBitmap = getBitmapFromMemCache(key);
        if (cachedBitmap != null) {
            callback.onLoad(cachedBitmap, -1, -1);
            return;
        }
        loadBitmap(context.getContentResolver(), uri, THUMBNAIL_SIZE, THUMBNAIL_SIZE, true, callback);
    }

    /**
     * Loads bitmap from given Uri
     *
     * @param contentResolver {@link ContentResolver} to resolve the uri
     * @param uri             Uri from where Bitmap will be loaded
     * @param reqWidth        Required width
     * @param reqHeight       Required height
     * @param addToCache      true if the loaded bitmap should be added to the mem cache
     * @param callback        Bitmap load callback
     */
    public static void loadBitmap(final ContentResolver contentResolver,
                                  final Uri uri,
                                  final float reqWidth,
                                  final float reqHeight,
                                  final boolean addToCache,
                                  final ThumbnailLoadCallback callback) {
        loadBitmap(contentResolver, uri, reqWidth, reqHeight, -1, addToCache, callback);
    }

    /**
     * Loads bitmap from given Uri
     *
     * @param contentResolver {@link ContentResolver} to resolve the uri
     * @param uri             Uri from where Bitmap will be loaded
     * @param maxDimenSize    Max size of the largest side of the image
     * @param addToCache      true if the loaded bitmap should be added to the mem cache
     * @param callback        Bitmap load callback
     */
    public static void loadBitmap(final ContentResolver contentResolver,
                                  final Uri uri,
                                  final float maxDimenSize,
                                  final boolean addToCache,
                                  final ThumbnailLoadCallback callback) {
        loadBitmap(contentResolver, uri, -1, -1, maxDimenSize, addToCache, callback);
    }

    /**
     * Loads bitmap from given Uri
     *
     * @param contentResolver {@link ContentResolver} to resolve the uri
     * @param uri             Uri from where {@link Bitmap} will be loaded
     * @param reqWidth        Required width (set to -1 if maxDimenSize provided)
     * @param reqHeight       Required height (set to -1 if maxDimenSize provided)
     * @param maxDimenSize    Max size of the largest side of the image (set to -1 if setting reqWidth and reqHeight)
     * @param addToCache      true if the loaded bitmap should be added to the mem cache
     * @param callback        Bitmap load callback
     */
    private static void loadBitmap(final ContentResolver contentResolver,
                                   final Uri uri,
                                   final float reqWidth,
                                   final float reqHeight,
                                   final float maxDimenSize,
                                   final boolean addToCache,
                                   final ThumbnailLoadCallback callback) {
        if (contentResolver == null || uri == null || callback == null) return;
        final ListenableFuture<BitmapResult> future = appExecutors
                .tasksThread()
                .submit(() -> getBitmapResult(contentResolver, uri, reqWidth, reqHeight, maxDimenSize, addToCache));
        Futures.addCallback(future, new FutureCallback<BitmapResult>() {
            @Override
            public void onSuccess(@Nullable final BitmapResult result) {
                if (result == null) {
                    callback.onLoad(null, -1, -1);
                    return;
                }
                callback.onLoad(result.bitmap, result.width, result.height);
            }

            @Override
            public void onFailure(@NonNull final Throwable t) {
                callback.onFailure(t);
            }
        }, callbackHandlers);
    }

    @Nullable
    public static BitmapResult getBitmapResult(final ContentResolver contentResolver,
                                                final Uri uri,
                                                final float reqWidth,
                                                final float reqHeight,
                                                final float maxDimenSize,
                                                final boolean addToCache) {
        BitmapFactory.Options bitmapOptions;
        float actualReqWidth = reqWidth;
        float actualReqHeight = reqHeight;
        try (InputStream input = contentResolver.openInputStream(uri)) {
            BitmapFactory.Options outBounds = new BitmapFactory.Options();
            outBounds.inJustDecodeBounds = true;
            outBounds.inPreferredConfig = Bitmap.Config.ARGB_8888;
            BitmapFactory.decodeStream(input, null, outBounds);
            if ((outBounds.outWidth == -1) || (outBounds.outHeight == -1)) return null;
            bitmapOptions = new BitmapFactory.Options();
            if (maxDimenSize > 0) {
                // Raw height and width of image
                final int height = outBounds.outHeight;
                final int width = outBounds.outWidth;
                final float ratio = (float) width / height;
                if (height > width) {
                    actualReqHeight = maxDimenSize;
                    actualReqWidth = actualReqHeight * ratio;
                } else {
                    actualReqWidth = maxDimenSize;
                    actualReqHeight = actualReqWidth / ratio;
                }
            }
            bitmapOptions.inSampleSize = calculateInSampleSize(outBounds, actualReqWidth, actualReqHeight);
        } catch (Exception e) {
            Log.e(TAG, "loadBitmap: ", e);
            return null;
        }
        try (InputStream input = contentResolver.openInputStream(uri)) {
            bitmapOptions.inPreferredConfig = Bitmap.Config.ARGB_8888;
            Bitmap bitmap = BitmapFactory.decodeStream(input, null, bitmapOptions);
            if (addToCache) {
                addBitmapToMemoryCache(uri.toString(), bitmap, true);
            }
            return new BitmapResult(bitmap, (int) actualReqWidth, (int) actualReqHeight);
        } catch (Exception e) {
            Log.e(TAG, "loadBitmap: ", e);
        }
        return null;
    }

    public static class BitmapResult {
        public Bitmap bitmap;
        int width;
        int height;

        public BitmapResult(final Bitmap bitmap, final int width, final int height) {
            this.width = width;
            this.height = height;
            this.bitmap = bitmap;
        }
    }

    private static int calculateInSampleSize(final BitmapFactory.Options options, final float reqWidth, final float reqHeight) {
        // Raw height and width of image
        final int height = options.outHeight;
        final int width = options.outWidth;
        int inSampleSize = 1;
        if (height > reqHeight || width > reqWidth) {
            final float halfHeight = height / 2f;
            final float halfWidth = width / 2f;
            // Calculate the largest inSampleSize value that is a power of 2 and keeps both
            // height and width larger than the requested height and width.
            while ((halfHeight / inSampleSize) >= reqHeight
                    && (halfWidth / inSampleSize) >= reqWidth) {
                inSampleSize *= 2;
            }
        }
        return inSampleSize;
    }

    public interface ThumbnailLoadCallback {
        /**
         * @param bitmap Resulting bitmap
         * @param width  width of the bitmap (Only correct if loadBitmap was called or -1)
         * @param height height of the bitmap (Only correct if loadBitmap was called or -1)
         */
        void onLoad(@Nullable Bitmap bitmap, int width, int height);

        void onFailure(@NonNull Throwable t);
    }

    /**
     * Decodes the bounds of an image from its Uri and returns a pair of the dimensions
     *
     * @param uri the Uri of the image
     * @return dimensions of the image
     */
    public static Pair<Integer, Integer> decodeDimensions(@NonNull final ContentResolver contentResolver,
                                                          @NonNull final Uri uri) throws FileNotFoundException {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeStream(contentResolver.openInputStream(uri), null, options);
        return (options.outWidth == -1 || options.outHeight == -1)
               ? null
               : new Pair<>(options.outWidth, options.outHeight);
    }

    public static File convertToJpegAndSaveToFile(@NonNull final Bitmap bitmap, @Nullable final File file) throws IOException {
        File tempFile = file;
        if (file == null) {
            tempFile = DownloadUtils.getTempFile();
        }
        try (OutputStream output = new FileOutputStream(tempFile)) {
            final boolean compressResult = bitmap.compress(Bitmap.CompressFormat.JPEG, 100, output);
            if (!compressResult) {
                throw new RuntimeException("Compression failed!");
            }
        }
        return tempFile;
    }

    public static void convertToJpegAndSaveToUri(@NonNull Context context,
                                                 @NonNull final Bitmap bitmap,
                                                 @NonNull final Uri uri) throws Exception {
        try (OutputStream output = context.getContentResolver().openOutputStream(uri)) {
            final boolean compressResult = bitmap.compress(Bitmap.CompressFormat.JPEG, 100, output);
            if (!compressResult) {
                throw new RuntimeException("Compression failed!");
            }
        }
    }
}
