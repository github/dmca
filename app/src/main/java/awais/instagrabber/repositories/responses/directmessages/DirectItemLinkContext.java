package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

public class DirectItemLinkContext implements Serializable {
    private final String linkUrl;
    private final String linkTitle;
    private final String linkSummary;
    private final String linkImageUrl;

    public DirectItemLinkContext(final String linkUrl,
                                 final String linkTitle,
                                 final String linkSummary,
                                 final String linkImageUrl) {
        this.linkUrl = linkUrl;
        this.linkTitle = linkTitle;
        this.linkSummary = linkSummary;
        this.linkImageUrl = linkImageUrl;
    }

    public String getLinkUrl() {
        return linkUrl;
    }

    public String getLinkTitle() {
        return linkTitle;
    }

    public String getLinkSummary() {
        return linkSummary;
    }

    public String getLinkImageUrl() {
        return linkImageUrl;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemLinkContext that = (DirectItemLinkContext) o;
        return Objects.equals(linkUrl, that.linkUrl) &&
                Objects.equals(linkTitle, that.linkTitle) &&
                Objects.equals(linkSummary, that.linkSummary) &&
                Objects.equals(linkImageUrl, that.linkImageUrl);
    }

    @Override
    public int hashCode() {
        return Objects.hash(linkUrl, linkTitle, linkSummary, linkImageUrl);
    }
}
