package awais.instagrabber.repositories.responses.directmessages;

import java.io.Serializable;
import java.util.List;
import java.util.Objects;

public final class DirectItemVideoCallEvent implements Serializable {
    private final String action;
    private final String encodedServerDataInfo;
    private final String description;
    private final boolean threadHasAudioOnlyCall;
    private final List<DirectItemActionLog.TextRange> textAttributes;

    public DirectItemVideoCallEvent(final String action,
                                    final String encodedServerDataInfo,
                                    final String description,
                                    final boolean threadHasAudioOnlyCall,
                                    final List<DirectItemActionLog.TextRange> textAttributes) {
        this.action = action;
        this.encodedServerDataInfo = encodedServerDataInfo;
        this.description = description;
        this.threadHasAudioOnlyCall = threadHasAudioOnlyCall;
        this.textAttributes = textAttributes;
    }

    public String getAction() {
        return action;
    }

    public String getEncodedServerDataInfo() {
        return encodedServerDataInfo;
    }

    public String getDescription() {
        return description;
    }

    public boolean isThreadHasAudioOnlyCall() {
        return threadHasAudioOnlyCall;
    }

    public List<DirectItemActionLog.TextRange> getTextAttributes() {
        return textAttributes;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectItemVideoCallEvent that = (DirectItemVideoCallEvent) o;
        return threadHasAudioOnlyCall == that.threadHasAudioOnlyCall &&
                Objects.equals(action, that.action) &&
                Objects.equals(encodedServerDataInfo, that.encodedServerDataInfo) &&
                Objects.equals(description, that.description) &&
                Objects.equals(textAttributes, that.textAttributes);
    }

    @Override
    public int hashCode() {
        return Objects.hash(action, encodedServerDataInfo, description, threadHasAudioOnlyCall, textAttributes);
    }
}
