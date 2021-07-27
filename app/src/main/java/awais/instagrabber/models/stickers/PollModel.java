package awais.instagrabber.models.stickers;

import java.io.Serializable;

public final class PollModel implements Serializable {
    private int leftcount, rightcount, mychoice;
    private final String id, question, leftchoice, rightchoice;

    public PollModel(final String id, final String question, final String leftchoice, final int leftcount,
                     final String rightchoice, final int rightcount, final int mychoice) {
        this.id = id; // only the poll id
        this.question = question;
        this.leftchoice = leftchoice;
        this.leftcount = leftcount;
        this.rightchoice = rightchoice;
        this.rightcount = rightcount;
        this.mychoice = mychoice;
    }

    public String getId() {
        return id;
    }

    public String getQuestion() {
        return question;
    }

    public String getLeftChoice() {
        return leftchoice;
    }

    public int getLeftCount() {
        return leftcount;
    }

    public String getRightChoice() {
        return rightchoice;
    }

    public int getRightCount() {
        return rightcount;
    }

    public int getMyChoice() { return mychoice; }

    public int setMyChoice(final int choice) {
        this.mychoice = choice;
        if (choice == 0) this.leftcount += 1;
        else if (choice == 1) this.rightcount += 1;
        return choice;
    }
}