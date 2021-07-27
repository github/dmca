package awais.instagrabber.repositories.responses.directmessages;

import androidx.annotation.NonNull;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public class DirectItemReactions implements Cloneable, Serializable {
    private List<DirectItemEmojiReaction> emojis;
    private List<DirectItemEmojiReaction> likes;

    public DirectItemReactions(final List<DirectItemEmojiReaction> emojis,
                               final List<DirectItemEmojiReaction> likes) {
        this.emojis = emojis;
        this.likes = likes;
    }

    public List<DirectItemEmojiReaction> getEmojis() {
        return emojis;
    }

    public List<DirectItemEmojiReaction> getLikes() {
        return likes;
    }

    public void setLikes(final List<DirectItemEmojiReaction> likes) {
        this.likes = likes;
    }

    public void setEmojis(final List<DirectItemEmojiReaction> emojis) {
        this.emojis = emojis;
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
        final DirectItemReactions that = (DirectItemReactions) o;
        return Objects.equals(emojis, that.emojis) &&
                Objects.equals(likes, that.likes);
    }

    @Override
    public int hashCode() {
        return Objects.hash(emojis, likes);
    }

    @Override
    public String toString() {
        return "DirectItemReactions{" +
                "emojis=" + emojis +
                ", likes=" + likes +
                '}';
    }
}
