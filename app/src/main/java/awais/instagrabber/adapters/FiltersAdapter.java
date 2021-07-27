package awais.instagrabber.adapters;

import android.graphics.Bitmap;
import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import java.util.Collection;
import java.util.List;

import awais.instagrabber.adapters.viewholder.FilterViewHolder;
import awais.instagrabber.databinding.ItemFilterBinding;
import awais.instagrabber.fragments.imageedit.filters.filters.Filter;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageFilter;

public class FiltersAdapter extends ListAdapter<Filter<?>, FilterViewHolder> {

    private static final DiffUtil.ItemCallback<Filter<?>> DIFF_CALLBACK = new DiffUtil.ItemCallback<Filter<?>>() {
        @Override
        public boolean areItemsTheSame(@NonNull final Filter<?> oldItem, @NonNull final Filter<?> newItem) {
            return oldItem.getType().equals(newItem.getType());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final Filter<?> oldItem, @NonNull final Filter<?> newItem) {
            return oldItem.getType().equals(newItem.getType());
        }
    };

    private final Bitmap bitmap;
    private final OnFilterClickListener onFilterClickListener;
    private final Collection<GPUImageFilter> filters;
    private final String originalKey;
    private int selectedPosition = 0;

    public FiltersAdapter(final Collection<GPUImageFilter> filters,
                          final String originalKey,
                          final Bitmap bitmap,
                          final OnFilterClickListener onFilterClickListener) {
        super(DIFF_CALLBACK);
        this.filters = filters;
        this.originalKey = originalKey;
        this.bitmap = bitmap;
        this.onFilterClickListener = onFilterClickListener;
        setHasStableIds(true);
    }

    @NonNull
    @Override
    public FilterViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemFilterBinding binding = ItemFilterBinding.inflate(layoutInflater, parent, false);
        return new FilterViewHolder(binding, filters, onFilterClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final FilterViewHolder holder, final int position) {
        holder.bind(position, originalKey, bitmap, getItem(position), selectedPosition == position);
    }

    @Override
    public long getItemId(final int position) {
        return getItem(position).getLabel();
    }

    public void setSelected(final int position) {
        final int prev = this.selectedPosition;
        this.selectedPosition = position;
        notifyItemChanged(position);
        notifyItemChanged(prev);
    }

    public void setSelectedFilter(final GPUImageFilter instance) {
        final List<Filter<?>> currentList = getCurrentList();
        int index = -1;
        for (int i = 0; i < currentList.size(); i++) {
            final Filter<?> filter = currentList.get(i);
            final GPUImageFilter filterInstance = filter.getInstance();
            if (filterInstance.getClass() == instance.getClass()) {
                index = i;
                break;
            }
        }
        if (index < 0) return;
        setSelected(index);
    }

    public interface OnFilterClickListener {
        void onClick(int position, Filter<?> filter);
    }
}
