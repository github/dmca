package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import awais.instagrabber.adapters.DirectUsersAdapter.OnDirectUserClickListener;
import awais.instagrabber.adapters.viewholder.directmessages.DirectUserViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.RecipientThreadViewHolder;
import awais.instagrabber.databinding.LayoutDmUserItemBinding;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipient;

public final class UserSearchResultsAdapter extends ListAdapter<RankedRecipient, RecyclerView.ViewHolder> {
    private static final int VIEW_TYPE_USER = 0;
    private static final int VIEW_TYPE_THREAD = 1;
    private static final DiffUtil.ItemCallback<RankedRecipient> DIFF_CALLBACK = new DiffUtil.ItemCallback<RankedRecipient>() {
        @Override
        public boolean areItemsTheSame(@NonNull final RankedRecipient oldItem, @NonNull final RankedRecipient newItem) {
            final boolean bothUsers = oldItem.getUser() != null && newItem.getUser() != null;
            if (!bothUsers) return false;
            final boolean bothThreads = oldItem.getThread() != null && newItem.getThread() != null;
            if (!bothThreads) return false;
            if (bothUsers) {
                return oldItem.getUser().getPk() == newItem.getUser().getPk();
            }
            return Objects.equals(oldItem.getThread().getThreadId(), newItem.getThread().getThreadId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final RankedRecipient oldItem, @NonNull final RankedRecipient newItem) {
            final boolean bothUsers = oldItem.getUser() != null && newItem.getUser() != null;
            if (bothUsers) {
                return Objects.equals(oldItem.getUser().getUsername(), newItem.getUser().getUsername()) &&
                        Objects.equals(oldItem.getUser().getFullName(), newItem.getUser().getFullName());
            }
            return Objects.equals(oldItem.getThread().getThreadTitle(), newItem.getThread().getThreadTitle());
        }
    };

    private final boolean showSelection;
    private final Set<RankedRecipient> selectedRecipients;
    private final OnDirectUserClickListener onUserClickListener;
    private final OnRecipientClickListener onRecipientClickListener;

    public UserSearchResultsAdapter(final boolean showSelection,
                                    final OnRecipientClickListener onRecipientClickListener) {
        super(DIFF_CALLBACK);
        this.showSelection = showSelection;
        selectedRecipients = showSelection ? new HashSet<>() : null;
        this.onRecipientClickListener = onRecipientClickListener;
        this.onUserClickListener = (position, user, selected) -> {
            if (onRecipientClickListener != null) {
                onRecipientClickListener.onClick(position, RankedRecipient.of(user), selected);
            }
        };
        setHasStableIds(true);
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final LayoutDmUserItemBinding binding = LayoutDmUserItemBinding.inflate(layoutInflater, parent, false);
        if (viewType == VIEW_TYPE_USER) {
            return new DirectUserViewHolder(binding, onUserClickListener, null);
        }
        return new RecipientThreadViewHolder(binding, onRecipientClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final RecyclerView.ViewHolder holder, final int position) {
        final RankedRecipient recipient = getItem(position);
        final int itemViewType = getItemViewType(position);
        if (itemViewType == VIEW_TYPE_USER) {
            boolean isSelected = false;
            if (selectedRecipients != null) {
                isSelected = selectedRecipients.stream()
                                               .anyMatch(rankedRecipient -> rankedRecipient.getUser() != null
                                                       && rankedRecipient.getUser().getPk() == recipient.getUser().getPk());
            }
            ((DirectUserViewHolder) holder).bind(position, recipient.getUser(), false, false, showSelection, isSelected);
            return;
        }
        boolean isSelected = false;
        if (selectedRecipients != null) {
            isSelected = selectedRecipients.stream()
                                           .anyMatch(rankedRecipient -> rankedRecipient.getThread() != null
                                                   && Objects.equals(rankedRecipient.getThread().getThreadId(), recipient.getThread().getThreadId()));
        }
        ((RecipientThreadViewHolder) holder).bind(position, recipient.getThread(), showSelection, isSelected);
    }

    @Override
    public long getItemId(final int position) {
        final RankedRecipient recipient = getItem(position);
        if (recipient.getUser() != null) {
            return recipient.getUser().getPk();
        }
        if (recipient.getThread() != null) {
            return recipient.getThread().getThreadTitle().hashCode();
        }
        return 0;
    }

    @Override
    public int getItemViewType(final int position) {
        final RankedRecipient recipient = getItem(position);
        return recipient.getUser() != null ? VIEW_TYPE_USER : VIEW_TYPE_THREAD;
    }

    public void setSelectedRecipient(final RankedRecipient recipient, final boolean selected) {
        if (selectedRecipients == null || recipient == null || (recipient.getUser() == null && recipient.getThread() == null)) return;
        final boolean isUser = recipient.getUser() != null;
        int position = -1;
        final List<RankedRecipient> currentList = getCurrentList();
        for (int i = 0; i < currentList.size(); i++) {
            final RankedRecipient temp = currentList.get(i);
            if (isUser) {
                if (temp.getUser() != null && temp.getUser().getPk() == recipient.getUser().getPk()) {
                    position = i;
                    break;
                }
                continue;
            }
            if (temp.getThread() != null && Objects.equals(temp.getThread().getThreadId(), recipient.getThread().getThreadId())) {
                position = i;
                break;
            }
        }
        if (position < 0) return;
        if (selected) {
            selectedRecipients.add(recipient);
        } else {
            selectedRecipients.remove(recipient);
        }
        notifyItemChanged(position);
    }

    public interface OnRecipientClickListener {
        void onClick(int position, RankedRecipient recipient, final boolean isSelected);
    }
}