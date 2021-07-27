package awais.instagrabber.adapters.viewholder;

import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.databinding.ItemFollowBinding;
import awais.instagrabber.models.FollowModel;
import awais.instagrabber.repositories.responses.User;

public final class FollowsViewHolder extends RecyclerView.ViewHolder {

    private final ItemFollowBinding binding;

    public FollowsViewHolder(@NonNull final ItemFollowBinding binding) {
        super(binding.getRoot());
        this.binding = binding;
    }

    public void bind(final User model,
                     final View.OnClickListener onClickListener) {
        if (model == null) return;
        itemView.setTag(model);
        itemView.setOnClickListener(onClickListener);
        binding.username.setUsername("@" + model.getUsername(), model.isVerified());
        binding.fullName.setText(model.getFullName());
        binding.profilePic.setImageURI(model.getProfilePicUrl());
    }

    public void bind(final FollowModel model,
                     final View.OnClickListener onClickListener) {
        if (model == null) return;
        itemView.setTag(model);
        itemView.setOnClickListener(onClickListener);
        binding.username.setUsername("@" + model.getUsername());
        binding.fullName.setText(model.getFullName());
        binding.profilePic.setImageURI(model.getProfilePicUrl());
    }
}