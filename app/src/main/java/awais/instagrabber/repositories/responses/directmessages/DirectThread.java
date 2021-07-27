package awais.instagrabber.repositories.responses.directmessages;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.Serializable;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import awais.instagrabber.repositories.responses.User;

public class DirectThread implements Serializable, Cloneable {
    private final String threadId;
    private final String threadV2Id;
    private List<User> users;
    private List<User> leftUsers;
    private List<Long> adminUserIds;
    private List<DirectItem> items;
    private final long lastActivityAt;
    private boolean muted;
    private final boolean isPin;
    private final boolean named;
    private final boolean canonical;
    private boolean pending;
    private final boolean archived;
    private final boolean valuedRequest;
    private final String threadType;
    private final long viewerId;
    private final String threadTitle;
    private final String pendingScore;
    private final long folder;
    private final boolean vcMuted;
    private final boolean isGroup;
    private boolean mentionsMuted;
    private final User inviter;
    private final boolean hasOlder;
    private final boolean hasNewer;
    private Map<Long, DirectThreadLastSeenAt> lastSeenAt;
    private final String newestCursor;
    private final String oldestCursor;
    private final boolean isSpam;
    private final DirectItem lastPermanentItem;
    private final DirectThreadDirectStory directStory;
    private boolean approvalRequiredForNewMembers;
    private int inputMode;
    private final List<ThreadContext> threadContextItems;
    private boolean isTemp;

    public DirectThread(final String threadId,
                        final String threadV2Id,
                        final List<User> users,
                        final List<User> leftUsers,
                        final List<Long> adminUserIds,
                        final List<DirectItem> items,
                        final long lastActivityAt,
                        final boolean muted,
                        final boolean isPin,
                        final boolean named,
                        final boolean canonical,
                        final boolean pending,
                        final boolean archived,
                        final boolean valuedRequest,
                        final String threadType,
                        final long viewerId,
                        final String threadTitle,
                        final String pendingScore,
                        final long folder,
                        final boolean vcMuted,
                        final boolean isGroup,
                        final boolean mentionsMuted,
                        final User inviter,
                        final boolean hasOlder,
                        final boolean hasNewer,
                        final Map<Long, DirectThreadLastSeenAt> lastSeenAt,
                        final String newestCursor,
                        final String oldestCursor,
                        final boolean isSpam,
                        final DirectItem lastPermanentItem,
                        final DirectThreadDirectStory directStory,
                        final boolean approvalRequiredForNewMembers,
                        final int inputMode,
                        final List<ThreadContext> threadContextItems) {
        this.threadId = threadId;
        this.threadV2Id = threadV2Id;
        this.users = users;
        this.leftUsers = leftUsers;
        this.adminUserIds = adminUserIds;
        this.items = items;
        this.lastActivityAt = lastActivityAt;
        this.muted = muted;
        this.isPin = isPin;
        this.named = named;
        this.canonical = canonical;
        this.pending = pending;
        this.archived = archived;
        this.valuedRequest = valuedRequest;
        this.threadType = threadType;
        this.viewerId = viewerId;
        this.threadTitle = threadTitle;
        this.pendingScore = pendingScore;
        this.folder = folder;
        this.vcMuted = vcMuted;
        this.isGroup = isGroup;
        this.mentionsMuted = mentionsMuted;
        this.inviter = inviter;
        this.hasOlder = hasOlder;
        this.hasNewer = hasNewer;
        this.lastSeenAt = lastSeenAt;
        this.newestCursor = newestCursor;
        this.oldestCursor = oldestCursor;
        this.isSpam = isSpam;
        this.lastPermanentItem = lastPermanentItem;
        this.directStory = directStory;
        this.approvalRequiredForNewMembers = approvalRequiredForNewMembers;
        this.inputMode = inputMode;
        this.threadContextItems = threadContextItems;
    }

    public String getThreadId() {
        return threadId;
    }

    public String getThreadV2Id() {
        return threadV2Id;
    }

    public List<User> getUsers() {
        return users;
    }

    public void setUsers(final List<User> users) {
        this.users = users;
    }

    public List<User> getLeftUsers() {
        return leftUsers;
    }

    public void setLeftUsers(final List<User> leftUsers) {
        this.leftUsers = leftUsers;
    }

    public List<Long> getAdminUserIds() {
        return adminUserIds;
    }

    public void setAdminUserIds(final List<Long> adminUserIds) {
        this.adminUserIds = adminUserIds;
    }

    public List<DirectItem> getItems() {
        return items;
    }

    public void setItems(final List<DirectItem> items) {
        this.items = items;
    }

    public long getLastActivityAt() {
        return lastActivityAt;
    }

    public boolean isMuted() {
        return muted;
    }

    public void setMuted(final boolean muted) {
        this.muted = muted;
    }

    public boolean isPin() {
        return isPin;
    }

    public boolean isNamed() {
        return named;
    }

    public boolean isCanonical() {
        return canonical;
    }

    public boolean isPending() {
        return pending;
    }

    public void setPending(final boolean pending) {
        this.pending = pending;
    }

    public boolean isArchived() {
        return archived;
    }

    public boolean isValuedRequest() {
        return valuedRequest;
    }

    public String getThreadType() {
        return threadType;
    }

    public long getViewerId() {
        return viewerId;
    }

    public String getThreadTitle() {
        return threadTitle;
    }

    public String getPendingScore() {
        return pendingScore;
    }

    public long getFolder() {
        return folder;
    }

    public boolean isVcMuted() {
        return vcMuted;
    }

    public boolean isGroup() {
        return isGroup;
    }

    public boolean isMentionsMuted() {
        return mentionsMuted;
    }

    public void setMentionsMuted(final boolean mentionsMuted) {
        this.mentionsMuted = mentionsMuted;
    }

    public User getInviter() {
        return inviter;
    }

    public boolean hasOlder() {
        return hasOlder;
    }

    public boolean hasNewer() {
        return hasNewer;
    }

    public Map<Long, DirectThreadLastSeenAt> getLastSeenAt() {
        return lastSeenAt;
    }

    public void setLastSeenAt(final Map<Long, DirectThreadLastSeenAt> lastSeenAt) {
        this.lastSeenAt = lastSeenAt;
    }

    public String getNewestCursor() {
        return newestCursor;
    }

    public String getOldestCursor() {
        return oldestCursor;
    }

    public boolean isSpam() {
        return isSpam;
    }

    public DirectItem getLastPermanentItem() {
        return lastPermanentItem;
    }

    public DirectThreadDirectStory getDirectStory() {
        return directStory;
    }

    public boolean isApprovalRequiredForNewMembers() {
        return approvalRequiredForNewMembers;
    }

    public void setApprovalRequiredForNewMembers(final boolean approvalRequiredForNewMembers) {
        this.approvalRequiredForNewMembers = approvalRequiredForNewMembers;
    }

    public int getInputMode() {
        return inputMode;
    }

    public void setInputMode(final int inputMode) {
        this.inputMode = inputMode;
    }

    public List<ThreadContext> getThreadContextItems() {
        return threadContextItems;
    }

    public boolean isTemp() {
        return isTemp;
    }

    public void setTemp(final boolean isTemp) {
        this.isTemp = isTemp;
    }

    @Nullable
    public DirectItem getFirstDirectItem() {
        DirectItem firstItem = null;
        if (!items.isEmpty()) {
            int position = 0;
            while (firstItem == null && position < items.size()) {
                firstItem = items.get(position);
                position++;
            }
        }
        return firstItem;
    }

    @NonNull
    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final DirectThread that = (DirectThread) o;
        return lastActivityAt == that.lastActivityAt &&
                muted == that.muted &&
                isPin == that.isPin &&
                named == that.named &&
                canonical == that.canonical &&
                pending == that.pending &&
                archived == that.archived &&
                valuedRequest == that.valuedRequest &&
                viewerId == that.viewerId &&
                folder == that.folder &&
                vcMuted == that.vcMuted &&
                isGroup == that.isGroup &&
                mentionsMuted == that.mentionsMuted &&
                hasOlder == that.hasOlder &&
                hasNewer == that.hasNewer &&
                isSpam == that.isSpam &&
                approvalRequiredForNewMembers == that.approvalRequiredForNewMembers &&
                inputMode == that.inputMode &&
                Objects.equals(threadId, that.threadId) &&
                Objects.equals(threadV2Id, that.threadV2Id) &&
                Objects.equals(users, that.users) &&
                Objects.equals(leftUsers, that.leftUsers) &&
                Objects.equals(adminUserIds, that.adminUserIds) &&
                Objects.equals(items, that.items) &&
                Objects.equals(threadType, that.threadType) &&
                Objects.equals(threadTitle, that.threadTitle) &&
                Objects.equals(pendingScore, that.pendingScore) &&
                Objects.equals(inviter, that.inviter) &&
                Objects.equals(lastSeenAt, that.lastSeenAt) &&
                Objects.equals(newestCursor, that.newestCursor) &&
                Objects.equals(oldestCursor, that.oldestCursor) &&
                Objects.equals(lastPermanentItem, that.lastPermanentItem) &&
                Objects.equals(directStory, that.directStory) &&
                Objects.equals(threadContextItems, that.threadContextItems);
    }

    @Override
    public int hashCode() {
        return Objects
                .hash(threadId, threadV2Id, users, leftUsers, adminUserIds, items, lastActivityAt, muted, isPin, named, canonical, pending, archived,
                      valuedRequest, threadType, viewerId, threadTitle, pendingScore, folder, vcMuted, isGroup, mentionsMuted, inviter, hasOlder,
                      hasNewer, lastSeenAt, newestCursor, oldestCursor, isSpam, lastPermanentItem, directStory, approvalRequiredForNewMembers,
                      inputMode, threadContextItems);
    }
}
