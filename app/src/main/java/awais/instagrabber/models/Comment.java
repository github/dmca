package awais.instagrabber.models;

import androidx.annotation.NonNull;

import java.io.Serializable;
import java.util.Date;
import java.util.Objects;

import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Utils;

public class Comment implements Serializable, Cloneable {
    private final User user;
    private final String id;
    private final String text;
    private long likes;
    private final long timestamp;
    private boolean liked;
    private final int replyCount;
    private final boolean isChild;

    public Comment(final String id,
                   final String text,
                   final long timestamp,
                   final long likes,
                   final boolean liked,
                   final User user,
                   final int replyCount, final boolean isChild) {
        this.id = id;
        this.text = text;
        this.likes = likes;
        this.liked = liked;
        this.timestamp = timestamp;
        this.user = user;
        this.replyCount = replyCount;
        this.isChild = isChild;
    }

    public String getId() {
        return id;
    }

    public String getText() {
        return text;
    }

    @NonNull
    public String getDateTime() {
        return Utils.datetimeParser.format(new Date(timestamp * 1000L));
    }

    public long getLikes() {
        return likes;
    }

    public boolean getLiked() {
        return liked;
    }

    public void setLiked(boolean liked) {
        this.likes = liked ? likes + 1 : likes - 1;
        this.liked = liked;
    }

    public User getUser() {
        return user;
    }

    public int getReplyCount() {
        return replyCount;
    }

    public boolean isChild() {
        return isChild;
    }

    @NonNull
    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Comment comment = (Comment) o;
        return likes == comment.likes &&
                timestamp == comment.timestamp &&
                liked == comment.liked &&
                replyCount == comment.replyCount &&
                Objects.equals(user, comment.user) &&
                Objects.equals(id, comment.id) &&
                Objects.equals(text, comment.text) &&
                isChild == comment.isChild;
    }

    @Override
    public int hashCode() {
        return Objects.hash(user, id, text, likes, timestamp, liked, replyCount, isChild);
    }

    @NonNull
    @Override
    public String toString() {
        return "Comment{" +
                "user=" + user +
                ", id='" + id + '\'' +
                ", text='" + text + '\'' +
                ", likes=" + likes +
                ", timestamp=" + timestamp +
                ", liked=" + liked +
                ", replyCount" + replyCount +
                ", isChild" + isChild +
                '}';
    }
}