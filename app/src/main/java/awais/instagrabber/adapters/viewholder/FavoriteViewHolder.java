package awais.instagrabber.adapters.viewholder;

import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.adapters.FavoritesAdapter;
import awais.instagrabber.databinding.ItemSearchResultBinding;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.utils.Constants;

public class FavoriteViewHolder extends RecyclerView.ViewHolder {
    private static final String TAG = "FavoriteViewHolder";

    private final ItemSearchResultBinding binding;

    public FavoriteViewHolder(@NonNull final ItemSearchResultBinding binding) {
        super(binding.getRoot());
        this.binding = binding;
        binding.verified.setVisibility(View.GONE);
    }

    public void bind(final Favorite model,
                     final FavoritesAdapter.OnFavoriteClickListener clickListener,
                     final FavoritesAdapter.OnFavoriteLongClickListener longClickListener) {
        // Log.d(TAG, "bind: " + model);
        if (model == null) return;
        itemView.setOnClickListener(v -> {
            if (clickListener == null) return;
            clickListener.onClick(model);
        });
        itemView.setOnLongClickListener(v -> {
            if (clickListener == null) return false;
            return longClickListener.onLongClick(model);
        });
        if (model.getType() == FavoriteType.HASHTAG) {
            binding.profilePic.setImageURI(Constants.DEFAULT_HASH_TAG_PIC);
        } else {
            binding.profilePic.setImageURI(model.getPicUrl());
        }
        binding.title.setVisibility(View.VISIBLE);
        binding.subtitle.setText(model.getDisplayName());
        String query = model.getQuery();
        switch (model.getType()) {
            case HASHTAG:
                query = "#" + query;
                break;
            case USER:
                query = "@" + query;
                break;
            case LOCATION:
                binding.title.setVisibility(View.GONE);
                break;
            default:
                // do nothing
        }
        binding.title.setText(query);
    }
}
