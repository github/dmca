package awais.instagrabber.repositories.requests.directmessages;

import java.util.Collections;
import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;

public class TextBroadcastOptions extends BroadcastOptions {
    private final String text;

    public TextBroadcastOptions(final String clientContext,
                                final ThreadIdOrUserIds threadIdOrUserIds,
                                final String text) {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.TEXT);
        this.text = text;
    }

    @Override
    public Map<String, String> getFormMap() {
        return Collections.singletonMap("text", text);
    }
}
