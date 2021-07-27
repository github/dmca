package awais.instagrabber.models;

import awais.instagrabber.models.enums.IntentModelType;

public final class IntentModel {
    private final IntentModelType type;
    private final String text;

    public IntentModel(final IntentModelType type, final String text) {
        this.type = type;
        this.text = text;
    }

    public IntentModelType getType() {
        return type;
    }

    public String getText() {
        return text;
    }
}