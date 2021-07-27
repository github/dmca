package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import java.util.List;

import awais.instagrabber.adapters.viewholder.directmessages.DirectReactionViewHolder;
import awais.instagrabber.databinding.LayoutDmUserItemBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItemEmojiReaction;

public final class DirectReactionsAdapter extends ListAdapter<DirectItemEmojiReaction, DirectReactionViewHolder> {

    private static final DiffUtil.ItemCallback<DirectItemEmojiReaction> DIFF_CALLBACK = new DiffUtil.ItemCallback<DirectItemEmojiReaction>() {
        @Override
        public boolean areItemsTheSame(@NonNull final DirectItemEmojiReaction oldItem, @NonNull final DirectItemEmojiReaction newItem) {
            return oldItem.getSenderId() == newItem.getSenderId();
        }

        @Override
        public boolean areContentsTheSame(@NonNull final DirectItemEmojiReaction oldItem, @NonNull final DirectItemEmojiReaction newItem) {
            return oldItem.getEmoji().equals(newItem.getEmoji());
        }
    };

    private final long viewerId;
    private final List<User> users;
    private final String itemId;
    private final OnReactionClickListener onReactionClickListener;

    public DirectReactionsAdapter(final long viewerId,
                                  final List<User> users,
                                  final String itemId,
                                  final OnReactionClickListener onReactionClickListener) {
        super(DIFF_CALLBACK);
        this.viewerId = viewerId;
        this.users = users;
        this.itemId = itemId;
        this.onReactionClickListener = onReactionClickListener;
        setHasStableIds(true);
    }

    @NonNull
    @Override
    public DirectReactionViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final LayoutDmUserItemBinding binding = LayoutDmUserItemBinding.inflate(layoutInflater, parent, false);
        return new DirectReactionViewHolder(binding, viewerId, itemId, onReactionClickListener);

    }

    @Override
    public void onBindViewHolder(@NonNull final DirectReactionViewHolder holder, final int position) {
        final DirectItemEmojiReaction reaction = getItem(position);
        if (reaction == null) return;
        holder.bind(reaction, getUser(reaction.getSenderId()));
    }

    @Override
    public long getItemId(final int position) {
        return getItem(position).getSenderId();
    }

    @Nullable
    private User getUser(final long pk) {
        return users.stream()
                    .filter(user -> user.getPk() == pk)
                    .findFirst()
                    .orElse(null);
    }

    public interface OnReactionClickListener {
        void onReactionClick(String itemId, DirectItemEmojiReaction reaction);
    }
}