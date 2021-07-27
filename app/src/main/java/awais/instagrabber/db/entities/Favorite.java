package awais.instagrabber.db.entities;

import androidx.annotation.NonNull;
import androidx.core.util.ObjectsCompat;
import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.PrimaryKey;

import java.util.Date;

import awais.instagrabber.models.enums.FavoriteType;

@Entity(tableName = Favorite.TABLE_NAME)
public class Favorite {
    public final static String TABLE_NAME = "favorites";
    public final static String COL_ID = "id";
    public final static String COL_QUERY = "query_text";
    public final static String COL_TYPE = "type";
    public final static String COL_DISPLAY_NAME = "display_name";
    public final static String COL_PIC_URL = "pic_url";
    public final static String COL_DATE_ADDED = "date_added";

    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = COL_ID)
    private final int id;

    @ColumnInfo(name = COL_QUERY)
    private final String query;

    @ColumnInfo(name = COL_TYPE)
    private final FavoriteType type;

    @ColumnInfo(name = COL_DISPLAY_NAME)
    private final String displayName;

    @ColumnInfo(name = COL_PIC_URL)
    private final String picUrl;

    @ColumnInfo(name = COL_DATE_ADDED)
    private final Date dateAdded;

    public Favorite(final int id,
                    final String query,
                    final FavoriteType type,
                    final String displayName,
                    final String picUrl,
                    final Date dateAdded) {
        this.id = id;
        this.query = query;
        this.type = type;
        this.displayName = displayName;
        this.picUrl = picUrl;
        this.dateAdded = dateAdded;
    }

    public int getId() {
        return id;
    }

    public String getQuery() {
        return query;
    }

    public FavoriteType getType() {
        return type;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getPicUrl() {
        return picUrl;
    }

    public Date getDateAdded() {
        return dateAdded;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Favorite that = (Favorite) o;
        return id == that.id &&
                ObjectsCompat.equals(query, that.query) &&
                type == that.type &&
                ObjectsCompat.equals(displayName, that.displayName) &&
                ObjectsCompat.equals(picUrl, that.picUrl) &&
                ObjectsCompat.equals(dateAdded, that.dateAdded);
    }

    @Override
    public int hashCode() {
        return ObjectsCompat.hash(id, query, type, displayName, picUrl, dateAdded);
    }

    @NonNull
    @Override
    public String toString() {
        return "FavoriteModel{" +
                "id=" + id +
                ", query='" + query + '\'' +
                ", type=" + type +
                ", displayName='" + displayName + '\'' +
                ", picUrl='" + picUrl + '\'' +
                ", dateAdded=" + dateAdded +
                '}';
    }
}
