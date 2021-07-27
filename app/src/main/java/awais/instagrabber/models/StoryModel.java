package awais.instagrabber.models;

import java.io.Serializable;

import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.models.stickers.PollModel;
import awais.instagrabber.models.stickers.QuestionModel;
import awais.instagrabber.models.stickers.QuizModel;
import awais.instagrabber.models.stickers.SliderModel;
import awais.instagrabber.models.stickers.SwipeUpModel;

public final class StoryModel implements Serializable {
    private final String storyMediaId;
    private final String storyUrl;
    private String thumbnail;
    private final String username;
    private final long userId;
    private final MediaItemType itemType;
    private final long timestamp;
    private String videoUrl;
    private String tappableShortCode;
    private String tappableId;
    private String spotify;
    private PollModel poll;
    private QuestionModel question;
    private SliderModel slider;
    private QuizModel quiz;
    private SwipeUpModel swipeUp;
    private String[] mentions;
    private int position;
    private boolean isCurrentSlide = false;
    private final boolean canReply;

    public StoryModel(final String storyMediaId, final String storyUrl, final String thumbnail, final MediaItemType itemType,
                      final long timestamp, final String username, final long userId, final boolean canReply) {
        this.storyMediaId = storyMediaId;
        this.storyUrl = storyUrl;
        this.thumbnail = thumbnail;
        this.itemType = itemType;
        this.timestamp = timestamp;
        this.username = username;
        this.userId = userId;
        this.canReply = canReply;
    }

    public String getStoryUrl() {
        return storyUrl;
    }

    public String getThumbnail() {
        return thumbnail;
    }

    public String getStoryMediaId() {
        return storyMediaId;
    }

    public MediaItemType getItemType() {
        return itemType;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public String getVideoUrl() {
        return videoUrl;
    }

    public String getTappableShortCode() {
        return tappableShortCode;
    }

    public PollModel getPoll() {
        return poll;
    }

    public QuestionModel getQuestion() {
        return question;
    }

    public SliderModel getSlider() {
        return slider;
    }

    public QuizModel getQuiz() {
        return quiz;
    }

    public SwipeUpModel getSwipeUp() { return swipeUp;}

    public String[] getMentions() {
        return mentions;
    }

    public int getPosition() {
        return position;
    }

    public void setThumbnail(final String thumbnail) {
        this.thumbnail = thumbnail;
    }

    public void setVideoUrl(final String videoUrl) {
        this.videoUrl = videoUrl;
    }

    public String getSpotify() {
        return spotify;
    }

    public void setSpotify(final String spotify) {
        this.spotify = spotify;
    }

    public void setTappableShortCode(final String tappableShortCode) {
        this.tappableShortCode = tappableShortCode;
    }

    public void setPoll(final PollModel poll) {
        this.poll = poll;
    }

    public void setQuestion(final QuestionModel question) {
        this.question = question;
    }

    public void setSlider(final SliderModel slider) {
        this.slider = slider;
    }

    public void setQuiz(final QuizModel quiz) {
        this.quiz = quiz;
    }

    public void setMentions(final String[] mentions) {
        this.mentions = mentions;
    }

    public void setSwipeUp(final SwipeUpModel swipeUp) {
        this.swipeUp = swipeUp;
    }

    public void setPosition(final int position) {
        this.position = position;
    }

    public void setCurrentSlide(final boolean currentSlide) {
        this.isCurrentSlide = currentSlide;
    }

    public boolean isCurrentSlide() {
        return isCurrentSlide;
    }

    public boolean canReply() {
        return canReply;
    }

    public String getUsername() {
        return username;
    }

    public long getUserId() {
        return userId;
    }
}