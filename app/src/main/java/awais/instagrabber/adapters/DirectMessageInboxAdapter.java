package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.AsyncDifferConfig;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import java.util.List;
import java.util.Objects;

import awais.instagrabber.adapters.viewholder.directmessages.DirectInboxItemViewHolder;
import awais.instagrabber.databinding.LayoutDmInboxItemBinding;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;

public final class DirectMessageInboxAdapter extends ListAdapter<DirectThread, DirectInboxItemViewHolder> {
    private final OnItemClickListener onClickListener;

    private static final DiffUtil.ItemCallback<DirectThread> diffCallback = new DiffUtil.ItemCallback<DirectThread>() {
        @Override
        public boolean areItemsTheSame(@NonNull final DirectThread oldItem, @NonNull final DirectThread newItem) {
            return oldItem.getThreadId().equals(newItem.getThreadId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final DirectThread oldThread,
                                          @NonNull final DirectThread newThread) {
            final boolean titleEqual = oldThread.getThreadTitle().equals(newThread.getThreadTitle());
            if (!titleEqual) return false;
            final boolean lastSeenAtEqual = Objects.equals(oldThread.getLastSeenAt(), newThread.getLastSeenAt());
            if (!lastSeenAtEqual) return false;
            final List<DirectItem> oldItems = oldThread.getItems();
            final List<DirectItem> newItems = newThread.getItems();
            if (oldItems == null || newItems == null) return false;
            if (oldItems.size() != newItems.size()) return false;
            final DirectItem oldItemFirst = oldThread.getFirstDirectItem();
            final DirectItem newItemFirst = newThread.getFirstDirectItem();
            if (oldItemFirst == null || newItemFirst == null) return false;
            final boolean idsEqual = oldItemFirst.getItemId().equals(newItemFirst.getItemId());
            if (!idsEqual) return false;
            return oldItemFirst.getTimestamp() == newItemFirst.getTimestamp();
        }
    };

    public DirectMessageInboxAdapter(final OnItemClickListener onClickListener) {
        super(new AsyncDifferConfig.Builder<>(diffCallback).build());
        this.onClickListener = onClickListener;
    }

    @NonNull
    @Override
    public DirectInboxItemViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int type) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final LayoutDmInboxItemBinding binding = LayoutDmInboxItemBinding.inflate(layoutInflater, parent, false);
        return new DirectInboxItemViewHolder(binding, onClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final DirectInboxItemViewHolder holder, final int position) {
        final DirectThread thread = getItem(position);
        holder.bind(thread);
    }

    @Override
    public long getItemId(final int position) {
        return getItem(position).getThreadId().hashCode();
    }

    public interface OnItemClickListener {
        void onItemClick(final DirectThread thread);
    }
}