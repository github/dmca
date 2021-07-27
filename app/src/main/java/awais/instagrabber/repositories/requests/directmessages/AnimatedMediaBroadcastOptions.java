package awais.instagrabber.repositories.requests.directmessages;

import java.util.HashMap;
import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;
import awais.instagrabber.repositories.responses.giphy.GiphyGif;

public class AnimatedMediaBroadcastOptions extends BroadcastOptions {

    private final GiphyGif giphyGif;

    public AnimatedMediaBroadcastOptions(final String clientContext,
                                         final ThreadIdOrUserIds threadIdOrUserIds,
                                         final GiphyGif giphyGif) {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.ANIMATED_MEDIA);
        this.giphyGif = giphyGif;
    }

    @Override
    public Map<String, String> getFormMap() {
        final Map<String, String> form = new HashMap<>();
        form.put("is_sticker", String.valueOf(giphyGif.isSticker()));
        form.put("id", giphyGif.getId());
        return form;
    }
}
