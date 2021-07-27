package awais.instagrabber.models.stickers;

import java.io.Serializable;

public final class QuizModel implements Serializable {
    private final String id, question;
    private final String[] choices;
    private Long[] counts;
    private int mychoice;

    public QuizModel(final String id, final String question, final String[] choices, final Long[] counts, final int mychoice) {
        this.id = id; // only the poll id
        this.question = question;
        this.choices = choices;
        this.counts = counts;
        this.mychoice = mychoice;
    }

    public String getId() {
        return id;
    }

    public String getQuestion() {
        return question;
    }

    public String[] getChoices() { return choices;}

    public Long[] getCounts() { return counts;}

    public int getMyChoice() { return mychoice; }

    public void setMyChoice(final int choice) {
        this.mychoice = choice;
        counts[choice] += 1L;
    }
}