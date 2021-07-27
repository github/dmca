package awais.instagrabber.db.entities;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.Ignore;
import androidx.room.Index;
import androidx.room.PrimaryKey;

import java.time.LocalDateTime;
import java.util.Objects;

import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.repositories.responses.search.SearchItem;

@Entity(tableName = RecentSearch.TABLE_NAME, indices = {@Index(value = {RecentSearch.COL_IG_ID, RecentSearch.COL_TYPE}, unique = true)})
public class RecentSearch {
    private static final String TAG = RecentSearch.class.getSimpleName();

    public static final String TABLE_NAME = "recent_searches";
    private static final String COL_ID = "id";
    public static final String COL_IG_ID = "ig_id";
    private static final String COL_NAME = "name";
    private static final String COL_USERNAME = "username";
    private static final String COL_PIC_URL = "pic_url";
    public static final String COL_TYPE = "type";
    private static final String COL_LAST_SEARCHED_ON = "last_searched_on";

    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = COL_ID)
    private final int id;

    @ColumnInfo(name = COL_IG_ID)
    @NonNull
    private final String igId;

    @ColumnInfo(name = COL_NAME)
    @NonNull
    private final String name;

    @ColumnInfo(name = COL_USERNAME)
    private final String username;

    @ColumnInfo(name = COL_PIC_URL)
    private final String picUrl;

    @ColumnInfo(name = COL_TYPE)
    @NonNull
    private final FavoriteType type;

    @ColumnInfo(name = COL_LAST_SEARCHED_ON)
    @NonNull
    private final LocalDateTime lastSearchedOn;

    @Ignore
    public RecentSearch(final String igId,
                        final String name,
                        final String username,
                        final String picUrl,
                        final FavoriteType type,
                        final LocalDateTime lastSearchedOn) {
        this(0, igId, name, username, picUrl, type, lastSearchedOn);
    }

    public RecentSearch(final int id,
                        @NonNull final String igId,
                        @NonNull final String name,
                        final String username,
                        final String picUrl,
                        @NonNull final FavoriteType type,
                        @NonNull final LocalDateTime lastSearchedOn) {
        this.id = id;
        this.igId = igId;
        this.name = name;
        this.username = username;
        this.picUrl = picUrl;
        this.type = type;
        this.lastSearchedOn = lastSearchedOn;
    }

    public int getId() {
        return id;
    }

    @NonNull
    public String getIgId() {
        return igId;
    }

    @NonNull
    public String getName() {
        return name;
    }

    public String getUsername() {
        return username;
    }

    public String getPicUrl() {
        return picUrl;
    }

    @NonNull
    public FavoriteType getType() {
        return type;
    }

    @NonNull
    public LocalDateTime getLastSearchedOn() {
        return lastSearchedOn;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final RecentSearch that = (RecentSearch) o;
        return Objects.equals(igId, that.igId) &&
                Objects.equals(name, that.name) &&
                Objects.equals(username, that.username) &&
                Objects.equals(picUrl, that.picUrl) &&
                type == that.type &&
                Objects.equals(lastSearchedOn, that.lastSearchedOn);
    }

    @Override
    public int hashCode() {
        return Objects.hash(igId, name, username, picUrl, type, lastSearchedOn);
    }

    @NonNull
    @Override
    public String toString() {
        return "RecentSearch{" +
                "id=" + id +
                ", igId='" + igId + '\'' +
                ", name='" + name + '\'' +
                ", username='" + username + '\'' +
                ", picUrl='" + picUrl + '\'' +
                ", type=" + type +
                ", lastSearchedOn=" + lastSearchedOn +
                '}';
    }

    @Nullable
    public static RecentSearch fromSearchItem(@NonNull final SearchItem searchItem) {
        final FavoriteType type = searchItem.getType();
        if (type == null) return null;
        try {
            final String igId;
            final String name;
            final String username;
            final String picUrl;
            switch (type) {
                case USER:
                    igId = String.valueOf(searchItem.getUser().getPk());
                    name = searchItem.getUser().getFullName();
                    username = searchItem.getUser().getUsername();
                    picUrl = searchItem.getUser().getProfilePicUrl();
                    break;
                case HASHTAG:
                    igId = searchItem.getHashtag().getId();
                    name = searchItem.getHashtag().getName();
                    username = null;
                    picUrl = null;
                    break;
                case LOCATION:
                    igId = String.valueOf(searchItem.getPlace().getLocation().getPk());
                    name = searchItem.getPlace().getTitle();
                    username = null;
                    picUrl = null;
                    break;
                default:
                    return null;
            }
            return new RecentSearch(igId, name, username, picUrl, type, LocalDateTime.now());
        } catch (Exception e) {
            Log.e(TAG, "fromSearchItem: ", e);
        }
        return null;
    }
}
