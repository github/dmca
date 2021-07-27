package awais.instagrabber.utils;

import android.Manifest;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Environment;
import android.util.Log;
import android.webkit.MimeTypeMap;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.work.Constraints;
import androidx.work.Data;
import androidx.work.NetworkType;
import androidx.work.OneTimeWorkRequest;
import androidx.work.WorkManager;
import androidx.work.WorkRequest;

import com.google.gson.Gson;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.regex.Pattern;

import awais.instagrabber.R;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.models.StoryModel;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Audio;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.VideoVersion;
import awais.instagrabber.workers.DownloadWorker;

import static awais.instagrabber.fragments.settings.PreferenceKeys.FOLDER_PATH;
import static awais.instagrabber.fragments.settings.PreferenceKeys.FOLDER_SAVE_TO;

public final class DownloadUtils {
    private static final String TAG = "DownloadUtils";

    public static final String WRITE_PERMISSION = Manifest.permission.WRITE_EXTERNAL_STORAGE;
    public static final String[] PERMS = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE};

    @NonNull
    private static File getDownloadDir() {
        File dir = new File(Environment.getExternalStorageDirectory(), "Download");

        if (Utils.settingsHelper.getBoolean(FOLDER_SAVE_TO)) {
            final String customPath = Utils.settingsHelper.getString(FOLDER_PATH);
            if (!TextUtils.isEmpty(customPath)) {
                dir = new File(customPath);
            }
        }
        return dir;
    }

    @Nullable
    private static File getDownloadDir(@NonNull final Context context, @Nullable final String username) {
        return getDownloadDir(context, username, false);
    }

    @Nullable
    private static File getDownloadDir(final Context context,
                                       @Nullable final String username,
                                       final boolean skipCreateDir) {
        File dir = getDownloadDir();

        if (Utils.settingsHelper.getBoolean(PreferenceKeys.DOWNLOAD_USER_FOLDER) && !TextUtils.isEmpty(username)) {
            final String finaleUsername = username.startsWith("@") ? username.substring(1) : username;
            dir = new File(dir, finaleUsername);
        }

        if (context != null && !skipCreateDir && !dir.exists() && !dir.mkdirs()) {
            Toast.makeText(context, R.string.error_creating_folders, Toast.LENGTH_SHORT).show();
            return null;
        }
        return dir;
    }

    //    public static void dmDownload(@NonNull final Context context,
    //                                  @Nullable final String username,
    //                                  final String modelId,
    //                                  final String url) {
    //        if (url == null) return;
    //        if (ContextCompat.checkSelfPermission(context, PERMS[0]) == PackageManager.PERMISSION_GRANTED) {
    //            dmDownloadImpl(context, username, modelId, url);
    //        } else if (context instanceof Activity) {
    //            ActivityCompat.requestPermissions((Activity) context, PERMS, 8020);
    //        }
    //    }

//    private static void dmDownloadImpl(@NonNull final Context context,
//                                       @Nullable final String username,
//                                       final String modelId,
//                                       final String url) {
//        final File dir = getDownloadDir(context, username);
//        if (dir.exists() || dir.mkdirs()) {
//            download(context,
//                     url,
//                     getDownloadSaveFile(dir, modelId, url).getAbsolutePath());
//            return;
//        }
//        Toast.makeText(context, R.string.error_creating_folders, Toast.LENGTH_SHORT).show();
//    }

    @NonNull
    private static File getDownloadSaveFile(final File finalDir,
                                            final String postId,
                                            final String displayUrl) {
        return getDownloadSaveFile(finalDir, postId, "", displayUrl, "");
    }

    @NonNull
    private static File getDownloadSaveFile(final File finalDir,
                                            final String postId,
                                            final String displayUrl,
                                            final String username) {
        return getDownloadSaveFile(finalDir, postId, "", displayUrl, username);
    }

    private static File getDownloadChildSaveFile(final File downloadDir,
                                                 final String postId,
                                                 final int childPosition,
                                                 final String url,
                                                 final String username) {
        final String sliderPostfix = "_slide_" + childPosition;
        return getDownloadSaveFile(downloadDir, postId, sliderPostfix, url, username);
    }

    @NonNull
    private static File getDownloadSaveFile(final File finalDir,
                                            final String postId,
                                            final String sliderPostfix,
                                            final String displayUrl,
                                            final String username) {
        final String usernamePrepend = TextUtils.isEmpty(username) ? "" : (username + "_");
        final String fileName = usernamePrepend + postId + sliderPostfix + getFileExtensionFromUrl(displayUrl);
        return new File(finalDir, fileName);
    }

    @NonNull
    public static File getTempFile() {
        return getTempFile(null, null);
    }

    public static File getTempFile(final String fileName, final String extension) {
        final File dir = getDownloadDir();
        String name = fileName;
        if (TextUtils.isEmpty(name)) {
            name = UUID.randomUUID().toString();
        }
        if (!TextUtils.isEmpty(extension)) {
            name += "." + extension;
        }
        return new File(dir, name);
    }

    /**
     * Copied from {@link MimeTypeMap#getFileExtensionFromUrl(String)})
     * <p>
     * Returns the file extension or an empty string if there is no
     * extension. This method is a convenience method for obtaining the
     * extension of a url and has undefined results for other Strings.
     *
     * @param url URL
     * @return The file extension of the given url.
     */
    public static String getFileExtensionFromUrl(String url) {
        if (!TextUtils.isEmpty(url)) {
            int fragment = url.lastIndexOf('#');
            if (fragment > 0) {
                url = url.substring(0, fragment);
            }

            int query = url.lastIndexOf('?');
            if (query > 0) {
                url = url.substring(0, query);
            }

            int filenamePos = url.lastIndexOf('/');
            String filename =
                    0 <= filenamePos ? url.substring(filenamePos + 1) : url;

            // if the filename contains special characters, we don't
            // consider it valid for our matching purposes:
            if (!filename.isEmpty() &&
                    Pattern.matches("[a-zA-Z_0-9.\\-()%]+", filename)) {
                int dotPos = filename.lastIndexOf('.');
                if (0 <= dotPos) {
                    return filename.substring(dotPos);
                }
            }
        }

        return "";
    }

    public static List<Boolean> checkDownloaded(@NonNull final Media media) {
        final List<Boolean> checkList = new LinkedList<>();
        final User user = media.getUser();
        String username = "username";
        if (user != null) {
            username = user.getUsername();
        }
        final File downloadDir = getDownloadDir(null, "@" + username, true);
        switch (media.getMediaType()) {
            case MEDIA_TYPE_IMAGE:
            case MEDIA_TYPE_VIDEO: {
                final String url = ResponseBodyUtils.getImageUrl(media);
                final File file = getDownloadSaveFile(downloadDir, media.getCode(), url, "");
                final File usernamePrependedFile = getDownloadSaveFile(downloadDir, media.getCode(), url, username);
                checkList.add(file.exists() || usernamePrependedFile.exists());
                break;
            }
            case MEDIA_TYPE_SLIDER:
                final List<Media> sliderItems = media.getCarouselMedia();
                for (int i = 0; i < sliderItems.size(); i++) {
                    final Media child = sliderItems.get(i);
                    if (child == null) continue;
                    final String url = ResponseBodyUtils.getImageUrl(child);
                    final File file = getDownloadChildSaveFile(downloadDir, media.getCode(), i + 1, url, "");
                    final File usernamePrependedFile = getDownloadChildSaveFile(downloadDir, media.getCode(), i + 1, url, username);
                    checkList.add(file.exists() || usernamePrependedFile.exists());
                }
                break;
            default:
        }
        return checkList;
    }

    public static void showDownloadDialog(@NonNull Context context,
                                          @NonNull final Media feedModel,
                                          final int childPosition) {
        if (childPosition >= 0) {
            final DialogInterface.OnClickListener clickListener = (dialog, which) -> {
                switch (which) {
                    case 0:
                        DownloadUtils.download(context, feedModel, childPosition);
                        break;
                    case 1:
                        DownloadUtils.download(context, feedModel);
                        break;
                    case DialogInterface.BUTTON_NEGATIVE:
                    default:
                        dialog.dismiss();
                        break;
                }
            };
            final String[] items = new String[]{
                    context.getString(R.string.post_viewer_download_current),
                    context.getString(R.string.post_viewer_download_album),
            };
            new AlertDialog.Builder(context)
                    .setTitle(R.string.post_viewer_download_dialog_title)
                    .setItems(items, clickListener)
                    .setNegativeButton(R.string.cancel, null)
                    .show();
            return;
        }
        DownloadUtils.download(context, feedModel);
    }

    public static void download(@NonNull final Context context,
                                @NonNull final StoryModel storyModel) {
        final File downloadDir = getDownloadDir(context, "@" + storyModel.getUsername());
        final String url = storyModel.getItemType() == MediaItemType.MEDIA_TYPE_VIDEO
                           ? storyModel.getVideoUrl()
                           : storyModel.getStoryUrl();
        final String baseFileName = storyModel.getStoryMediaId() + "_"
                + storyModel.getTimestamp() + DownloadUtils.getFileExtensionFromUrl(url);
        final String usernamePrepend = Utils.settingsHelper.getBoolean(PreferenceKeys.DOWNLOAD_PREPEND_USER_NAME)
                && storyModel.getUsername() != null ? storyModel.getUsername() + "_" : "";
        final File saveFile = new File(downloadDir,
                 usernamePrepend + baseFileName);
        download(context, url, saveFile.getAbsolutePath());
    }

    public static void download(@NonNull final Context context,
                                @NonNull final Media feedModel) {
        download(context, feedModel, -1);
    }

    public static void download(@NonNull final Context context,
                                @NonNull final Media feedModel,
                                final int position) {
        download(context, Collections.singletonList(feedModel), position);
    }

    public static void download(@NonNull final Context context,
                                @NonNull final List<Media> feedModels) {
        download(context, feedModels, -1);
    }

    private static void download(@NonNull final Context context,
                                 @NonNull final List<Media> feedModels,
                                 final int childPositionIfSingle) {
        final Map<String, String> map = new HashMap<>();
        for (final Media media : feedModels) {
            final User mediaUser = media.getUser();
            final File downloadDir = getDownloadDir(context, mediaUser == null ? "" : mediaUser.getUsername());
            if (downloadDir == null) return;
            switch (media.getMediaType()) {
                case MEDIA_TYPE_IMAGE:
                case MEDIA_TYPE_VIDEO: {
                    final String url = getUrlOfType(media);
                    String fileName = media.getId();
                    if (mediaUser != null && TextUtils.isEmpty(media.getCode())) {
                        fileName = mediaUser.getUsername() + "_" + fileName;
                    }
                    if (!TextUtils.isEmpty(media.getCode())) {
                        fileName = media.getCode();
                        if (Utils.settingsHelper.getBoolean(PreferenceKeys.DOWNLOAD_PREPEND_USER_NAME) && mediaUser != null) {
                            fileName = mediaUser.getUsername() + "_" + fileName;
                        }
                    }
                    final File file = getDownloadSaveFile(downloadDir, fileName, url);
                    map.put(url, file.getAbsolutePath());
                    break;
                }
                case MEDIA_TYPE_VOICE: {
                    final String url = getUrlOfType(media);
                    String fileName = media.getId();
                    if (mediaUser != null) {
                        fileName = mediaUser.getUsername() + "_" + fileName;
                    }
                    final File file = getDownloadSaveFile(downloadDir, fileName, url);
                    map.put(url, file.getAbsolutePath());
                    break;
                }
                case MEDIA_TYPE_SLIDER:
                    final List<Media> sliderItems = media.getCarouselMedia();
                    for (int i = 0; i < sliderItems.size(); i++) {
                        if (childPositionIfSingle >= 0 && feedModels.size() == 1 && i != childPositionIfSingle) {
                            continue;
                        }
                        final Media child = sliderItems.get(i);
                        final String url = getUrlOfType(child);
                        final String usernamePrepend = Utils.settingsHelper.getBoolean(PreferenceKeys.DOWNLOAD_PREPEND_USER_NAME) && mediaUser != null ? mediaUser.getUsername() : "";
                        final File file = getDownloadChildSaveFile(downloadDir, media.getCode(), i + 1, url, usernamePrepend);
                        map.put(url, file.getAbsolutePath());
                    }
                    break;
                default:
            }
        }
        download(context, map);
    }

    @Nullable
    private static String getUrlOfType(@NonNull final Media media) {
        switch (media.getMediaType()) {
            case MEDIA_TYPE_IMAGE: {
                return ResponseBodyUtils.getImageUrl(media);
            }
            case MEDIA_TYPE_VIDEO: {
                final List<VideoVersion> videoVersions = media.getVideoVersions();
                String url = null;
                if (videoVersions != null && !videoVersions.isEmpty()) {
                    final VideoVersion videoVersion = videoVersions.get(0);
                    if (videoVersion != null) {
                        url = videoVersion.getUrl();
                    }
                }
                return url;
            }
            case MEDIA_TYPE_VOICE: {
                final Audio audio = media.getAudio();
                String url = null;
                if (audio != null) {
                    url = audio.getAudioSrc();
                }
                return url;
            }
        }
        return null;
    }

    public static void download(final Context context,
                                final String url,
                                final String filePath) {
        if (context == null || url == null || filePath == null) return;
        download(context, Collections.singletonMap(url, filePath));
    }

    private static void download(final Context context, final Map<String, String> urlFilePathMap) {
        final Constraints constraints = new Constraints.Builder()
                .setRequiredNetworkType(NetworkType.CONNECTED)
                .build();
        final DownloadWorker.DownloadRequest request = DownloadWorker.DownloadRequest.builder()
                                                                                     .setUrlToFilePathMap(urlFilePathMap)
                                                                                     .build();
        final String requestJson = new Gson().toJson(request);
        final File tempFile = getTempFile();
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(tempFile))) {
            writer.write(requestJson);
        } catch (IOException e) {
            Log.e(TAG, "download: Error writing request to file", e);
            //noinspection ResultOfMethodCallIgnored
            tempFile.delete();
            return;
        }
        final WorkRequest downloadWorkRequest = new OneTimeWorkRequest.Builder(DownloadWorker.class)
                .setInputData(
                        new Data.Builder()
                                .putString(DownloadWorker.KEY_DOWNLOAD_REQUEST_JSON, tempFile.getAbsolutePath())
                                .build()
                )
                .setConstraints(constraints)
                .addTag("download")
                .build();
        WorkManager.getInstance(context)
                   .enqueue(downloadWorkRequest);
    }
}
