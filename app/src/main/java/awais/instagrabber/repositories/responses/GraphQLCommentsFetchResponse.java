package awais.instagrabber.repositories.responses;

import androidx.annotation.NonNull;

import java.util.List;

import awais.instagrabber.models.Comment;

public class GraphQLCommentsFetchResponse {
    private final int count;
    private final String cursor;
    private final boolean hasNext;
    private final List<Comment> comments;

    public GraphQLCommentsFetchResponse(final int count,
                                        final String cursor,
                                        final boolean hasNext,
                                        final List<Comment> comments) {
        this.count = count;
        this.cursor = cursor;
        this.hasNext = hasNext;
        this.comments = comments;
    }

    public int getCount() {
        return count;
    }

    public String getCursor() {
        return cursor;
    }

    public boolean hasNext() {
        return hasNext;
    }

    public List<Comment> getComments() {
        return comments;
    }

    @NonNull
    @Override
    public String toString() {
        return "GraphQLCommentsFetchResponse{" +
                "count=" + count +
                ", cursor='" + cursor + '\'' +
                ", hasNext=" + hasNext +
                ", comments=" + comments +
                '}';
    }
}
