package awais.instagrabber.adapters.viewholder;

import android.view.View;

import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.adapters.FeedStoriesListAdapter.OnFeedStoryClickListener;
import awais.instagrabber.adapters.HighlightStoriesListAdapter.OnHighlightStoryClickListener;
import awais.instagrabber.databinding.ItemNotificationBinding;
import awais.instagrabber.models.FeedStoryModel;
import awais.instagrabber.models.HighlightModel;

public final class StoryListViewHolder extends RecyclerView.ViewHolder {
    private final ItemNotificationBinding binding;

    public StoryListViewHolder(final ItemNotificationBinding binding) {
        super(binding.getRoot());
        this.binding = binding;
    }

    public void bind(final FeedStoryModel model,
                     final OnFeedStoryClickListener notificationClickListener) {
        if (model == null) return;

        final int storiesCount = model.getMediaCount();
        binding.tvComment.setVisibility(View.VISIBLE);
        binding.tvComment.setText(itemView.getResources().getQuantityString(R.plurals.stories_count, storiesCount, storiesCount));

        binding.tvSubComment.setVisibility(View.GONE);

        binding.tvDate.setText(model.getDateTime());

        binding.tvUsername.setText(model.getProfileModel().getUsername());
        binding.ivProfilePic.setImageURI(model.getProfileModel().getProfilePicUrl());
        binding.ivProfilePic.setOnClickListener(v -> {
            if (notificationClickListener == null) return;
            notificationClickListener.onProfileClick(model.getProfileModel().getUsername());
        });

        if (model.getFirstStoryModel() != null) {
            binding.ivPreviewPic.setVisibility(View.VISIBLE);
            binding.ivPreviewPic.setImageURI(model.getFirstStoryModel().getThumbnail());
        } else binding.ivPreviewPic.setVisibility(View.INVISIBLE);

        float alpha = model.isFullyRead() ? 0.5F : 1.0F;
        binding.ivProfilePic.setAlpha(alpha);
        binding.ivPreviewPic.setAlpha(alpha);
        binding.tvUsername.setAlpha(alpha);
        binding.tvComment.setAlpha(alpha);
        binding.tvDate.setAlpha(alpha);

        itemView.setOnClickListener(v -> {
            if (notificationClickListener == null) return;
            notificationClickListener.onFeedStoryClick(model);
        });
    }

    public void bind(final HighlightModel model,
                     final int position,
                     final OnHighlightStoryClickListener notificationClickListener) {
        if (model == null) return;

        final int storiesCount = model.getMediaCount();
        binding.tvComment.setVisibility(View.VISIBLE);
        binding.tvComment.setText(itemView.getResources().getQuantityString(R.plurals.stories_count, storiesCount, storiesCount));

        binding.tvSubComment.setVisibility(View.GONE);

        binding.tvUsername.setText(model.getDateTime());

        binding.ivProfilePic.setVisibility(View.GONE);

        binding.ivPreviewPic.setVisibility(View.VISIBLE);
        binding.ivPreviewPic.setImageURI(model.getThumbnailUrl());

        itemView.setOnClickListener(v -> {
            if (notificationClickListener == null) return;
            notificationClickListener.onHighlightClick(model, position);
        });
    }
}