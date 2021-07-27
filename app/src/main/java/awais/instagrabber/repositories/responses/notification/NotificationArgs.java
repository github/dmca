package awais.instagrabber.repositories.responses.notification;

import androidx.annotation.NonNull;

import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import awais.instagrabber.utils.Utils;

public class NotificationArgs {
    private final String text;
    private final String richText;
    private final long profileId;
    private final String profileImage;
    private final List<NotificationImage> media;
    private final double timestamp;
    private final String profileName;
    private final String fullName; // for AYML, not naturally generated
    private final boolean isVerified; // mostly for AYML, not sure about notif

    public NotificationArgs(final String text,
                            final String richText, // for AYML, this is the algorithm
                            final long profileId,
                            final String profileImage,
                            final List<NotificationImage> media,
                            final double timestamp,
                            final String profileName,
                            final String fullName,
                            final boolean isVerified) {
        this.text = text;
        this.richText = richText;
        this.profileId = profileId;
        this.profileImage = profileImage;
        this.media = media;
        this.timestamp = timestamp;
        this.profileName = profileName;
        this.fullName = fullName;
        this.isVerified = isVerified;
    }

    public String getText() {
        return text == null ? cleanRichText(richText) : text;
    }

    public long getUserId() {
        return profileId;
    }

    public String getProfilePic() {
        return profileImage;
    }

    public String getUsername() {
        return profileName;
    }

    public String getFullName() {
        return fullName;
    }

    public List<NotificationImage> getMedia() {
        return media;
    }

    public double getTimestamp() {
        return timestamp;
    }

    public boolean isVerified() {
        return isVerified;
    }

    @NonNull
    public String getDateTime() {
        return Utils.datetimeParser.format(new Date(Math.round(timestamp * 1000)));
    }

    private String cleanRichText(final String raw) {
        if (raw == null) return null;
        final Matcher matcher = Pattern.compile("\\{[\\p{L}\\d._]+\\|000000\\|1\\|user\\?id=\\d+\\}").matcher(raw);
        String result = raw;
        while (matcher.find()) {
            final String richObject = raw.substring(matcher.start(), matcher.end());
            final String username = richObject.split("\\|")[0].substring(1);
            result = result.replace(richObject, username);
        }
        return result;
    }
}
