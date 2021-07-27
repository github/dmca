package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.Objects;

public class DirectItemLink implements Serializable {
    private final String text;
    private final DirectItemLinkContext linkContext;
    private final String clientContext;
    private final String mutationToken;

    public DirectItemLink(final String text,
                          final DirectItemLinkContext linkContext,
                          final String clientContext,
                          final String mutationToken) {
        this.text = text;
        this.linkContext = linkContext;
        this.clientContext = clientContext;
        this.mutationToken = mutationToken;
    }

    public String getText() {
        return text;
    }

    public DirectItemLinkContext getLinkContext() {
        return linkContext;
    }

    public String getClientContext() {
        return clientContext;
    }

    public String getMutationToken() {
        return mutationToken;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemLink that = (DirectItemLink) o;
        return Objects.equals(text, that.text) &&
                Objects.equals(linkContext, that.linkContext) &&
                Objects.equals(clientContext, that.clientContext) &&
                Objects.equals(mutationToken, that.mutationToken);
    }

    @Override
    public int hashCode() {
        return Objects.hash(text, linkContext, clientContext, mutationToken);
    }
}
