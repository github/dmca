package awais.instagrabber.adapters;

import android.net.Uri;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.drawee.backends.pipeline.PipelineDraweeControllerBuilder;
import com.facebook.drawee.controller.BaseControllerListener;
import com.facebook.drawee.drawable.ScalingUtils;
import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.imagepipeline.common.ResizeOptions;
import com.facebook.imagepipeline.image.ImageInfo;
import com.facebook.imagepipeline.request.ImageRequest;
import com.facebook.imagepipeline.request.ImageRequestBuilder;

import java.util.Objects;

import awais.instagrabber.databinding.ItemMediaBinding;
import awais.instagrabber.repositories.responses.giphy.GiphyGif;
import awais.instagrabber.utils.Utils;

public class GifItemsAdapter extends ListAdapter<GiphyGif, GifItemsAdapter.GifViewHolder> {

    private static final DiffUtil.ItemCallback<GiphyGif> diffCallback = new DiffUtil.ItemCallback<GiphyGif>() {
        @Override
        public boolean areItemsTheSame(@NonNull final GiphyGif oldItem, @NonNull final GiphyGif newItem) {
            return Objects.equals(oldItem.getId(), newItem.getId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final GiphyGif oldItem, @NonNull final GiphyGif newItem) {
            return Objects.equals(oldItem.getId(), newItem.getId());
        }
    };

    private final OnItemClickListener onItemClickListener;

    public GifItemsAdapter(final OnItemClickListener onItemClickListener) {
        super(diffCallback);
        this.onItemClickListener = onItemClickListener;
    }

    @NonNull
    @Override
    public GifViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemMediaBinding binding = ItemMediaBinding.inflate(layoutInflater, parent, false);
        return new GifViewHolder(binding, onItemClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final GifViewHolder holder, final int position) {
        holder.bind(getItem(position));
    }

    public static class GifViewHolder extends RecyclerView.ViewHolder {
        private static final String TAG = GifViewHolder.class.getSimpleName();
        private static final int size = Utils.displayMetrics.widthPixels / 3;

        private final ItemMediaBinding binding;
        private final OnItemClickListener onItemClickListener;

        public GifViewHolder(@NonNull final ItemMediaBinding binding,
                             final OnItemClickListener onItemClickListener) {
            super(binding.getRoot());
            this.binding = binding;
            this.onItemClickListener = onItemClickListener;
            binding.duration.setVisibility(View.GONE);
            final GenericDraweeHierarchyBuilder builder = new GenericDraweeHierarchyBuilder(itemView.getResources());
            builder.setActualImageScaleType(ScalingUtils.ScaleType.FIT_CENTER);
            binding.item.setHierarchy(builder.build());
        }

        public void bind(final GiphyGif item) {
            if (onItemClickListener != null) {
                itemView.setOnClickListener(v -> onItemClickListener.onItemClick(item));
            }
            final BaseControllerListener<ImageInfo> controllerListener = new BaseControllerListener<ImageInfo>() {
                @Override
                public void onFailure(final String id, final Throwable throwable) {
                    Log.e(TAG, "onFailure: ", throwable);
                }
            };
            final ImageRequest request = ImageRequestBuilder
                    .newBuilderWithSource(Uri.parse(item.getImages().getFixedHeight().getWebp()))
                    .setResizeOptions(ResizeOptions.forDimensions(size, size))
                    .build();
            final PipelineDraweeControllerBuilder builder = Fresco.newDraweeControllerBuilder()
                                                                  .setImageRequest(request)
                                                                  .setAutoPlayAnimations(true)
                                                                  .setControllerListener(controllerListener);
            binding.item.setController(builder.build());
        }
    }

    public interface OnItemClickListener {
        void onItemClick(GiphyGif giphyGif);
    }
}
