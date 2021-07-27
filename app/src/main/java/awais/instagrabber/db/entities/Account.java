package awais.instagrabber.db.entities;

import androidx.annotation.NonNull;
import androidx.core.util.ObjectsCompat;
import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.Ignore;
import androidx.room.PrimaryKey;

import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.TextUtils;

@Entity(tableName = Account.TABLE_NAME)
public class Account {
    public final static String TABLE_NAME = "accounts";
    public final static String COL_ID = "id";
    public final static String COL_USERNAME = Constants.EXTRAS_USERNAME;
    public final static String COL_COOKIE = "cookie";
    public final static String COL_UID = "uid";
    public final static String COL_FULL_NAME = "full_name";
    public final static String COL_PROFILE_PIC = "profile_pic";

    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = COL_ID)
    private final int id;

    @ColumnInfo(name = COL_UID)
    private final String uid;

    @ColumnInfo(name = COL_USERNAME)
    private final String username;

    @ColumnInfo(name = COL_COOKIE)
    private final String cookie;

    @ColumnInfo(name = COL_FULL_NAME)
    private final String fullName;

    @ColumnInfo(name = COL_PROFILE_PIC)
    private final String profilePic;

    @Ignore
    private boolean selected;

    public Account(final int id,
                   final String uid,
                   final String username,
                   final String cookie,
                   final String fullName,
                   final String profilePic) {
        this.id = id;
        this.uid = uid;
        this.username = username;
        this.cookie = cookie;
        this.fullName = fullName;
        this.profilePic = profilePic;
    }

    public int getId() {
        return id;
    }

    public String getUid() {
        return uid;
    }

    public String getUsername() {
        return username;
    }

    public String getCookie() {
        return cookie;
    }

    public String getFullName() {
        return fullName;
    }

    public String getProfilePic() {
        return profilePic;
    }

    public boolean isSelected() {
        return selected;
    }

    public void setSelected(final boolean selected) {
        this.selected = selected;
    }

    public boolean isValid() {
        return !TextUtils.isEmpty(uid)
                && !TextUtils.isEmpty(username)
                && !TextUtils.isEmpty(cookie);
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Account that = (Account) o;
        return ObjectsCompat.equals(uid, that.uid) &&
                ObjectsCompat.equals(username, that.username) &&
                ObjectsCompat.equals(cookie, that.cookie);
    }

    @Override
    public int hashCode() {
        return ObjectsCompat.hash(uid, username, cookie);
    }

    @NonNull
    @Override
    public String toString() {
        return "Account{" +
                "uid='" + uid + '\'' +
                ", username='" + username + '\'' +
                ", cookie='" + cookie + '\'' +
                ", fullName='" + fullName + '\'' +
                ", profilePic='" + profilePic + '\'' +
                ", selected=" + selected +
                '}';
    }
}
