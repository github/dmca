package awais.instagrabber.adapters.viewholder;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.net.Uri;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.palette.graphics.Palette;
import androidx.recyclerview.widget.RecyclerView;

import com.facebook.common.executors.CallerThreadExecutor;
import com.facebook.common.references.CloseableReference;
import com.facebook.datasource.DataSource;
import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.imagepipeline.core.ImagePipeline;
import com.facebook.imagepipeline.datasource.BaseBitmapDataSubscriber;
import com.facebook.imagepipeline.image.CloseableImage;
import com.facebook.imagepipeline.request.ImageRequest;
import com.facebook.imagepipeline.request.ImageRequestBuilder;

import java.util.concurrent.atomic.AtomicInteger;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DiscoverTopicsAdapter;
import awais.instagrabber.adapters.SavedCollectionsAdapter;
import awais.instagrabber.databinding.ItemDiscoverTopicBinding;
import awais.instagrabber.repositories.responses.discover.TopicCluster;
import awais.instagrabber.repositories.responses.saved.SavedCollection;
import awais.instagrabber.utils.ResponseBodyUtils;

public class TopicClusterViewHolder extends RecyclerView.ViewHolder {
    private final ItemDiscoverTopicBinding binding;
    private final DiscoverTopicsAdapter.OnTopicClickListener onTopicClickListener;
    private final SavedCollectionsAdapter.OnCollectionClickListener onCollectionClickListener;

    public TopicClusterViewHolder(@NonNull final ItemDiscoverTopicBinding binding,
                                  final DiscoverTopicsAdapter.OnTopicClickListener onTopicClickListener,
                                  final SavedCollectionsAdapter.OnCollectionClickListener onCollectionClickListener) {
        super(binding.getRoot());
        this.binding = binding;
        this.onTopicClickListener = onTopicClickListener;
        this.onCollectionClickListener = onCollectionClickListener;
    }

    public void bind(final TopicCluster topicCluster) {
        if (topicCluster == null) {
            return;
        }
        final AtomicInteger titleColor = new AtomicInteger(-1);
        final AtomicInteger backgroundColor = new AtomicInteger(-1);
        if (onTopicClickListener != null) {
            itemView.setOnClickListener(v -> onTopicClickListener.onTopicClick(
                    topicCluster,
                    binding.cover,
                    titleColor.get(),
                    backgroundColor.get()
            ));
            itemView.setOnLongClickListener(v -> {
                onTopicClickListener.onTopicLongClick(topicCluster.getCoverMedia());
                return true;
            });
        }
        // binding.title.setTransitionName("title-" + topicCluster.getId());
        binding.cover.setTransitionName("cover-" + topicCluster.getId());
        final String thumbUrl = ResponseBodyUtils.getThumbUrl(topicCluster.getCoverMedia());
        if (thumbUrl == null) {
            binding.cover.setImageURI((String) null);
        } else {
            final ImageRequest imageRequest = ImageRequestBuilder
                    .newBuilderWithSource(Uri.parse(thumbUrl))
                    .build();
            final ImagePipeline imagePipeline = Fresco.getImagePipeline();
            final DataSource<CloseableReference<CloseableImage>> dataSource = imagePipeline
                    .fetchDecodedImage(imageRequest, CallerThreadExecutor.getInstance());
            dataSource.subscribe(new BaseBitmapDataSubscriber() {
                @Override
                public void onNewResultImpl(@Nullable Bitmap bitmap) {
                    if (dataSource.isFinished()) {
                        dataSource.close();
                    }
                    if (bitmap != null) {
                        Palette.from(bitmap).generate(p -> {
                            final Resources resources = itemView.getResources();
                            int titleTextColor = resources.getColor(R.color.white);
                            if (p != null) {
                                final Palette.Swatch swatch = p.getDominantSwatch();
                                if (swatch != null) {
                                    backgroundColor.set(swatch.getRgb());
                                    GradientDrawable gd = new GradientDrawable(
                                            GradientDrawable.Orientation.TOP_BOTTOM,
                                            new int[]{Color.TRANSPARENT, backgroundColor.get()});
                                    titleTextColor = swatch.getTitleTextColor();
                                    binding.background.setBackground(gd);
                                }
                            }
                            titleColor.set(titleTextColor);
                            binding.title.setTextColor(titleTextColor);
                        });
                    }
                }

                @Override
                public void onFailureImpl(@NonNull DataSource dataSource) {
                    dataSource.close();
                }
            }, CallerThreadExecutor.getInstance());
            binding.cover.setImageRequest(imageRequest);
        }
        binding.title.setText(topicCluster.getTitle());
    }

    public void bind(final SavedCollection topicCluster) {
        if (topicCluster == null) {
            return;
        }
        final AtomicInteger titleColor = new AtomicInteger(-1);
        final AtomicInteger backgroundColor = new AtomicInteger(-1);
        if (onCollectionClickListener != null) {
            itemView.setOnClickListener(v -> onCollectionClickListener.onCollectionClick(
                    topicCluster,
                    binding.getRoot(),
                    binding.cover,
                    binding.title,
                    titleColor.get(),
                    backgroundColor.get()
            ));
        }
        // binding.title.setTransitionName("title-" + topicCluster.getId());
        binding.cover.setTransitionName("cover-" + topicCluster.getId());
        final String thumbUrl = ResponseBodyUtils.getThumbUrl(topicCluster.getCoverMedias() == null
                                                              ? topicCluster.getCoverMedia()
                                                              : topicCluster.getCoverMedias().get(0));
        if (thumbUrl == null) {
            binding.cover.setImageURI((String) null);
        } else {
            final ImageRequest imageRequest = ImageRequestBuilder
                    .newBuilderWithSource(Uri.parse(thumbUrl))
                    .build();
            final ImagePipeline imagePipeline = Fresco.getImagePipeline();
            final DataSource<CloseableReference<CloseableImage>> dataSource = imagePipeline
                    .fetchDecodedImage(imageRequest, CallerThreadExecutor.getInstance());
            dataSource.subscribe(new BaseBitmapDataSubscriber() {
                @Override
                public void onNewResultImpl(@Nullable Bitmap bitmap) {
                    if (dataSource.isFinished()) {
                        dataSource.close();
                    }
                    if (bitmap != null) {
                        Palette.from(bitmap).generate(p -> {
                            final Palette.Swatch swatch = p.getDominantSwatch();
                            final Resources resources = itemView.getResources();
                            int titleTextColor = resources.getColor(R.color.white);
                            if (swatch != null) {
                                backgroundColor.set(swatch.getRgb());
                                GradientDrawable gd = new GradientDrawable(
                                        GradientDrawable.Orientation.TOP_BOTTOM,
                                        new int[]{Color.TRANSPARENT, backgroundColor.get()});
                                titleTextColor = swatch.getTitleTextColor();
                                binding.background.setBackground(gd);
                            }
                            titleColor.set(titleTextColor);
                            binding.title.setTextColor(titleTextColor);
                        });
                    }
                }

                @Override
                public void onFailureImpl(@NonNull DataSource dataSource) {
                    dataSource.close();
                }
            }, CallerThreadExecutor.getInstance());
            binding.cover.setImageRequest(imageRequest);
        }
        binding.title.setText(topicCluster.getTitle());
    }
}
