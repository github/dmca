package awais.instagrabber.adapters.viewholder;

import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.databinding.ItemSearchResultBinding;
import awais.instagrabber.fragments.search.SearchCategoryFragment.OnSearchItemClickListener;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.repositories.responses.Hashtag;
import awais.instagrabber.repositories.responses.Place;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.search.SearchItem;

public class SearchItemViewHolder extends RecyclerView.ViewHolder {

    private final ItemSearchResultBinding binding;
    private final OnSearchItemClickListener onSearchItemClickListener;

    public SearchItemViewHolder(@NonNull final ItemSearchResultBinding binding,
                                final OnSearchItemClickListener onSearchItemClickListener) {
        super(binding.getRoot());
        this.binding = binding;
        this.onSearchItemClickListener = onSearchItemClickListener;
    }

    public void bind(final SearchItem searchItem) {
        if (searchItem == null) return;
        final FavoriteType type = searchItem.getType();
        if (type == null) return;
        String title;
        String subtitle;
        String picUrl;
        boolean isVerified = false;
        switch (type) {
            case USER:
                final User user = searchItem.getUser();
                title = "@" + user.getUsername();
                subtitle = user.getFullName();
                picUrl = user.getProfilePicUrl();
                isVerified = user.isVerified();
                break;
            case HASHTAG:
                final Hashtag hashtag = searchItem.getHashtag();
                title = "#" + hashtag.getName();
                subtitle = hashtag.getSubtitle();
                picUrl = "res:/" + R.drawable.ic_hashtag;
                break;
            case LOCATION:
                final Place place = searchItem.getPlace();
                title = place.getTitle();
                subtitle = place.getSubtitle();
                picUrl = "res:/" + R.drawable.ic_location;
                break;
            default:
                return;
        }
        itemView.setOnClickListener(v -> {
            if (onSearchItemClickListener != null) {
                onSearchItemClickListener.onSearchItemClick(searchItem);
            }
        });
        binding.delete.setVisibility(searchItem.isRecent() ? View.VISIBLE : View.GONE);
        if (searchItem.isRecent()) {
            binding.delete.setEnabled(true);
            binding.delete.setOnClickListener(v -> {
                if (onSearchItemClickListener != null) {
                    binding.delete.setEnabled(false);
                    onSearchItemClickListener.onSearchItemDelete(searchItem);
                }
            });
        }
        binding.title.setText(title);
        binding.subtitle.setText(subtitle);
        binding.profilePic.setImageURI(picUrl);
        binding.verified.setVisibility(isVerified ? View.VISIBLE : View.GONE);
    }
}
