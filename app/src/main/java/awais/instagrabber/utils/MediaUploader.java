package awais.instagrabber.utils;

import android.content.ContentResolver;
import android.graphics.Bitmap;
import android.net.Uri;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Map;

import awais.instagrabber.models.UploadPhotoOptions;
import awais.instagrabber.models.UploadVideoOptions;
import awais.instagrabber.webservices.interceptors.AddCookiesInterceptor;
import okhttp3.Call;
import okhttp3.Headers;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;
import okhttp3.ResponseBody;
import okio.BufferedSink;
import okio.Okio;
import okio.Source;

public final class MediaUploader {
    private static final String TAG = MediaUploader.class.getSimpleName();
    private static final String HOST = "https://i.instagram.com";
    private static final AppExecutors appExecutors = AppExecutors.getInstance();

    public static void uploadPhoto(@NonNull final Uri uri,
                                   @NonNull final ContentResolver contentResolver,
                                   @NonNull final OnMediaUploadCompleteListener listener) {
        BitmapUtils.loadBitmap(contentResolver, uri, 1000, false, new BitmapUtils.ThumbnailLoadCallback() {
            @Override
            public void onLoad(@Nullable final Bitmap bitmap, final int width, final int height) {
                if (bitmap == null) {
                    listener.onFailure(new RuntimeException("Bitmap result was null"));
                    return;
                }
                uploadPhoto(bitmap, listener);
            }

            @Override
            public void onFailure(@NonNull final Throwable t) {
                listener.onFailure(t);
            }
        });
    }

    private static void uploadPhoto(@NonNull final Bitmap bitmap,
                                    @NonNull final OnMediaUploadCompleteListener listener) {
        appExecutors.tasksThread().submit(() -> {
            final File file;
            final long byteLength;
            try {
                file = BitmapUtils.convertToJpegAndSaveToFile(bitmap, null);
                byteLength = file.length();
            } catch (Exception e) {
                listener.onFailure(e);
                return;
            }
            final UploadPhotoOptions options = MediaUploadHelper.createUploadPhotoOptions(byteLength);
            final Map<String, String> headers = MediaUploadHelper.getUploadPhotoHeaders(options);
            final String url = HOST + "/rupload_igphoto/" + options.getName() + "/";
            appExecutors.networkIO().execute(() -> {
                try (FileInputStream input = new FileInputStream(file)) {
                    upload(input, url, headers, listener);
                } catch (IOException e) {
                    listener.onFailure(e);
                } finally {
                    //noinspection ResultOfMethodCallIgnored
                    file.delete();
                }
            });
        });
    }

    public static void uploadVideo(final Uri uri,
                                   final ContentResolver contentResolver,
                                   final UploadVideoOptions options,
                                   final OnMediaUploadCompleteListener listener) {
        appExecutors.tasksThread().submit(() -> {
            final Map<String, String> headers = MediaUploadHelper.getUploadVideoHeaders(options);
            final String url = HOST + "/rupload_igvideo/" + options.getName() + "/";
            appExecutors.networkIO().execute(() -> {
                try (InputStream input = contentResolver.openInputStream(uri)) {
                    if (input == null) {
                        listener.onFailure(new RuntimeException("InputStream was null"));
                        return;
                    }
                    upload(input, url, headers, listener);
                } catch (IOException e) {
                    listener.onFailure(e);
                }
            });
        });
    }

    private static void upload(@NonNull final InputStream input,
                               @NonNull final String url,
                               @NonNull final Map<String, String> headers,
                               @NonNull final OnMediaUploadCompleteListener listener) {
        try {
            final OkHttpClient client = new OkHttpClient.Builder()
                    .addInterceptor(new AddCookiesInterceptor())
                    .followRedirects(false)
                    .followSslRedirects(false)
                    .build();
            final Request request = new Request.Builder()
                    .headers(Headers.of(headers))
                    .url(url)
                    .post(create(MediaType.parse("application/octet-stream"), input))
                    .build();
            final Call call = client.newCall(request);
            final Response response = call.execute();
            final ResponseBody body = response.body();
            if (!response.isSuccessful()) {
                listener.onFailure(new IOException("Unexpected code " + response + (body != null ? ": " + body.string() : "")));
                return;
            }
            listener.onUploadComplete(new MediaUploadResponse(response.code(), body != null ? new JSONObject(body.string()) : null));
        } catch (Exception e) {
            listener.onFailure(e);
        }
    }

    public interface OnMediaUploadCompleteListener {
        void onUploadComplete(MediaUploadResponse response);

        void onFailure(Throwable t);
    }

    private static RequestBody create(final MediaType mediaType, final InputStream inputStream) {
        return new RequestBody() {
            @Override
            public MediaType contentType() {
                return mediaType;
            }

            @Override
            public long contentLength() {
                try {
                    return inputStream.available();
                } catch (IOException e) {
                    return 0;
                }
            }

            @Override
            public void writeTo(@NonNull BufferedSink sink) throws IOException {
                try (Source source = Okio.source(inputStream)) {
                    sink.writeAll(source);
                }
            }
        };
    }

    public static class MediaUploadResponse {
        private final int responseCode;
        private final JSONObject response;

        public MediaUploadResponse(int responseCode, JSONObject response) {
            this.responseCode = responseCode;
            this.response = response;
        }

        public int getResponseCode() {
            return responseCode;
        }

        public JSONObject getResponse() {
            return response;
        }

        @NonNull
        @Override
        public String toString() {
            return "MediaUploadResponse{" +
                    "responseCode=" + responseCode +
                    ", response=" + response +
                    '}';
        }
    }
}
