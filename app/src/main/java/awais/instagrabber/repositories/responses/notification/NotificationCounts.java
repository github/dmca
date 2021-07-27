package awais.instagrabber.repositories.responses.notification;

public class NotificationCounts {
    private final int commentLikes;
    private final int usertags;
    private final int likes;
    private final int comments;
    private final int relationships;
    private final int photosOfYou;
    private final int requests;

    public NotificationCounts(final int commentLikes,
                              final int usertags,
                              final int likes,
                              final int comments,
                              final int relationships,
                              final int photosOfYou,
                              final int requests) {
        this.commentLikes = commentLikes;
        this.usertags = usertags;
        this.likes = likes;
        this.comments = comments;
        this.relationships = relationships;
        this.photosOfYou = photosOfYou;
        this.requests = requests;
    }

    public int getRelationshipsCount() {
        return relationships;
    }

    public int getUserTagsCount() {
        return usertags;
    }

    public int getCommentsCount() {
        return comments;
    }

    public int getCommentLikesCount() {
        return commentLikes;
    }

    public int getLikesCount() {
        return likes;
    }

    public int getPOYCount() {
        return photosOfYou;
    }

    public int getRequestsCount() {
        return requests;
    }
}
