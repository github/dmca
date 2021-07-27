// java.lang.RuntimeException: Method parse in android.net.Uri not mocked.
// See http://g.co/androidstudio/not-mocked for details.

package awais.instagrabber.utils;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import awais.instagrabber.models.IntentModel;
import awais.instagrabber.models.enums.IntentModelType;

class IntentUtilsTest {

    @Test
    void getIntentFromUrl() {
        IntentModel intent = IntentUtils.parseUrl("https://instagr.am/austinhuang.me");
        Assertions.assertEquals(new IntentModel(IntentModelType.USERNAME, "austinhuang.me"), intent);
        intent = IntentUtils.parseUrl("https://www.instagr.am/_u/austinhuang.me");
        Assertions.assertEquals(new IntentModel(IntentModelType.USERNAME, "austinhuang.me"), intent);
        intent = IntentUtils.parseUrl("https://instagram.com/p/BmjKdkxjzO7/");
        Assertions.assertEquals(new IntentModel(IntentModelType.POST, "BmjKdkxjzO7"), intent);
        intent = IntentUtils.parseUrl("https://www.instagram.com/explore/tags/metrodemontreal/");
        Assertions.assertEquals(new IntentModel(IntentModelType.HASHTAG, "metrodemontreal"), intent);
        intent = IntentUtils.parseUrl("http://www.instagram.com/explore/locations/538444610/abcde");
        Assertions.assertEquals(new IntentModel(IntentModelType.LOCATION, "538444610"), intent);
        // todo: reel and igtv test cases that are sfw and preferably n i c e
    }
}