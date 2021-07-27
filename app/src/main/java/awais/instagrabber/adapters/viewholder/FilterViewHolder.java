package awais.instagrabber.adapters.viewholder;

import android.graphics.Bitmap;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.google.common.collect.ImmutableList;

import java.util.Collection;

import awais.instagrabber.adapters.FiltersAdapter;
import awais.instagrabber.databinding.ItemFilterBinding;
import awais.instagrabber.fragments.imageedit.filters.filters.Filter;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.BitmapUtils;
import jp.co.cyberagent.android.gpuimage.GPUImage;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageFilter;

public class FilterViewHolder extends RecyclerView.ViewHolder {
    private static final String TAG = FilterViewHolder.class.getSimpleName();

    private final ItemFilterBinding binding;
    private final Collection<GPUImageFilter> tuneFilters;
    private final FiltersAdapter.OnFilterClickListener onFilterClickListener;
    private final AppExecutors appExecutors;

    public FilterViewHolder(@NonNull final ItemFilterBinding binding,
                            final Collection<GPUImageFilter> tuneFilters,
                            final FiltersAdapter.OnFilterClickListener onFilterClickListener) {
        super(binding.getRoot());
        this.binding = binding;
        this.tuneFilters = tuneFilters;
        this.onFilterClickListener = onFilterClickListener;
        appExecutors = AppExecutors.getInstance();
    }

    public void bind(final int position, final String originalKey, final Bitmap originalBitmap, final Filter<?> item, final boolean isSelected) {
        if (originalBitmap == null || item == null) return;
        if (onFilterClickListener != null) {
            itemView.setOnClickListener(v -> onFilterClickListener.onClick(position, item));
        }
        if (item.getLabel() != -1) {
            binding.name.setVisibility(View.VISIBLE);
            binding.name.setText(item.getLabel());
            binding.name.setSelected(isSelected);
        } else {
            binding.name.setVisibility(View.GONE);
        }
        final String filterKey = item.getLabel() + "_" + originalKey;
        // avoid resetting the bitmap
        if (binding.preview.getTag() != null && binding.preview.getTag().equals(filterKey)) return;
        binding.preview.setTag(filterKey);
        final Bitmap bitmap = BitmapUtils.getBitmapFromMemCache(filterKey);
        if (bitmap == null) {
            final GPUImageFilter filter = item.getInstance();
            appExecutors.tasksThread().submit(() -> {
                GPUImage.getBitmapForMultipleFilters(
                        originalBitmap,
                        ImmutableList.<GPUImageFilter>builder().add(filter).addAll(tuneFilters).build(),
                        filteredBitmap -> {
                            BitmapUtils.addBitmapToMemoryCache(filterKey, filteredBitmap, true);
                            appExecutors.mainThread().execute(() -> binding.getRoot().post(() -> binding.preview.setImageBitmap(filteredBitmap)));
                        }
                );
            });
            return;
        }
        binding.getRoot().post(() -> binding.preview.setImageBitmap(bitmap));
    }
}
