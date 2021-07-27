package awais.instagrabber.utils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Locale;
import java.util.Random;

public final class NumberUtils {
    // @NonNull
    // public static String millisToString(final long timeMs) {
    //     final long totalSeconds = timeMs / 1000;
    //
    //     final long seconds = totalSeconds % 60;
    //     final long minutes = totalSeconds / 60 % 60;
    //     final long hours = totalSeconds / 3600;
    //
    //     final String strSec = Long.toString(seconds);
    //     final String strMin = Long.toString(minutes);
    //
    //     final String strRetSec = strSec.length() > 1 ? strSec : "0" + seconds;
    //     final String strRetMin = strMin.length() > 1 ? strMin : "0" + minutes;
    //
    //     final String retMinSec = strRetMin + ':' + strRetSec;
    //
    //     if (hours > 0)
    //         return Long.toString(hours) + ':' + retMinSec;
    //     return retMinSec;
    // }

    public static int getResultingHeight(final int requiredWidth, final int height, final int width) {
        return requiredWidth * height / width;
    }

    public static int getResultingWidth(final int requiredHeight, final int height, final int width) {
        return requiredHeight * width / height;
    }

    public static long random(long origin, long bound) {
        final Random random = new Random();
        long r = random.nextLong();
        long n = bound - origin, m = n - 1;
        if ((n & m) == 0L)  // power of two
            r = (r & m) + origin;
        else if (n > 0L) {  // reject over-represented candidates
            //noinspection StatementWithEmptyBody
            for (long u = r >>> 1;            // ensure non-negative
                 u + m - (r = u % n) < 0L;    // rejection check
                 u = random.nextLong() >>> 1) // retry
                ;
            r += origin;
        } else {              // range not representable as long
            while (r < origin || r >= bound)
                r = random.nextLong();
        }
        return r;
    }

    @NonNull
    public static NullSafePair<Integer, Integer> calculateWidthHeight(final int height, final int width, final int maxHeight, final int maxWidth) {
        if (width > maxWidth) {
            int tempHeight = getResultingHeight(maxWidth, height, width);
            int tempWidth = maxWidth;
            if (tempHeight > maxHeight) {
                tempWidth = getResultingWidth(maxHeight, tempHeight, tempWidth);
                tempHeight = maxHeight;
            }
            return new NullSafePair<>(tempWidth, tempHeight);
        }
        if ((height < maxHeight && width < maxWidth) || (height > maxHeight)) {
            int tempWidth = getResultingWidth(maxHeight, height, width);
            int tempHeight = maxHeight;
            if (tempWidth > maxWidth) {
                tempHeight = getResultingHeight(maxWidth, tempHeight, tempWidth);
                tempWidth = maxWidth;
            }
            return new NullSafePair<>(tempWidth, tempHeight);
        }
        return new NullSafePair<>(width, height);
    }

    public static float roundFloat2Decimals(final float value) {
        return ((int) ((value + (value >= 0 ? 1 : -1) * 0.005f) * 100)) / 100f;
    }

    @NonNull
    public static String abbreviate(final long number) {
        return abbreviate(number, null);
    }


    @NonNull
    public static String abbreviate(final long number, @Nullable final AbbreviateOptions options) {
        // adapted from https://stackoverflow.com/a/9769590/1436766
        int threshold = 1000;
        boolean addSpace = false;
        if (options != null) {
            threshold = options.getThreshold();
            addSpace = options.addSpaceBeforePrefix();
        }
        if (number < threshold) return "" + number;
        int exp = (int) (Math.log(number) / Math.log(threshold));
        return String.format(Locale.US,
                             "%.1f%s%c",
                             number / Math.pow(threshold, exp),
                             addSpace ? " " : "",
                             "kMGTPE".charAt(exp - 1));
    }

    public static final class AbbreviateOptions {
        private final int threshold;
        private final boolean addSpaceBeforePrefix;

        public static final class Builder {

            private int threshold = 1000;
            private boolean addSpaceBeforePrefix = false;

            public Builder setThreshold(final int threshold) {
                this.threshold = threshold;
                return this;
            }

            public Builder setAddSpaceBeforePrefix(final boolean addSpaceBeforePrefix) {
                this.addSpaceBeforePrefix = addSpaceBeforePrefix;
                return this;
            }

            @NonNull
            public AbbreviateOptions build() {
                return new AbbreviateOptions(threshold, addSpaceBeforePrefix);
            }

        }

        private AbbreviateOptions(final int threshold, final boolean addSpaceBeforePrefix) {
            this.threshold = threshold;
            this.addSpaceBeforePrefix = addSpaceBeforePrefix;
        }

        public int getThreshold() {
            return threshold;
        }

        public boolean addSpaceBeforePrefix() {
            return addSpaceBeforePrefix;
        }
    }
}
