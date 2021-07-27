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
import com.facebook.imagepipeline.common.ResizeOptions;
import com.facebook.imagepipeline.image.ImageInfo;
import com.facebook.imagepipeline.request.ImageRequest;
import com.facebook.imagepipeline.request.ImageRequestBuilder;

import java.io.File;

import awais.instagrabber.databinding.ItemMediaBinding;
import awais.instagrabber.utils.MediaController.MediaEntry;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

public class MediaItemsAdapter extends ListAdapter<MediaEntry, MediaItemsAdapter.MediaItemViewHolder> {

    private static final DiffUtil.ItemCallback<MediaEntry> diffCallback = new DiffUtil.ItemCallback<MediaEntry>() {
        @Override
        public boolean areItemsTheSame(@NonNull final MediaEntry oldItem, @NonNull final MediaEntry newItem) {
            return oldItem.imageId == newItem.imageId;
        }

        @Override
        public boolean areContentsTheSame(@NonNull final MediaEntry oldItem, @NonNull final MediaEntry newItem) {
            return oldItem.imageId == newItem.imageId;
        }
    };

    private final OnItemClickListener onItemClickListener;

    public MediaItemsAdapter(final OnItemClickListener onItemClickListener) {
        super(diffCallback);
        this.onItemClickListener = onItemClickListener;
    }

    @NonNull
    @Override
    public MediaItemViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemMediaBinding binding = ItemMediaBinding.inflate(layoutInflater, parent, false);
        return new MediaItemViewHolder(binding, onItemClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final MediaItemViewHolder holder, final int position) {
        holder.bind(getItem(position));
    }

    public static class MediaItemViewHolder extends RecyclerView.ViewHolder {
        private static final String TAG = MediaItemViewHolder.class.getSimpleName();
        private static final int size = Utils.displayMetrics.widthPixels / 3;

        private final ItemMediaBinding binding;
        private final OnItemClickListener onItemClickListener;

        public MediaItemViewHolder(@NonNull final ItemMediaBinding binding,
                                   final OnItemClickListener onItemClickListener) {
            super(binding.getRoot());
            this.binding = binding;
            this.onItemClickListener = onItemClickListener;
        }

        public void bind(final MediaEntry item) {
            final Uri uri = Uri.fromFile(new File(item.path));
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
                    .newBuilderWithSource(uri)
                    .setLocalThumbnailPreviewsEnabled(true)
                    .setProgressiveRenderingEnabled(false)
                    .setResizeOptions(ResizeOptions.forDimensions(size, size))
                    .build();
            final PipelineDraweeControllerBuilder builder = Fresco.newDraweeControllerBuilder()
                                                                  .setImageRequest(request)
                                                                  .setControllerListener(controllerListener);
            binding.item.setController(builder.build());
            if (item.isVideo && item.duration >= 0) {
                final String timeString = TextUtils.millisToTimeString(item.duration);
                binding.duration.setVisibility(View.VISIBLE);
                binding.duration.setText(timeString);
            } else {
                binding.duration.setVisibility(View.GONE);
            }
        }
    }

    public interface OnItemClickListener {
        void onItemClick(MediaEntry entry);
    }
}
