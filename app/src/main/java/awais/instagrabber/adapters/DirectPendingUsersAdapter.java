package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.adapters.viewholder.directmessages.DirectPendingUserViewHolder;
import awais.instagrabber.databinding.LayoutDmPendingUserItemBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadParticipantRequestsResponse;

public final class DirectPendingUsersAdapter extends ListAdapter<DirectPendingUsersAdapter.PendingUser, DirectPendingUserViewHolder> {

    private static final DiffUtil.ItemCallback<PendingUser> DIFF_CALLBACK = new DiffUtil.ItemCallback<PendingUser>() {
        @Override
        public boolean areItemsTheSame(@NonNull final PendingUser oldItem, @NonNull final PendingUser newItem) {
            return oldItem.user.getPk() == newItem.user.getPk();
        }

        @Override
        public boolean areContentsTheSame(@NonNull final PendingUser oldItem, @NonNull final PendingUser newItem) {
            return Objects.equals(oldItem.user.getUsername(), newItem.user.getUsername()) &&
                    Objects.equals(oldItem.user.getFullName(), newItem.user.getFullName()) &&
                    Objects.equals(oldItem.requester, newItem.requester);
        }
    };

    private final PendingUserCallback callback;

    public DirectPendingUsersAdapter(final PendingUserCallback callback) {
        super(DIFF_CALLBACK);
        this.callback = callback;
        setHasStableIds(true);
    }

    public void submitPendingRequests(final DirectThreadParticipantRequestsResponse requests) {
        if (requests == null || requests.getUsers() == null) {
            submitList(Collections.emptyList());
            return;
        }
        submitList(parse(requests));
    }

    private List<PendingUser> parse(final DirectThreadParticipantRequestsResponse requests) {
        final List<User> users = requests.getUsers();
        final Map<Long, String> requesterUsernames = requests.getRequesterUsernames();
        return users.stream()
                    .map(user -> new PendingUser(user, requesterUsernames.get(user.getPk())))
                    .collect(Collectors.toList());
    }

    @NonNull
    @Override
    public DirectPendingUserViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final LayoutDmPendingUserItemBinding binding = LayoutDmPendingUserItemBinding.inflate(layoutInflater, parent, false);
        return new DirectPendingUserViewHolder(binding, callback);
    }

    @Override
    public void onBindViewHolder(@NonNull final DirectPendingUserViewHolder holder, final int position) {
        final PendingUser pendingUser = getItem(position);
        holder.bind(position, pendingUser);
    }

    @Override
    public long getItemId(final int position) {
        final PendingUser item = getItem(position);
        return item.user.getPk();
    }

    public static class PendingUser {
        private final User user;
        private final String requester;

        private boolean inProgress;

        public PendingUser(final User user, final String requester) {
            this.user = user;
            this.requester = requester;
        }

        public User getUser() {
            return user;
        }

        public String getRequester() {
            return requester;
        }

        public boolean isInProgress() {
            return inProgress;
        }

        public PendingUser setInProgress(final boolean inProgress) {
            this.inProgress = inProgress;
            return this;
        }
    }

    public interface PendingUserCallback {
        void onClick(int position, PendingUser pendingUser);

        void onApprove(int position, PendingUser pendingUser);

        void onDeny(int position, PendingUser pendingUser);
    }
}