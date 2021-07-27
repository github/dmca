package awais.instagrabber.db.entities;

import androidx.annotation.NonNull;
import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.Index;
import androidx.room.PrimaryKey;

import java.time.LocalDateTime;
import java.util.Objects;

@Entity(tableName = DMLastNotified.TABLE_NAME, indices = {@Index(value = DMLastNotified.COL_THREAD_ID, unique = true)})
public class DMLastNotified {
    public final static String TABLE_NAME = "dm_last_notified";
    public final static String COL_ID = "id";
    public final static String COL_THREAD_ID = "thread_id";
    public final static String COL_LAST_NOTIFIED_MSG_TS = "last_notified_msg_ts";
    public final static String COL_LAST_NOTIFIED_AT = "last_notified_at";

    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = COL_ID)
    private final int id;

    @ColumnInfo(name = COL_THREAD_ID)
    private final String threadId;

    @ColumnInfo(name = COL_LAST_NOTIFIED_MSG_TS)
    private final LocalDateTime lastNotifiedMsgTs;

    @ColumnInfo(name = COL_LAST_NOTIFIED_AT)
    private final LocalDateTime lastNotifiedAt;

    public DMLastNotified(final int id,
                          final String threadId,
                          final LocalDateTime lastNotifiedMsgTs,
                          final LocalDateTime lastNotifiedAt) {
        this.id = id;
        this.threadId = threadId;
        this.lastNotifiedMsgTs = lastNotifiedMsgTs;
        this.lastNotifiedAt = lastNotifiedAt;
    }

    public int getId() {
        return id;
    }

    public String getThreadId() {
        return threadId;
    }

    public LocalDateTime getLastNotifiedMsgTs() {
        return lastNotifiedMsgTs;
    }

    public LocalDateTime getLastNotifiedAt() {
        return lastNotifiedAt;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DMLastNotified that = (DMLastNotified) o;
        return id == that.id &&
                Objects.equals(threadId, that.threadId) &&
                Objects.equals(lastNotifiedMsgTs, that.lastNotifiedMsgTs) &&
                Objects.equals(lastNotifiedAt, that.lastNotifiedAt);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, threadId, lastNotifiedMsgTs, lastNotifiedAt);
    }

    @NonNull
    @Override
    public String toString() {
        return "DMLastNotified{" +
                "id=" + id +
                ", threadId='" + threadId + '\'' +
                ", lastNotifiedMsgTs='" + lastNotifiedMsgTs + '\'' +
                ", lastNotifiedAt='" + lastNotifiedAt + '\'' +
                '}';
    }
}
