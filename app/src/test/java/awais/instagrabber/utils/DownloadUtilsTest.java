package awais.instagrabber.utils;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class DownloadUtilsTest {

    @Test
    void getFileExtensionFromUrl() {
        String extension = DownloadUtils.getFileExtensionFromUrl(
                "https://scontent-nrt1-1.cdninstagram.com/v/t51.2885-15/e35/119960990_327287531705166_2607097160272969998_n.jpg?_nc_ht=scontent-nrt1-1.cdninstagram.com&_nc_cat=1&_nc_ohc=Wu8uveQFmwkAX9qy7kN&_nc_tp=18&oh=f2f93b9eecc1b7dee2b43252c9cf451e&oe=5F9418F4");
        Assertions.assertEquals("jpg", extension);
        extension = DownloadUtils.getFileExtensionFromUrl(
                "https://scontent-nrt1-1.cdninstagram.com/v/t50.2886-16/119894911_194292172085874_5977032350348410166_n.mp4?_nc_ht=scontent-nrt1-1.cdninstagram.com&_nc_cat=106&_nc_ohc=nLErTQ4bU0kAX9XJ4Of&oe=5F6C6B8C&oh=682532b4a941181a6b89736a9002b890");
        Assertions.assertEquals("mp4", extension);
    }

    @Test
    void testDownloadPercent() {
        // int position = 3;
        int total = 6;
        // int currentPercent = 100;
        // ans: (((100 * 1/ 6) ) * (100 / 100)) + (100 * (position - 1)) / total
        for (int position = 1; position <= total; position++) {
            for (float currentPercent = 1; currentPercent <= 100; currentPercent++) {
                float totalPercent = (100f * (position - 1) / total) + (1f / total) * (currentPercent);
                System.out.println(totalPercent);
            }
        }
    }
}