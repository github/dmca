package awais.instagrabber.utils;

import android.content.Context;
import android.text.SpannableString;
import android.text.format.DateFormat;
import android.text.format.DateUtils;
import android.text.style.URLSpan;
import android.util.Patterns;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.regex.Matcher;
import java.util.stream.Collectors;

public final class TextUtils {
    // extracted from String class
    public static int indexOfChar(@NonNull final CharSequence sequence, final int ch, final int startIndex) {
        final int max = sequence.length();
        if (startIndex < max) {
            if (ch < Character.MIN_SUPPLEMENTARY_CODE_POINT) {
                for (int i = startIndex; i < max; i++) if (sequence.charAt(i) == ch) return i;
            } else if (Character.isValidCodePoint(ch)) {
                final char hi = (char) ((ch >>> 10) + (Character.MIN_HIGH_SURROGATE - (Character.MIN_SUPPLEMENTARY_CODE_POINT >>> 10)));
                final char lo = (char) ((ch & 0x3ff) + Character.MIN_LOW_SURROGATE);
                for (int i = startIndex; i < max; i++)
                    if (sequence.charAt(i) == hi && sequence.charAt(i + 1) == lo) return i;
            }
        }
        return -1;
    }

    public static CharSequence getSpannableUrl(final String url) {
        if (isEmpty(url)) return url;
        final int httpIndex = url.indexOf("http:");
        final int httpsIndex = url.indexOf("https:");
        if (httpIndex == -1 && httpsIndex == -1) return url;

        final int length = url.length();

        final int startIndex = httpIndex != -1 ? httpIndex : httpsIndex;
        final int spaceIndex = url.indexOf(' ', startIndex + 1);

        final int endIndex = (spaceIndex != -1 ? spaceIndex : length);

        final String extractUrl = url.substring(startIndex, Math.min(length, endIndex));

        final SpannableString spannableString = new SpannableString(url);
        spannableString.setSpan(new URLSpan(extractUrl), startIndex, endIndex, 0);

        return spannableString;
    }

    public static boolean isEmpty(final CharSequence charSequence) {
        if (charSequence == null || charSequence.length() < 1) return true;
        if (charSequence instanceof String) {
            String str = (String) charSequence;
            if ("".equals(str) || "null".equals(str) || str.isEmpty()) return true;
            str = str.trim();
            return "".equals(str) || "null".equals(str) || str.isEmpty();
        }
        return "null".contentEquals(charSequence) || "".contentEquals(charSequence);
    }

    public static String millisToTimeString(final long millis) {
        return millisToTimeString(millis, false);
    }

    public static String millisToTimeString(final long millis, final boolean includeHoursAlways) {
        final int sec = (int) (millis / 1000) % 60;
        int min = (int) (millis / (1000 * 60));
        if (min >= 60) {
            min = (int) ((millis / (1000 * 60)) % 60);
            final int hr = (int) ((millis / (1000 * 60 * 60)) % 24);
            return String.format(Locale.ENGLISH, "%02d:%02d:%02d", hr, min, sec);
        }
        if (includeHoursAlways) {
            return String.format(Locale.ENGLISH, "%02d:%02d:%02d", 0, min, sec);
        }
        return String.format(Locale.ENGLISH, "%02d:%02d", min, sec);
    }

    public static String getRelativeDateTimeString(final Context context, final long from) {
        final Date now = new Date();
        final Date then = new Date(from);
        int days = daysBetween(from, now.getTime());
        if (days == 0) {
            return DateFormat.getTimeFormat(context).format(then);
        }
        return DateFormat.getDateFormat(context).format(then);
    }

    private static int daysBetween(long d1, long d2) {
        return (int) ((d2 - d1) / DateUtils.DAY_IN_MILLIS);
    }

    public static List<String> extractUrls(final String text) {
        if (isEmpty(text)) return Collections.emptyList();
        final Matcher matcher = Patterns.WEB_URL.matcher(text);
        final List<String> urls = new ArrayList<>();
        while (matcher.find()) {
            urls.add(matcher.group());
        }
        return urls;
    }

    // https://github.com/notslang/instagram-id-to-url-segment
    public static long shortcodeToId(final String shortcode) {
        long result = 0L;
        for (int i = 0; i < shortcode.length() && i < 11; i++){
            final char c = shortcode.charAt(i);
            final int k = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_".indexOf(c);
            result = result * 64 + k;
        }
        return result;
    }
}
