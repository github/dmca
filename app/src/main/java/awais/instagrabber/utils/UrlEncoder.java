package awais.instagrabber.utils;

import androidx.annotation.NonNull;

import java.io.CharArrayWriter;
import java.util.BitSet;

// same as java.net.URLEncoder
public final class UrlEncoder {
    private static final BitSet dontNeedEncoding = new BitSet(256);
    private static final int caseDiff = ('a' - 'A');

    static {
        int i;
        for (i = 'a'; i <= 'z'; i++) dontNeedEncoding.set(i);
        for (i = 'A'; i <= 'Z'; i++) dontNeedEncoding.set(i);
        for (i = '0'; i <= '9'; i++) dontNeedEncoding.set(i);
        dontNeedEncoding.set(' ');
        dontNeedEncoding.set('-');
        dontNeedEncoding.set('_');
        dontNeedEncoding.set('.');
        dontNeedEncoding.set('*');
    }

    @NonNull
    public static String encodeUrl(@NonNull final String s) {
        final StringBuilder out = new StringBuilder(s.length());
        final CharArrayWriter charArrayWriter = new CharArrayWriter();

        boolean needToChange = false;
        for (int i = 0; i < s.length(); ) {
            int c = s.charAt(i);

            if (dontNeedEncoding.get(c)) {
                if (c == ' ') {
                    c = '+';
                    needToChange = true;
                }

                out.append((char) c);
                i++;
            } else {
                do {
                    charArrayWriter.write(c);
                    if (c >= 0xD800 && c <= 0xDBFF && i + 1 < s.length()) {
                        final int d = s.charAt(i + 1);
                        if (d >= 0xDC00 && d <= 0xDFFF) {
                            charArrayWriter.write(d);
                            i++;
                        }
                    }
                    i++;
                } while (i < s.length() && !dontNeedEncoding.get(c = s.charAt(i)));

                charArrayWriter.flush();

                final byte[] ba = charArrayWriter.toString().getBytes();
                for (final byte b : ba) {
                    out.append('%');
                    char ch = Character.forDigit((b >> 4) & 0xF, 16);
                    if (Character.isLetter(ch)) ch -= caseDiff;
                    out.append(ch);
                    ch = Character.forDigit(b & 0xF, 16);
                    if (Character.isLetter(ch)) ch -= caseDiff;
                    out.append(ch);
                }
                charArrayWriter.reset();
                needToChange = true;
            }
        }

        return (needToChange ? out.toString() : s);
    }
}