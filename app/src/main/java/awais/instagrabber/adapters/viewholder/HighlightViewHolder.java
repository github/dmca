package awais.instagrabber.adapters.viewholder;

import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.databinding.ItemHighlightBinding;
import awais.instagrabber.models.HighlightModel;

public final class HighlightViewHolder extends RecyclerView.ViewHolder {

    private final ItemHighlightBinding binding;

    public HighlightViewHolder(final ItemHighlightBinding binding) {
        super(binding.getRoot());
        this.binding = binding;
    }

    public void bind(final HighlightModel model) {
        if (model == null) return;
        binding.title.setText(model.getTitle());
        binding.icon.setImageURI(model.getThumbnailUrl());
        // binding.getRoot().setOnClickListener(v -> {
        //     if (listener == null) return;
        //     listener.onFeedStoryClick(model, position);
        // });
        // final ProfileModel profileModel = model.getProfileModel();
        // binding.title.setText(profileModel.getUsername());
        // binding.title.setAlpha(model.getFullyRead() ? 0.5F : 1.0F);
        // binding.icon.setImageURI(profileModel.getSdProfilePic());
        // binding.icon.setAlpha(model.getFullyRead() ? 0.5F : 1.0F);
    }
}