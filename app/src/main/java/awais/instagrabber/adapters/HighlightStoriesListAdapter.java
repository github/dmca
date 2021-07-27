package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import awais.instagrabber.adapters.viewholder.StoryListViewHolder;
import awais.instagrabber.databinding.ItemNotificationBinding;
import awais.instagrabber.models.HighlightModel;

public final class HighlightStoriesListAdapter extends ListAdapter<HighlightModel, StoryListViewHolder> {
    private final OnHighlightStoryClickListener listener;

    private static final DiffUtil.ItemCallback<HighlightModel> diffCallback = new DiffUtil.ItemCallback<HighlightModel>() {
        @Override
        public boolean areItemsTheSame(@NonNull final HighlightModel oldItem, @NonNull final HighlightModel newItem) {
            return oldItem.getId().equals(newItem.getId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final HighlightModel oldItem, @NonNull final HighlightModel newItem) {
            return oldItem.getId().equals(newItem.getId());
        }
    };

    public HighlightStoriesListAdapter(final OnHighlightStoryClickListener listener) {
        super(diffCallback);
        this.listener = listener;
    }

    @NonNull
    @Override
    public StoryListViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemNotificationBinding binding = ItemNotificationBinding.inflate(layoutInflater, parent, false);
        return new StoryListViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull final StoryListViewHolder holder, final int position) {
        final HighlightModel model = getItem(position);
        holder.bind(model, position, listener);
    }

    public interface OnHighlightStoryClickListener {
        void onHighlightClick(final HighlightModel model, final int position);

        void onProfileClick(final String username);
    }
}
