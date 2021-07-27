package awais.instagrabber.repositories.requests;

import java.io.Serializable;

public class StoryViewerOptions implements Serializable {
    private final long id;
    private final String name;
    private final Type type;
    private int currentFeedStoryIndex;

    private StoryViewerOptions(final int position, final Type type) {
        id = 0;
        name = null;
        this.currentFeedStoryIndex = position;
        this.type = type;
    }

    private StoryViewerOptions(final String name, final Type type) {
        this.name = name;
        this.id = 0;
        this.type = type;
    }

    private StoryViewerOptions(final long id, final Type type) {
        this.name = null;
        this.id = id;
        this.type = type;
    }

    private StoryViewerOptions(final long id, final String name, final Type type) {
        this.id = id;
        this.name = name;
        this.type = type;
    }

    public static StoryViewerOptions forHashtag(final String name) {
        return new StoryViewerOptions(name, Type.HASHTAG);
    }

    public static StoryViewerOptions forLocation(final long id, final String name) {
        return new StoryViewerOptions(id, name, Type.LOCATION);
    }

    public static StoryViewerOptions forUser(final long id, final String name) {
        return new StoryViewerOptions(id, name,Type.USER);
    }

    public static StoryViewerOptions forHighlight(final String highlight) {
        return new StoryViewerOptions(highlight, Type.HIGHLIGHT);
    }

    public static StoryViewerOptions forStory(final long mediaId, final String username) {
        return new StoryViewerOptions(mediaId, username, Type.STORY);
    }

    public static StoryViewerOptions forFeedStoryPosition(final int position) {
        return new StoryViewerOptions(position, Type.FEED_STORY_POSITION);
    }

    public static StoryViewerOptions forStoryArchive(final String id) {
        return new StoryViewerOptions(id, Type.STORY_ARCHIVE);
    }

    public static StoryViewerOptions forStoryArchive(final int position) {
        return new StoryViewerOptions(position, Type.STORY_ARCHIVE);
    }

    public long getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public Type getType() {
        return type;
    }

    public int getCurrentFeedStoryIndex() {
        return currentFeedStoryIndex;
    }

    public void setCurrentFeedStoryIndex(final int index) {
        this.currentFeedStoryIndex = index;
    }

    public enum Type {
        HASHTAG,
        LOCATION,
        USER,
        HIGHLIGHT,
        STORY,
        FEED_STORY_POSITION,
        STORY_ARCHIVE
    }
}
