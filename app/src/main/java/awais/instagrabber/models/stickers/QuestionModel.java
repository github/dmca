package awais.instagrabber.models.stickers;

import java.io.Serializable;

public final class QuestionModel implements Serializable {
    private final String id, question;

    public QuestionModel(final String id, final String question) {
        this.id = id; // only the poll id
        this.question = question;
    }

    public String getId() {
        return id;
    }

    public String getQuestion() {
        return question;
    }
}