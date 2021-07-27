package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import awais.instagrabber.adapters.viewholder.TopicClusterViewHolder;
import awais.instagrabber.databinding.ItemDiscoverTopicBinding;
import awais.instagrabber.repositories.responses.saved.SavedCollection;
import awais.instagrabber.utils.ResponseBodyUtils;

public class SavedCollectionsAdapter extends ListAdapter<SavedCollection, TopicClusterViewHolder> {
    private static final DiffUtil.ItemCallback<SavedCollection> DIFF_CALLBACK = new DiffUtil.ItemCallback<SavedCollection>() {
        @Override
        public boolean areItemsTheSame(@NonNull final SavedCollection oldItem, @NonNull final SavedCollection newItem) {
            return oldItem.getId().equals(newItem.getId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final SavedCollection oldItem, @NonNull final SavedCollection newItem) {
            if (oldItem.getCoverMedias() != null && newItem.getCoverMedias() != null
                && oldItem.getCoverMedias().size() == newItem.getCoverMedias().size()) {
                return oldItem.getCoverMedias().get(0).getId().equals(newItem.getCoverMedias().get(0).getId());
            }
            else if (oldItem.getCoverMedia() != null && newItem.getCoverMedia() != null) {
                return oldItem.getCoverMedia().getId().equals(newItem.getCoverMedia().getId());
            }
            return false;
        }
    };

    private final OnCollectionClickListener onCollectionClickListener;

    public SavedCollectionsAdapter(final OnCollectionClickListener onCollectionClickListener) {
        super(DIFF_CALLBACK);
        this.onCollectionClickListener = onCollectionClickListener;
    }

    @NonNull
    @Override
    public TopicClusterViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemDiscoverTopicBinding binding = ItemDiscoverTopicBinding.inflate(layoutInflater, parent, false);
        return new TopicClusterViewHolder(binding, null, onCollectionClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final TopicClusterViewHolder holder, final int position) {
        final SavedCollection topicCluster = getItem(position);
        holder.bind(topicCluster);
    }

    public interface OnCollectionClickListener {
        void onCollectionClick(SavedCollection savedCollection, View root, View cover, View title, int titleColor, int backgroundColor);
    }
}
