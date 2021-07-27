package awais.instagrabber.utils;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class TextUtilsTest {

    @Test
    void testMillisToTimeString() {
        String timeString = TextUtils.millisToTimeString(18000000);
        Assertions.assertEquals("05:00:00", timeString);

        timeString = TextUtils.millisToTimeString(300000);
        Assertions.assertEquals("05:00", timeString);

        timeString = TextUtils.millisToTimeString(300000, true);
        Assertions.assertEquals("00:05:00", timeString);
    }

    @Test
    void testShortcodeConversion() {
        long conversion = TextUtils.shortcodeToId("CA0YnOonSfS");
        Assertions.assertEquals(2320587956892280786L, conversion);
        conversion = TextUtils.shortcodeToId("B_7n8mblwx6gv1ZaNvA5ZhAs2qslMnRiMMYW1c0");
        Assertions.assertEquals(2304611322577751162L, conversion);
    }
}