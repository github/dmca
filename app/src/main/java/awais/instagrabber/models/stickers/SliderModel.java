package awais.instagrabber.models.stickers;

import java.io.Serializable;

public final class SliderModel implements Serializable {
    private final int voteCount;
    private final Double average;
    private Double myChoice;
    private final boolean canVote;
    private final String id, question, emoji;

    public SliderModel(final String id, final String question, final String emoji, final boolean canVote,
                       final Double average, final int voteCount, final Double myChoice) {
        this.id = id;
        this.question = question;
        this.emoji = emoji;
        this.canVote = canVote;
        this.average = average;
        this.voteCount = voteCount;
        this.myChoice = myChoice;
    }

    public String getId() {
        return id;
    }

    public String getQuestion() {
        return question;
    }

    public String getEmoji() {
        return emoji;
    }

    public boolean canVote() {
        return canVote;
    }

    public int getVoteCount() {
        return voteCount;
    }

    public Double getAverage() {
        return average;
    }

    public Double getMyChoice() { return myChoice; }

    public Double setMyChoice(final Double choice) {
        this.myChoice = choice;
        return choice;
    }
}