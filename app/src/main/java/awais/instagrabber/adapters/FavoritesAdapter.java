package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.util.ObjectsCompat;
import androidx.recyclerview.widget.AdapterListUpdateCallback;
import androidx.recyclerview.widget.AsyncDifferConfig;
import androidx.recyclerview.widget.AsyncListDiffer;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.viewholder.FavoriteViewHolder;
import awais.instagrabber.databinding.ItemFavSectionHeaderBinding;
import awais.instagrabber.databinding.ItemSearchResultBinding;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.models.enums.FavoriteType;

public class FavoritesAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    private final OnFavoriteClickListener clickListener;
    private final OnFavoriteLongClickListener longClickListener;
    private final AsyncListDiffer<FavoriteModelOrHeader> differ;

    private static final DiffUtil.ItemCallback<FavoriteModelOrHeader> diffCallback = new DiffUtil.ItemCallback<FavoriteModelOrHeader>() {
        @Override
        public boolean areItemsTheSame(@NonNull final FavoriteModelOrHeader oldItem, @NonNull final FavoriteModelOrHeader newItem) {
            boolean areSame = oldItem.isHeader() && newItem.isHeader();
            if (!areSame) {
                return false;
            }
            if (oldItem.isHeader()) {
                return ObjectsCompat.equals(oldItem.header, newItem.header);
            }
            if (oldItem.model != null && newItem.model != null) {
                return oldItem.model.getId() == newItem.model.getId();
            }
            return false;
        }

        @Override
        public boolean areContentsTheSame(@NonNull final FavoriteModelOrHeader oldItem, @NonNull final FavoriteModelOrHeader newItem) {
            boolean areSame = oldItem.isHeader() && newItem.isHeader();
            if (!areSame) {
                return false;
            }
            if (oldItem.isHeader()) {
                return ObjectsCompat.equals(oldItem.header, newItem.header);
            }
            return ObjectsCompat.equals(oldItem.model, newItem.model);
        }
    };

    public FavoritesAdapter(final OnFavoriteClickListener clickListener, final OnFavoriteLongClickListener longClickListener) {
        this.clickListener = clickListener;
        this.longClickListener = longClickListener;
        differ = new AsyncListDiffer<>(new AdapterListUpdateCallback(this),
                                       new AsyncDifferConfig.Builder<>(diffCallback).build());
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        if (viewType == 0) {
            // header
            return new FavSectionViewHolder(ItemFavSectionHeaderBinding.inflate(inflater, parent, false));
        }
        final ItemSearchResultBinding binding = ItemSearchResultBinding.inflate(inflater, parent, false);
        return new FavoriteViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull final RecyclerView.ViewHolder holder, final int position) {
        if (getItemViewType(position) == 0) {
            final FavoriteModelOrHeader modelOrHeader = getItem(position);
            if (!modelOrHeader.isHeader()) return;
            ((FavSectionViewHolder) holder).bind(modelOrHeader.header);
            return;
        }
        ((FavoriteViewHolder) holder).bind(getItem(position).model, clickListener, longClickListener);
    }

    protected FavoriteModelOrHeader getItem(int position) {
        return differ.getCurrentList().get(position);
    }

    @Override
    public int getItemCount() {
        return differ.getCurrentList().size();
    }

    @Override
    public int getItemViewType(final int position) {
        return getItem(position).isHeader() ? 0 : 1;
    }

    public void submitList(@Nullable final List<Favorite> list) {
        if (list == null) {
            differ.submitList(null);
            return;
        }
        differ.submitList(sectionAndSort(list));
    }

    public void submitList(@Nullable final List<Favorite> list, @Nullable final Runnable commitCallback) {
        if (list == null) {
            differ.submitList(null, commitCallback);
            return;
        }
        differ.submitList(sectionAndSort(list), commitCallback);
    }

    @NonNull
    private List<FavoriteModelOrHeader> sectionAndSort(@NonNull final List<Favorite> list) {
        final List<Favorite> listCopy = new ArrayList<>(list);
        Collections.sort(listCopy, (o1, o2) -> {
            if (o1.getType() == o2.getType()) return 0;
            // keep users at top
            if (o1.getType() == FavoriteType.USER) return -1;
            if (o2.getType() == FavoriteType.USER) return 1;
            // keep locations at bottom
            if (o1.getType() == FavoriteType.LOCATION) return 1;
            if (o2.getType() == FavoriteType.LOCATION) return -1;
            return 0;
        });
        final List<FavoriteModelOrHeader> modelOrHeaders = new ArrayList<>();
        for (int i = 0; i < listCopy.size(); i++) {
            final Favorite model = listCopy.get(i);
            final FavoriteModelOrHeader prev = modelOrHeaders.isEmpty() ? null : modelOrHeaders.get(modelOrHeaders.size() - 1);
            boolean prevWasSameType = prev != null && prev.model.getType() == model.getType();
            if (prevWasSameType) {
                // just add model
                final FavoriteModelOrHeader modelOrHeader = new FavoriteModelOrHeader();
                modelOrHeader.model = model;
                modelOrHeaders.add(modelOrHeader);
                continue;
            }
            // add header and model
            FavoriteModelOrHeader modelOrHeader = new FavoriteModelOrHeader();
            modelOrHeader.header = model.getType();
            modelOrHeaders.add(modelOrHeader);
            modelOrHeader = new FavoriteModelOrHeader();
            modelOrHeader.model = model;
            modelOrHeaders.add(modelOrHeader);
        }
        return modelOrHeaders;
    }

    private static class FavoriteModelOrHeader {
        FavoriteType header;
        Favorite model;

        boolean isHeader() {
            return header != null;
        }
    }

    public interface OnFavoriteClickListener {
        void onClick(final Favorite model);
    }

    public interface OnFavoriteLongClickListener {
        boolean onLongClick(final Favorite model);
    }

    public static class FavSectionViewHolder extends RecyclerView.ViewHolder {
        private final ItemFavSectionHeaderBinding binding;

        public FavSectionViewHolder(@NonNull final ItemFavSectionHeaderBinding binding) {
            super(binding.getRoot());
            this.binding = binding;
        }

        public void bind(final FavoriteType header) {
            if (header == null) return;
            final int headerText;
            switch (header) {
                case USER:
                    headerText = R.string.accounts;
                    break;
                case HASHTAG:
                    headerText = R.string.hashtags;
                    break;
                case LOCATION:
                    headerText = R.string.locations;
                    break;
                default:
                    headerText = R.string.unknown;
                    break;
            }
            binding.getRoot().setText(headerText);
        }
    }
}
