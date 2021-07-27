package awais.instagrabber.repositories.requests.directmessages;

import com.google.common.collect.ImmutableMap;

import org.json.JSONArray;

import java.util.List;
import java.util.Map;

import awais.instagrabber.models.enums.BroadcastItemType;

public class LinkBroadcastOptions extends BroadcastOptions {
    private final String linkText;
    private final List<String> urls;

    public LinkBroadcastOptions(final String clientContext,
                                final ThreadIdOrUserIds threadIdOrUserIds,
                                final String linkText,
                                final List<String> urls) {
        super(clientContext, threadIdOrUserIds, BroadcastItemType.LINK);
        this.linkText = linkText;
        this.urls = urls;
    }

    @Override
    public Map<String, String> getFormMap() {
        return ImmutableMap.of(
                "link_text", linkText,
                "link_urls", new JSONArray(urls).toString()
        );
    }
}
