package awais.instagrabber.utils;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableMap;

import org.json.JSONObject;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import awais.instagrabber.models.UploadPhotoOptions;
import awais.instagrabber.models.UploadVideoOptions;
import awais.instagrabber.models.enums.MediaItemType;

public final class MediaUploadHelper {
    private static final long LOWER = 1000000000L;
    private static final long UPPER = 9999999999L;

    private static Map<String, String> createPhotoRuploadParams(final UploadPhotoOptions options) {
        final String retryContextString = getRetryContextString();
        final Map<String, String> params = new HashMap<>();
        params.put("retry_context", retryContextString);
        params.put("media_type", "1");
        params.put("upload_id", options.getUploadId());
        params.put("xsharing_user_ids", "[]");
        final Map<String, String> imageCompression = new HashMap<>();
        imageCompression.put("lib_name", "moz");
        imageCompression.put("lib_version", "3.1.m");
        imageCompression.put("quality", "80");
        params.put("image_compression", new JSONObject(imageCompression).toString());
        if (options.isSideCar()) {
            params.put("is_sidecar", "1");
        }
        return params;
    }

    private static Map<String, String> createVideoRuploadParams(final UploadVideoOptions options) {
        final String retryContextString = getRetryContextString();
        final Map<String, String> ruploadParams = new HashMap<>();
        ruploadParams.put("retry_context", retryContextString);
        ruploadParams.put("media_type", "2");
        ruploadParams.put("xsharing_user_ids", "[]");
        ruploadParams.put("upload_id", options.getUploadId());
        ruploadParams.put("upload_media_width", String.valueOf(options.getWidth()));
        ruploadParams.put("upload_media_height", String.valueOf(options.getHeight()));
        ruploadParams.put("upload_media_duration_ms", String.valueOf(options.getDuration()));
        if (options.isSideCar()) {
            ruploadParams.put("is_sidecar", "1");
        }
        if (options.isForAlbum()) {
            ruploadParams.put("for_album", "1");
        }
        if (options.isDirect()) {
            ruploadParams.put("direct_v2", "1");
        }
        if (options.isForDirectStory()) {
            ruploadParams.put("for_direct_story", "1");
            ruploadParams.put("content_tags", "");
        }
        if (options.isIgtvVideo()) {
            ruploadParams.put("is_igtv_video", "1");
        }
        if (options.isDirectVoice()) {
            ruploadParams.put("is_direct_voice", "1");
        }
        return ruploadParams;
    }

    @NonNull
    public static String getRetryContextString() {
        final Map<String, Integer> retryContext = new HashMap<>();
        retryContext.put("num_step_auto_retry", 0);
        retryContext.put("num_reupload", 0);
        retryContext.put("num_step_manual_retry", 0);
        return new JSONObject(retryContext).toString();
    }

    public static UploadPhotoOptions createUploadPhotoOptions(final long byteLength) {
        final String uploadId = generateUploadId();
        return UploadPhotoOptions.builder()
                                 .setUploadId(uploadId)
                                 .setName(generateName(uploadId))
                                 .setByteLength(byteLength)
                                 .build();
    }

    public static UploadVideoOptions createUploadDmVideoOptions(final long byteLength,
                                                                final long duration,
                                                                final int width,
                                                                final int height) {
        final String uploadId = generateUploadId();
        return UploadVideoOptions.builder()
                                 .setUploadId(uploadId)
                                 .setName(generateName(uploadId))
                                 .setByteLength(byteLength)
                                 .setDuration(duration)
                                 .setWidth(width)
                                 .setHeight(height)
                                 .setIsDirect(true)
                                 .setMediaType(MediaItemType.MEDIA_TYPE_VIDEO)
                                 .build();
    }

    public static UploadVideoOptions createUploadDmVoiceOptions(final long byteLength,
                                                                final long duration) {
        final String uploadId = generateUploadId();
        return UploadVideoOptions.builder()
                                 .setUploadId(uploadId)
                                 .setName(generateName(uploadId))
                                 .setDuration(duration)
                                 .setIsDirectVoice(true)
                                 .setByteLength(byteLength)
                                 .setMediaType(MediaItemType.MEDIA_TYPE_VOICE)
                                 .build();
    }

    @NonNull
    public static String generateUploadId() {
        return String.valueOf(new Date().getTime() / 1000);
    }

    @NonNull
    public static String generateName(final String uploadId) {
        final long random = NumberUtils.random(LOWER, UPPER + 1);
        return String.format("%s_0_%s", uploadId, random);
    }

    @NonNull
    public static Map<String, String> getUploadPhotoHeaders(@NonNull final UploadPhotoOptions options) {
        final String waterfallId = TextUtils.isEmpty(options.getWaterfallId()) ? UUID.randomUUID().toString() : options.getWaterfallId();
        final String contentLength = String.valueOf(options.getByteLength());
        final Map<String, String> headers = new HashMap<>();
        headers.put("X_FB_PHOTO_WATERFALL_ID", waterfallId);
        headers.put("X-Entity-Type", "image/jpeg");
        headers.put("Offset", "0");
        headers.put("X-Instagram-Rupload-Params", new JSONObject(createPhotoRuploadParams(options)).toString());
        headers.put("X-Entity-Name", options.getName());
        headers.put("X-Entity-Length", contentLength);
        headers.put("Content-Type", "application/octet-stream");
        headers.put("Content-Length", contentLength);
        headers.put("Accept-Encoding", "gzip");
        return headers;
    }

    @NonNull
    public static Map<String, String> getUploadVideoHeaders(@NonNull final UploadVideoOptions options) {
        final Map<String, String> ruploadParams = createVideoRuploadParams(options);
        final String waterfallId = TextUtils.isEmpty(options.getWaterfallId()) ? UUID.randomUUID().toString() : options.getWaterfallId();
        final String contentLength = String.valueOf(options.getByteLength());
        return ImmutableMap.<String, String>builder()
                .putAll(getBaseUploadVideoHeaders(ruploadParams))
                .put("X_FB_PHOTO_WATERFALL_ID", waterfallId)
                .put("X-Entity-Type", "video/mp4")
                .put("Offset", String.valueOf(options.getOffset() > 0 ? options.getOffset() : 0))
                .put("X-Entity-Name", options.getName())
                .put("X-Entity-Length", contentLength)
                .put("Content-Type", "application/octet-stream")
                .put("Content-Length", contentLength)
                .build();
    }

    private static Map<String, String> getBaseUploadVideoHeaders(@NonNull final Map<String, String> ruploadParams) {
        return ImmutableMap.of(
                "X-IG-Connection-Type", "WIFI",
                "X-IG-Capabilities", "3brTvwE=",
                "Accept-Encoding", "gzip",
                "X-Instagram-Rupload-Params", new JSONObject(ruploadParams).toString()
        );
    }
}
