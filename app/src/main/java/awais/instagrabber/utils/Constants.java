package awais.instagrabber.utils;

public final class Constants {
    public static final String CRASH_REPORT_EMAIL = "barinsta@austinhuang.me";

    // int prefs, do not export
    public static final String PREV_INSTALL_VERSION = "prevVersion";
    public static final String BROWSER_UA_CODE = "browser_ua_code";
    public static final String APP_UA_CODE = "app_ua_code";
    // never Export
    public static final String COOKIE = "cookie";
    // deprecated: public static final String SHOW_QUICK_ACCESS_DIALOG = "show_quick_dlg";
    public static final String DEVICE_UUID = "device_uuid";
    public static final String BROWSER_UA = "browser_ua";
    public static final String APP_UA = "app_ua";
    //////////////////////// EXTRAS ////////////////////////
    public static final String EXTRAS_USER = "user";
    public static final String EXTRAS_HASHTAG = "hashtag";
    public static final String EXTRAS_LOCATION = "location";
    public static final String EXTRAS_USERNAME = "username";
    public static final String EXTRAS_ID = "id";
    public static final String EXTRAS_POST = "post";
    public static final String EXTRAS_PROFILE = "profile";
    public static final String EXTRAS_TYPE = "type";
    public static final String EXTRAS_NAME = "name";
    public static final String EXTRAS_STORIES = "stories";
    public static final String EXTRAS_HIGHLIGHT = "highlight";
    public static final String EXTRAS_INDEX = "index";
    public static final String EXTRAS_THREAD_MODEL = "threadModel";
    public static final String EXTRAS_FOLLOWERS = "followers";
    public static final String EXTRAS_SHORTCODE = "shortcode";
    public static final String EXTRAS_END_CURSOR = "endCursor";
    public static final String FEED = "feed";
    public static final String FEED_ORDER = "feedOrder";

    // Notification ids
    public static final int ACTIVITY_NOTIFICATION_ID = 10;
    public static final int DM_UNREAD_PARENT_NOTIFICATION_ID = 20;
    public static final int DM_CHECK_NOTIFICATION_ID = 11;

    // see https://github.com/dilame/instagram-private-api/blob/master/src/core/constants.ts
//    public static final String SUPPORTED_CAPABILITIES = "[ { \"name\": \"SUPPORTED_SDK_VERSIONS\", \"value\":" +
//            " \"13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,22.0,23.0,24.0,25.0,26.0,27.0,28.0,29.0,30.0,31.0," +
//            "32.0,33.0,34.0,35.0,36.0,37.0,38.0,39.0,40.0,41.0,42.0,43.0,44.0,45.0,46.0,47.0,48.0,49.0,50.0,51.0," +
//            "52.0,53.0,54.0,55.0,56.0,57.0,58.0,59.0,60.0,61.0,62.0,63.0,64.0,65.0,66.0\" }, { \"name\": \"FACE_TRACKER_VERSION\", " +
//            "\"value\": 12 }, { \"name\": \"segmentation\", \"value\": \"segmentation_enabled\" }, { \"name\": \"COMPRESSION\", " +
//            "\"value\": \"ETC2_COMPRESSION\" }, { \"name\": \"world_tracker\", \"value\": \"world_tracker_enabled\" }, { \"name\": " +
//            "\"gyroscope\", \"value\": \"gyroscope_enabled\" } ]";
//    public static final String SIGNATURE_VERSION = "4";
//    public static final String SIGNATURE_KEY = "9193488027538fd3450b83b7d05286d4ca9599a0f7eeed90d8c85925698a05dc";
    public static final String BREADCRUMB_KEY = "iN4$aGr0m";
    public static final int LOGIN_RESULT_CODE = 5000;
    public static final String SKIPPED_VERSION = "skipped_version";
    public static final String DEFAULT_TAB = "default_tab";
    public static final String PREF_DARK_THEME = "dark_theme";
    public static final String PREF_LIGHT_THEME = "light_theme";
    public static final String DEFAULT_HASH_TAG_PIC = "https://www.instagram.com/static/images/hashtag/search-hashtag-default-avatar.png/1d8417c9a4f5.png";
    public static final String SHARED_PREFERENCES_NAME = "settings";
    public static final String PREF_POSTS_LAYOUT = "posts_layout";
    public static final String PREF_PROFILE_POSTS_LAYOUT = "profile_posts_layout";
    public static final String PREF_TOPIC_POSTS_LAYOUT = "topic_posts_layout";
    public static final String PREF_HASHTAG_POSTS_LAYOUT = "hashtag_posts_layout";
    public static final String PREF_LOCATION_POSTS_LAYOUT = "location_posts_layout";
    public static final String PREF_LIKED_POSTS_LAYOUT = "liked_posts_layout";
    public static final String PREF_TAGGED_POSTS_LAYOUT = "tagged_posts_layout";
    public static final String PREF_SAVED_POSTS_LAYOUT = "saved_posts_layout";
    public static final String PREF_EMOJI_VARIANTS = "emoji_variants";
    public static final String PREF_REACTIONS = "reactions";

    public static final String ACTIVITY_CHANNEL_ID = "activity";
    public static final String ACTIVITY_CHANNEL_NAME = "Activity";
    public static final String DOWNLOAD_CHANNEL_ID = "download";
    public static final String DOWNLOAD_CHANNEL_NAME = "Downloads";
    public static final String DM_UNREAD_CHANNEL_ID = "dmUnread";
    public static final String DM_UNREAD_CHANNEL_NAME = "Messages";
    public static final String SILENT_NOTIFICATIONS_CHANNEL_ID = "silentNotifications";
    public static final String SILENT_NOTIFICATIONS_CHANNEL_NAME = "Silent notifications";

    public static final String NOTIF_GROUP_NAME = "awais.instagrabber.InstaNotif";
    public static final String GROUP_KEY_DM = "awais.instagrabber.MESSAGES";
    public static final String GROUP_KEY_SILENT_NOTIFICATIONS = "awais.instagrabber.SILENT_NOTIFICATIONS";

    public static final int SHOW_ACTIVITY_REQUEST_CODE = 1738;
    public static final int SHOW_DM_THREAD = 2000;
    public static final int DM_SYNC_SERVICE_REQUEST_CODE = 3000;
    public static final int GLOBAL_NETWORK_ERROR_DIALOG_REQUEST_CODE = 7777;

    public static final String ACTION_SHOW_ACTIVITY = "show_activity";
    public static final String ACTION_SHOW_DM_THREAD = "show_dm_thread";

    public static final String DM_THREAD_ACTION_EXTRA_THREAD_ID = "thread_id";
    public static final String DM_THREAD_ACTION_EXTRA_THREAD_TITLE = "thread_title";

    public static final String X_IG_APP_ID = "936619743392459";
}