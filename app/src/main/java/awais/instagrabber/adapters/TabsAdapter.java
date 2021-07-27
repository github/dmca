package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import com.google.common.collect.ImmutableList;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.adapters.viewholder.TabViewHolder;
import awais.instagrabber.databinding.ItemFavSectionHeaderBinding;
import awais.instagrabber.databinding.ItemTabOrderPrefBinding;
import awais.instagrabber.models.Tab;
import awais.instagrabber.utils.Utils;

public class TabsAdapter extends ListAdapter<TabsAdapter.TabOrHeader, RecyclerView.ViewHolder> {
    private static final DiffUtil.ItemCallback<TabOrHeader> DIFF_CALLBACK = new DiffUtil.ItemCallback<TabOrHeader>() {
        @Override
        public boolean areItemsTheSame(@NonNull final TabOrHeader oldItem, @NonNull final TabOrHeader newItem) {
            if (oldItem.isHeader() && newItem.isHeader()) {
                return oldItem.header == newItem.header;
            }
            if (!oldItem.isHeader() && !newItem.isHeader()) {
                final Tab oldTab = oldItem.tab;
                final Tab newTab = newItem.tab;
                return oldTab.getIconResId() == newTab.getIconResId()
                        && Objects.equals(oldTab.getTitle(), newTab.getTitle());
            }
            return false;
        }

        @Override
        public boolean areContentsTheSame(@NonNull final TabOrHeader oldItem, @NonNull final TabOrHeader newItem) {
            if (oldItem.isHeader() && newItem.isHeader()) {
                return oldItem.header == newItem.header;
            }
            if (!oldItem.isHeader() && !newItem.isHeader()) {
                final Tab oldTab = oldItem.tab;
                final Tab newTab = newItem.tab;
                return oldTab.getIconResId() == newTab.getIconResId()
                        && Objects.equals(oldTab.getTitle(), newTab.getTitle());
            }
            return false;
        }
    };

    private final TabAdapterCallback tabAdapterCallback;

    private List<Tab> current = new ArrayList<>();
    private List<Tab> others = new ArrayList<>();

    public TabsAdapter(@NonNull final TabAdapterCallback tabAdapterCallback) {
        super(DIFF_CALLBACK);
        this.tabAdapterCallback = tabAdapterCallback;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        if (viewType == 1) {
            final ItemTabOrderPrefBinding binding = ItemTabOrderPrefBinding.inflate(layoutInflater, parent, false);
            return new TabViewHolder(binding, tabAdapterCallback);
        }
        final ItemFavSectionHeaderBinding headerBinding = ItemFavSectionHeaderBinding.inflate(layoutInflater, parent, false);
        return new DirectUsersAdapter.HeaderViewHolder(headerBinding);
    }

    @Override
    public void onBindViewHolder(@NonNull final RecyclerView.ViewHolder holder, final int position) {
        if (holder instanceof DirectUsersAdapter.HeaderViewHolder) {
            ((DirectUsersAdapter.HeaderViewHolder) holder).bind(R.string.other_tabs);
            return;
        }
        if (holder instanceof TabViewHolder) {
            final Tab tab = getItem(position).tab;
            ((TabViewHolder) holder).bind(tab, others.contains(tab), current.size() == 5);
        }
    }

    @Override
    public int getItemViewType(final int position) {
        return getItem(position).isHeader() ? 0 : 1;
    }

    public void submitList(final List<Tab> current, final List<Tab> others, final Runnable commitCallback) {
        final ImmutableList.Builder<TabOrHeader> builder = ImmutableList.builder();
        if (current != null) {
            builder.addAll(current.stream()
                                  .map(TabOrHeader::new)
                                  .collect(Collectors.toList()));
        }
        builder.add(new TabOrHeader(R.string.other_tabs));
        if (others != null) {
            builder.addAll(others.stream()
                                 .map(TabOrHeader::new)
                                 .collect(Collectors.toList()));
        }
        // Mutable non-null copies
        this.current = current != null ? new ArrayList<>(current) : new ArrayList<>();
        this.others = others != null ? new ArrayList<>(others) : new ArrayList<>();
        submitList(builder.build(), commitCallback);
    }

    public void submitList(final List<Tab> current, final List<Tab> others) {
        submitList(current, others, null);
    }

    public void moveItem(final int from, final int to) {
        final List<Tab> currentCopy = new ArrayList<>(current);
        Utils.moveItem(from, to, currentCopy);
        submitList(currentCopy, others);
        tabAdapterCallback.onOrderChange(currentCopy);
    }

    public int getCurrentCount() {
        return current.size();
    }

    public static class TabOrHeader {
        Tab tab;
        int header;

        public TabOrHeader(final Tab tab) {
            this.tab = tab;
        }

        public TabOrHeader(@StringRes final int header) {
            this.header = header;
        }

        boolean isHeader() {
            return header != 0;
        }
    }

    public interface TabAdapterCallback {
        void onStartDrag(TabViewHolder viewHolder);

        void onOrderChange(List<Tab> newOrderTabs);

        void onAdd(Tab tab);

        void onRemove(Tab tab);
    }
}
