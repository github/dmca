package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.AdapterListUpdateCallback;
import androidx.recyclerview.widget.AsyncDifferConfig;
import androidx.recyclerview.widget.AsyncListDiffer;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.adapters.viewholder.SearchItemViewHolder;
import awais.instagrabber.databinding.ItemFavSectionHeaderBinding;
import awais.instagrabber.databinding.ItemSearchResultBinding;
import awais.instagrabber.fragments.search.SearchCategoryFragment.OnSearchItemClickListener;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.repositories.responses.search.SearchItem;

public final class SearchItemsAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private static final String TAG = SearchItemsAdapter.class.getSimpleName();
    private static final DiffUtil.ItemCallback<SearchItemOrHeader> DIFF_CALLBACK = new DiffUtil.ItemCallback<SearchItemOrHeader>() {
        @Override
        public boolean areItemsTheSame(@NonNull final SearchItemOrHeader oldItem, @NonNull final SearchItemOrHeader newItem) {
            return Objects.equals(oldItem, newItem);
        }

        @Override
        public boolean areContentsTheSame(@NonNull final SearchItemOrHeader oldItem, @NonNull final SearchItemOrHeader newItem) {
            return Objects.equals(oldItem, newItem);
        }
    };
    private static final String RECENT = "recent";
    private static final String FAVORITE = "favorite";
    private static final int VIEW_TYPE_HEADER = 0;
    private static final int VIEW_TYPE_ITEM = 1;

    private final OnSearchItemClickListener onSearchItemClickListener;
    private final AsyncListDiffer<SearchItemOrHeader> differ;

    public SearchItemsAdapter(final OnSearchItemClickListener onSearchItemClickListener) {
        differ = new AsyncListDiffer<>(new AdapterListUpdateCallback(this),
                                       new AsyncDifferConfig.Builder<>(DIFF_CALLBACK).build());
        this.onSearchItemClickListener = onSearchItemClickListener;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        if (viewType == VIEW_TYPE_HEADER) {
            return new HeaderViewHolder(ItemFavSectionHeaderBinding.inflate(layoutInflater, parent, false));
        }
        final ItemSearchResultBinding binding = ItemSearchResultBinding.inflate(layoutInflater, parent, false);
        return new SearchItemViewHolder(binding, onSearchItemClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final RecyclerView.ViewHolder holder, final int position) {
        if (getItemViewType(position) == VIEW_TYPE_HEADER) {
            final SearchItemOrHeader searchItemOrHeader = getItem(position);
            if (!searchItemOrHeader.isHeader()) return;
            ((HeaderViewHolder) holder).bind(searchItemOrHeader.header);
            return;
        }
        ((SearchItemViewHolder) holder).bind(getItem(position).searchItem);
    }

    protected SearchItemOrHeader getItem(int position) {
        return differ.getCurrentList().get(position);
    }

    @Override
    public int getItemCount() {
        return differ.getCurrentList().size();
    }

    @Override
    public int getItemViewType(final int position) {
        return getItem(position).isHeader() ? VIEW_TYPE_HEADER : VIEW_TYPE_ITEM;
    }

    public void submitList(@Nullable final List<SearchItem> list) {
        if (list == null) {
            differ.submitList(null);
            return;
        }
        differ.submitList(sectionAndSort(list));
    }

    public void submitList(@Nullable final List<SearchItem> list, @Nullable final Runnable commitCallback) {
        if (list == null) {
            differ.submitList(null, commitCallback);
            return;
        }
        differ.submitList(sectionAndSort(list), commitCallback);
    }

    @NonNull
    private List<SearchItemOrHeader> sectionAndSort(@NonNull final List<SearchItem> list) {
        final boolean containsRecentOrFavorite = list.stream().anyMatch(searchItem -> searchItem.isRecent() || searchItem.isFavorite());
        // Don't do anything if not showing recent results
        if (!containsRecentOrFavorite) {
            return list.stream()
                       .map(SearchItemOrHeader::new)
                       .collect(Collectors.toList());
        }
        final List<SearchItem> listCopy = new ArrayList<>(list);
        Collections.sort(listCopy, (o1, o2) -> {
            final boolean bothRecent = o1.isRecent() && o2.isRecent();
            if (bothRecent) {
                // Don't sort
                return 0;
            }
            final boolean bothFavorite = o1.isFavorite() && o2.isFavorite();
            if (bothFavorite) {
                if (o1.getType() == o2.getType()) return 0;
                // keep users at top
                if (o1.getType() == FavoriteType.USER) return -1;
                if (o2.getType() == FavoriteType.USER) return 1;
                // keep locations at bottom
                if (o1.getType() == FavoriteType.LOCATION) return 1;
                if (o2.getType() == FavoriteType.LOCATION) return -1;
            }
            // keep recents at top
            if (o1.isRecent()) return -1;
            if (o2.isRecent()) return 1;
            return 0;
        });
        final List<SearchItemOrHeader> itemOrHeaders = new ArrayList<>();
        for (int i = 0; i < listCopy.size(); i++) {
            final SearchItem searchItem = listCopy.get(i);
            final SearchItemOrHeader prev = itemOrHeaders.isEmpty() ? null : itemOrHeaders.get(itemOrHeaders.size() - 1);
            boolean prevWasSameType = prev != null && ((prev.searchItem.isRecent() && searchItem.isRecent())
                    || (prev.searchItem.isFavorite() && searchItem.isFavorite()));
            if (prevWasSameType) {
                // just add the item
                itemOrHeaders.add(new SearchItemOrHeader(searchItem));
                continue;
            }
            // add header and item
            // add header only if search item is recent or favorite
            if (searchItem.isRecent() || searchItem.isFavorite()) {
                itemOrHeaders.add(new SearchItemOrHeader(searchItem.isRecent() ? RECENT : FAVORITE));
            }
            itemOrHeaders.add(new SearchItemOrHeader(searchItem));
        }
        return itemOrHeaders;
    }

    private static class SearchItemOrHeader {
        String header;
        SearchItem searchItem;

        public SearchItemOrHeader(final SearchItem searchItem) {
            this.searchItem = searchItem;
        }

        public SearchItemOrHeader(final String header) {
            this.header = header;
        }

        boolean isHeader() {
            return header != null;
        }

        @Override
        public boolean equals(final Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            final SearchItemOrHeader that = (SearchItemOrHeader) o;
            return Objects.equals(header, that.header) &&
                    Objects.equals(searchItem, that.searchItem);
        }

        @Override
        public int hashCode() {
            return Objects.hash(header, searchItem);
        }
    }

    public static class HeaderViewHolder extends RecyclerView.ViewHolder {
        private final ItemFavSectionHeaderBinding binding;

        public HeaderViewHolder(@NonNull final ItemFavSectionHeaderBinding binding) {
            super(binding.getRoot());
            this.binding = binding;
        }

        public void bind(final String header) {
            if (header == null) return;
            final int headerText;
            switch (header) {
                case RECENT:
                    headerText = R.string.recent;
                    break;
                case FAVORITE:
                    headerText = R.string.title_favorites;
                    break;
                default:
                    headerText = R.string.unknown;
                    break;
            }
            binding.getRoot().setText(headerText);
        }
    }
}