package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.databinding.ItemStoryBinding;
import awais.instagrabber.models.StoryModel;

public final class StoriesAdapter extends ListAdapter<StoryModel, StoriesAdapter.StoryViewHolder> {
    private final OnItemClickListener onItemClickListener;

    private static final DiffUtil.ItemCallback<StoryModel> diffCallback = new DiffUtil.ItemCallback<StoryModel>() {
        @Override
        public boolean areItemsTheSame(@NonNull final StoryModel oldItem, @NonNull final StoryModel newItem) {
            return oldItem.getStoryMediaId().equals(newItem.getStoryMediaId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final StoryModel oldItem, @NonNull final StoryModel newItem) {
            return oldItem.getStoryMediaId().equals(newItem.getStoryMediaId());
        }
    };

    public StoriesAdapter(final OnItemClickListener onItemClickListener) {
        super(diffCallback);
        this.onItemClickListener = onItemClickListener;
    }

    @NonNull
    @Override
    public StoryViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemStoryBinding binding = ItemStoryBinding.inflate(layoutInflater, parent, false);
        return new StoryViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull final StoryViewHolder holder, final int position) {
        final StoryModel storyModel = getItem(position);
        holder.bind(storyModel, position, onItemClickListener);
    }

    public final static class StoryViewHolder extends RecyclerView.ViewHolder {
        private final ItemStoryBinding binding;

        public StoryViewHolder(final ItemStoryBinding binding) {
            super(binding.getRoot());
            this.binding = binding;
        }

        public void bind(final StoryModel model,
                         final int position,
                         final OnItemClickListener clickListener) {
            if (model == null) return;
            model.setPosition(position);

            itemView.setTag(model);
            itemView.setOnClickListener(v -> {
                if (clickListener == null) return;
                clickListener.onItemClick(model, position);
            });

            binding.selectedView.setVisibility(model.isCurrentSlide() ? View.VISIBLE : View.GONE);
            binding.icon.setImageURI(model.getStoryUrl());
            // Glide.with(itemView).load(model.getStoryUrl())
            //      .apply(new RequestOptions().override(width, height))
            //      .into(holder.icon);
        }
    }

    public interface OnItemClickListener {
        void onItemClick(StoryModel storyModel, int position);
    }
}