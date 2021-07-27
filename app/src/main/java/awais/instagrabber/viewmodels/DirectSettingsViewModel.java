package awais.instagrabber.viewmodels;

import android.app.Application;
import android.content.ContentResolver;
import android.content.res.Resources;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;
import androidx.core.util.Pair;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import awais.instagrabber.R;
import awais.instagrabber.dialogs.MultiOptionDialogFragment.Option;
import awais.instagrabber.managers.DirectMessagesManager;
import awais.instagrabber.managers.ThreadManager;
import awais.instagrabber.models.Resource;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadParticipantRequestsResponse;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class DirectSettingsViewModel extends AndroidViewModel {
    private static final String TAG = DirectSettingsViewModel.class.getSimpleName();
    private static final String ACTION_KICK = "kick";
    private static final String ACTION_MAKE_ADMIN = "make_admin";
    private static final String ACTION_REMOVE_ADMIN = "remove_admin";
    private static final String ACTION_BLOCK = "block";
    private static final String ACTION_UNBLOCK = "unblock";
    // private static final String ACTION_REPORT = "report";
    private static final String ACTION_RESTRICT = "restrict";
    private static final String ACTION_UNRESTRICT = "unrestrict";

    private final long viewerId;
    private final Resources resources;
    private final ThreadManager threadManager;

    public DirectSettingsViewModel(final Application application,
                                   @NonNull final String threadId,
                                   final boolean pending,
                                   @NonNull final User currentUser) {
        super(application);
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        viewerId = CookieUtils.getUserIdFromCookie(cookie);
        final String deviceUuid = settingsHelper.getString(Constants.DEVICE_UUID);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        if (TextUtils.isEmpty(csrfToken) || viewerId <= 0 || TextUtils.isEmpty(deviceUuid)) {
            throw new IllegalArgumentException("User is not logged in!");
        }
        final ContentResolver contentResolver = application.getContentResolver();
        resources = getApplication().getResources();
        final DirectMessagesManager messagesManager = DirectMessagesManager.getInstance();
        threadManager = messagesManager.getThreadManager(threadId, pending, currentUser, contentResolver);
    }

    @NonNull
    public LiveData<DirectThread> getThread() {
        return threadManager.getThread();
    }

    // public void setThread(@NonNull final DirectThread thread) {
    //     this.thread = thread;
    //     inputMode.postValue(thread.getInputMode());
    //     List<User> users = thread.getUsers();
    //     final ImmutableList.Builder<User> builder = ImmutableList.<User>builder().add(currentUser);
    //     if (users != null) {
    //         builder.addAll(users);
    //     }
    //     users = builder.build();
    //     this.users.postValue(new Pair<>(users, thread.getLeftUsers()));
    //     // setTitle(thread.getThreadTitle());
    //     final List<Long> adminUserIds = thread.getAdminUserIds();
    //     this.adminUserIds.postValue(adminUserIds);
    //     viewerIsAdmin = adminUserIds.contains(viewerId);
    //     muted.postValue(thread.isMuted());
    //     mentionsMuted.postValue(thread.isMentionsMuted());
    //     approvalRequiredToJoin.postValue(thread.isApprovalRequiredForNewMembers());
    //     isPending.postValue(thread.isPending());
    //     if (thread.getInputMode() != 1 && thread.isGroup() && viewerIsAdmin) {
    //         fetchPendingRequests();
    //     }
    // }

    public LiveData<Integer> getInputMode() {
        return threadManager.getInputMode();
    }

    public LiveData<Boolean> isGroup() {
        return threadManager.isGroup();
    }

    public LiveData<List<User>> getUsers() {
        return threadManager.getUsersWithCurrent();
    }

    public LiveData<List<User>> getLeftUsers() {
        return threadManager.getLeftUsers();
    }

    public LiveData<Pair<List<User>, List<User>>> getUsersAndLeftUsers() {
        return threadManager.getUsersAndLeftUsers();
    }

    public LiveData<String> getTitle() {
        return threadManager.getThreadTitle();
    }

    // public void setTitle(final String title) {
    //     if (title == null) {
    //         this.title.postValue("");
    //         return;
    //     }
    //     this.title.postValue(title.trim());
    // }

    public LiveData<List<Long>> getAdminUserIds() {
        return threadManager.getAdminUserIds();
    }

    public LiveData<Boolean> isMuted() {
        return threadManager.isMuted();
    }

    public LiveData<Boolean> getApprovalRequiredToJoin() {
        return threadManager.isApprovalRequiredToJoin();
    }

    public LiveData<DirectThreadParticipantRequestsResponse> getPendingRequests() {
        return threadManager.getPendingRequests();
    }

    public LiveData<Boolean> isPending() {
        return threadManager.isPending();
    }

    public LiveData<Boolean> isViewerAdmin() {
        return threadManager.isViewerAdmin();
    }

    public LiveData<Resource<Object>> updateTitle(final String newTitle) {
        return threadManager.updateTitle(newTitle);
    }

    public LiveData<Resource<Object>> addMembers(final Set<User> users) {
        return threadManager.addMembers(users);
    }

    public LiveData<Resource<Object>> removeMember(final User user) {
        return threadManager.removeMember(user);
    }

    private LiveData<Resource<Object>> makeAdmin(final User user) {
        return threadManager.makeAdmin(user);
    }

    private LiveData<Resource<Object>> removeAdmin(final User user) {
        return threadManager.removeAdmin(user);
    }

    public LiveData<Resource<Object>> mute() {
        return threadManager.mute();
    }

    public LiveData<Resource<Object>> unmute() {
        return threadManager.unmute();
    }

    public LiveData<Resource<Object>> muteMentions() {
        return threadManager.muteMentions();
    }

    public LiveData<Resource<Object>> unmuteMentions() {
        return threadManager.unmuteMentions();
    }

    private LiveData<Resource<Object>> blockUser(final User user) {
        return threadManager.blockUser(user);
    }

    private LiveData<Resource<Object>> unblockUser(final User user) {
        return threadManager.unblockUser(user);
    }

    private LiveData<Resource<Object>> restrictUser(final User user) {
        return threadManager.restrictUser(user);
    }

    private LiveData<Resource<Object>> unRestrictUser(final User user) {
        return threadManager.unRestrictUser(user);
    }

    public LiveData<Resource<Object>> approveUsers(final List<User> users) {
        return threadManager.approveUsers(users);
    }

    public LiveData<Resource<Object>> denyUsers(final List<User> users) {
        return threadManager.denyUsers(users);
    }

    public LiveData<Resource<Object>> approvalRequired() {
        return threadManager.approvalRequired();
    }

    public LiveData<Resource<Object>> approvalNotRequired() {
        return threadManager.approvalNotRequired();
    }

    public LiveData<Resource<Object>> leave() {
        return threadManager.leave();
    }

    public LiveData<Resource<Object>> end() {
        return threadManager.end();
    }

    public ArrayList<Option<String>> createUserOptions(final User user) {
        final ArrayList<Option<String>> options = new ArrayList<>();
        if (user == null || isSelf(user) || hasLeft(user)) {
            return options;
        }
        final Boolean viewerIsAdmin = threadManager.isViewerAdmin().getValue();
        if (viewerIsAdmin != null && viewerIsAdmin) {
            options.add(new Option<>(getString(R.string.dms_action_kick), ACTION_KICK));

            final boolean isAdmin = threadManager.isAdmin(user);
            options.add(new Option<>(
                    isAdmin ? getString(R.string.dms_action_remove_admin) : getString(R.string.dms_action_make_admin),
                    isAdmin ? ACTION_REMOVE_ADMIN : ACTION_MAKE_ADMIN
            ));
        }

        final boolean blocking = user.getFriendshipStatus().isBlocking();
        options.add(new Option<>(
                blocking ? getString(R.string.unblock) : getString(R.string.block),
                blocking ? ACTION_UNBLOCK : ACTION_BLOCK
        ));

        // options.add(new Option<>(getString(R.string.report), ACTION_REPORT));
        final Boolean isGroup = threadManager.isGroup().getValue();
        if (isGroup != null && isGroup) {
            final boolean restricted = user.getFriendshipStatus().isRestricted();
            options.add(new Option<>(
                    restricted ? getString(R.string.unrestrict) : getString(R.string.restrict),
                    restricted ? ACTION_UNRESTRICT : ACTION_RESTRICT
            ));
        }
        return options;
    }

    private boolean hasLeft(final User user) {
        final List<User> leftUsers = getLeftUsers().getValue();
        if (leftUsers == null) return false;
        return leftUsers.contains(user);
    }

    private boolean isSelf(final User user) {
        return user.getPk() == viewerId;
    }

    private String getString(@StringRes final int resId) {
        return resources.getString(resId);
    }

    public LiveData<Resource<Object>> doAction(final User user, final String action) {
        if (user == null || action == null) return null;
        switch (action) {
            case ACTION_KICK:
                return removeMember(user);
            case ACTION_MAKE_ADMIN:
                return makeAdmin(user);
            case ACTION_REMOVE_ADMIN:
                return removeAdmin(user);
            case ACTION_BLOCK:
                return blockUser(user);
            case ACTION_UNBLOCK:
                return unblockUser(user);
            // case ACTION_REPORT:
            //     break;
            case ACTION_RESTRICT:
                return restrictUser(user);
            case ACTION_UNRESTRICT:
                return unRestrictUser(user);
            default:
                return null;
        }
    }

    public LiveData<User> getInviter() {
        return threadManager.getInviter();
    }
}
