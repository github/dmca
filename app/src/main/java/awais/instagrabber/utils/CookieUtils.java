package awais.instagrabber.utils;

import android.content.Context;
import android.util.Log;
import android.webkit.CookieManager;

import androidx.annotation.Nullable;

import java.net.CookiePolicy;
import java.net.CookieStore;
import java.net.HttpCookie;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.db.datasources.AccountDataSource;
import awais.instagrabber.db.repositories.AccountRepository;
import awais.instagrabber.db.repositories.RepositoryCallback;
//import awaisomereport.LogCollector;

public final class CookieUtils {
    private static final String TAG = CookieUtils.class.getSimpleName();
    public static final CookieManager COOKIE_MANAGER = CookieManager.getInstance();
    public static final java.net.CookieManager NET_COOKIE_MANAGER = new java.net.CookieManager(null, CookiePolicy.ACCEPT_ALL);

    public static void setupCookies(final String cookieRaw) {
        final CookieStore cookieStore = NET_COOKIE_MANAGER.getCookieStore();
        if (cookieStore == null || TextUtils.isEmpty(cookieRaw)) {
            return;
        }
        if (cookieRaw.equals("LOGOUT")) {
            cookieStore.removeAll();
            return;
        }
        try {
            final URI uri1 = new URI("https://instagram.com");
            final URI uri2 = new URI("https://instagram.com/");
            final URI uri3 = new URI("https://i.instagram.com/");
            for (final String cookie : cookieRaw.split("; ")) {
                final String[] strings = cookie.split("=", 2);
                final HttpCookie httpCookie = new HttpCookie(strings[0].trim(), strings[1].trim());
                httpCookie.setDomain(".instagram.com");
                httpCookie.setPath("/");
                httpCookie.setVersion(0);
                cookieStore.add(uri1, httpCookie);
                cookieStore.add(uri2, httpCookie);
                cookieStore.add(uri3, httpCookie);
            }
        } catch (final URISyntaxException e) {
//            if (Utils.logCollector != null)
//                Utils.logCollector.appendException(e, LogCollector.LogFile.UTILS, "setupCookies");
            if (BuildConfig.DEBUG) Log.e(TAG, "", e);
        }
    }

    public static void removeAllAccounts(final Context context, final RepositoryCallback<Void> callback) {
        final CookieStore cookieStore = NET_COOKIE_MANAGER.getCookieStore();
        if (cookieStore == null) return;
        cookieStore.removeAll();
        try {
            AccountRepository.getInstance(AccountDataSource.getInstance(context))
                             .deleteAllAccounts(callback);
        } catch (Exception e) {
            Log.e(TAG, "setupCookies", e);
        }
    }

    public static long getUserIdFromCookie(final String cookies) {
        final String dsUserId = getCookieValue(cookies, "ds_user_id");
        if (dsUserId == null) {
            return 0;
        }
        try {
            return Long.parseLong(dsUserId);
        } catch (NumberFormatException e) {
            Log.e(TAG, "getUserIdFromCookie: ", e);
        }
        return 0;
    }

    @Nullable
    public static String getCsrfTokenFromCookie(final String cookies) {
        return getCookieValue(cookies, "csrftoken");
    }

    @Nullable
    private static String getCookieValue(final String cookies, final String name) {
        if (cookies == null) return null;
        final Pattern pattern = Pattern.compile(name + "=(.+?);");
        final Matcher matcher = pattern.matcher(cookies);
        if (matcher.find()) {
            return matcher.group(1);
        }
        return null;
    }

    @Nullable
    public static String getCookie(@Nullable final String webViewUrl) {
        final List<String> domains = new ArrayList<>(Arrays.asList(
                "https://instagram.com",
                "https://instagram.com/",
                "http://instagram.com",
                "http://instagram.com",
                "https://www.instagram.com",
                "https://www.instagram.com/",
                "http://www.instagram.com",
                "http://www.instagram.com/"
        ));
        if (!TextUtils.isEmpty(webViewUrl)) {
            domains.add(0, webViewUrl);
        }

        return getLongestCookie(domains);
    }

    @Nullable
    private static String getLongestCookie(final List<String> domains) {
        int longestLength = 0;
        String longestCookie = null;

        for (final String domain : domains) {
            final String cookie = COOKIE_MANAGER.getCookie(domain);
            if (cookie != null) {
                final int cookieLength = cookie.length();
                if (cookieLength > longestLength) {
                    longestCookie = cookie;
                    longestLength = cookieLength;
                }
            }
        }

        return longestCookie;
    }
}
