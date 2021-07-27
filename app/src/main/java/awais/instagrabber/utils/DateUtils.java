package awais.instagrabber.utils;

import androidx.annotation.NonNull;

import java.time.LocalDateTime;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

public final class DateUtils {
    public static boolean isSameDay(final Date date1, final Date date2) {
        if (date1 == null || date2 == null) {
            return false;
        }
        Calendar calendar1 = Calendar.getInstance();
        calendar1.setTime(date1);
        Calendar calendar2 = Calendar.getInstance();
        calendar2.setTime(date2);
        return calendar1.get(Calendar.YEAR) == calendar2.get(Calendar.YEAR)
                && calendar1.get(Calendar.MONTH) == calendar2.get(Calendar.MONTH)
                && calendar1.get(Calendar.DAY_OF_MONTH) == calendar2.get(Calendar.DAY_OF_MONTH);
    }

    public static Date dateAtZeroHours(@NonNull final Date date) {
        final Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        cal.set(Calendar.HOUR_OF_DAY, 0);
        cal.set(Calendar.MINUTE, 0);
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MILLISECOND, 0);
        return cal.getTime();
    }

    public static int getTimezoneOffset() {
        final Calendar calendar = Calendar.getInstance(Locale.getDefault());
        return -(calendar.get(Calendar.ZONE_OFFSET) + calendar.get(Calendar.DST_OFFSET)) / (60 * 1000);
    }

    public static boolean isBeforeOrEqual(final LocalDateTime localDateTime, final LocalDateTime comparedTo) {
        return localDateTime.isBefore(comparedTo) || localDateTime.isEqual(comparedTo);
    }
}
