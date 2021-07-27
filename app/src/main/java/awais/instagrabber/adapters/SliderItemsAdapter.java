package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import com.google.android.exoplayer2.ui.StyledPlayerView;

import awais.instagrabber.adapters.viewholder.SliderItemViewHolder;
import awais.instagrabber.adapters.viewholder.SliderPhotoViewHolder;
import awais.instagrabber.adapters.viewholder.SliderVideoViewHolder;
import awais.instagrabber.databinding.ItemSliderPhotoBinding;
import awais.instagrabber.databinding.LayoutVideoPlayerWithThumbnailBinding;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Media;

public final class SliderItemsAdapter extends ListAdapter<Media, SliderItemViewHolder> {

    private final boolean loadVideoOnItemClick;
    private final SliderCallback sliderCallback;

    private static final DiffUtil.ItemCallback<Media> DIFF_CALLBACK = new DiffUtil.ItemCallback<Media>() {
        @Override
        public boolean areItemsTheSame(@NonNull final Media oldItem, @NonNull final Media newItem) {
            return oldItem.getPk().equals(newItem.getPk());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final Media oldItem, @NonNull final Media newItem) {
            return oldItem.getPk().equals(newItem.getPk());
        }
    };

    public SliderItemsAdapter(final boolean loadVideoOnItemClick,
                              final SliderCallback sliderCallback) {
        super(DIFF_CALLBACK);
        this.loadVideoOnItemClick = loadVideoOnItemClick;
        this.sliderCallback = sliderCallback;
    }

    @NonNull
    @Override
    public SliderItemViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        final MediaItemType mediaItemType = MediaItemType.valueOf(viewType);
        switch (mediaItemType) {
            case MEDIA_TYPE_VIDEO: {
                final LayoutVideoPlayerWithThumbnailBinding binding = LayoutVideoPlayerWithThumbnailBinding.inflate(inflater, parent, false);
                return new SliderVideoViewHolder(binding, loadVideoOnItemClick);
            }
            case MEDIA_TYPE_IMAGE:
            default:
                final ItemSliderPhotoBinding binding = ItemSliderPhotoBinding.inflate(inflater, parent, false);
                return new SliderPhotoViewHolder(binding);
        }
    }

    @Override
    public void onBindViewHolder(@NonNull final SliderItemViewHolder holder, final int position) {
        final Media media = getItem(position);
        holder.bind(media, position, sliderCallback);
    }

    @Override
    public int getItemViewType(final int position) {
        final Media media = getItem(position);
        return media.getMediaType().getId();
    }

    // @NonNull
    // @Override
    // public Object instantiateItem(@NonNull final ViewGroup container, final int position) {
    //     final Context context = container.getContext();
    //     final ViewerPostModel sliderItem = sliderItems.get(position);
    //
    //     if (sliderItem.getItemType() == MediaItemType.MEDIA_TYPE_VIDEO) {
    //         final ViewSwitcher viewSwitcher = createViewSwitcher(context, position, sliderItem.getThumbnailUrl(), sliderItem.getDisplayUrl());
    //         container.addView(viewSwitcher);
    //         return viewSwitcher;
    //     }
    //     final GenericDraweeHierarchy hierarchy = GenericDraweeHierarchyBuilder.newInstance(container.getResources())
    //                                                                           .setActualImageScaleType(ScalingUtils.ScaleType.FIT_CENTER)
    //                                                                           .build();
    //     final SimpleDraweeView photoView = new SimpleDraweeView(context, hierarchy);
    //     photoView.setLayoutParams(layoutParams);
    //     final ImageRequest imageRequest = ImageRequestBuilder.newBuilderWithSource(Uri.parse(sliderItem.getDisplayUrl()))
    //                                                          .setLocalThumbnailPreviewsEnabled(true)
    //                                                          .setProgressiveRenderingEnabled(true)
    //                                                          .build();
    //     photoView.setImageRequest(imageRequest);
    //     container.addView(photoView);
    //     return photoView;
    // }

    // @NonNull
    // private ViewSwitcher createViewSwitcher(final Context context,
    //                                         final int position,
    //                                         final String thumbnailUrl,
    //                                         final String displayUrl) {
    //
    //     final ViewSwitcher viewSwitcher = new ViewSwitcher(context);
    //     viewSwitcher.setLayoutParams(layoutParams);
    //
    //     final FrameLayout frameLayout = new FrameLayout(context);
    //     frameLayout.setLayoutParams(layoutParams);
    //
    //     final GenericDraweeHierarchy hierarchy = new GenericDraweeHierarchyBuilder(context.getResources())
    //             .setActualImageScaleType(ScalingUtils.ScaleType.FIT_CENTER)
    //             .build();
    //     final SimpleDraweeView simpleDraweeView = new SimpleDraweeView(context, hierarchy);
    //     simpleDraweeView.setLayoutParams(layoutParams);
    //     simpleDraweeView.setImageURI(thumbnailUrl);
    //     frameLayout.addView(simpleDraweeView);
    //
    //     final AppCompatImageView imageView = new AppCompatImageView(context);
    //     final int px = Utils.convertDpToPx(50);
    //     final FrameLayout.LayoutParams playButtonLayoutParams = new FrameLayout.LayoutParams(px, px);
    //     playButtonLayoutParams.gravity = Gravity.CENTER;
    //     imageView.setLayoutParams(playButtonLayoutParams);
    //     imageView.setImageResource(R.drawable.exo_icon_play);
    //     frameLayout.addView(imageView);
    //
    //     viewSwitcher.addView(frameLayout);
    //
    //     final PlayerView playerView = new PlayerView(context);
    //     viewSwitcher.addView(playerView);
    //     if (shouldAutoPlay && position == 0) {
    //         loadPlayer(context, position, displayUrl, viewSwitcher, factory, playerChangeListener);
    //     } else
    //         frameLayout.setOnClickListener(v -> loadPlayer(context, position, displayUrl, viewSwitcher, factory, playerChangeListener));
    //     return viewSwitcher;
    // }

    public interface SliderCallback {
        void onThumbnailLoaded(int position);

        void onItemClicked(int position, final Media media, final View view);

        void onPlayerPlay(int position);

        void onPlayerPause(int position);

        void onPlayerRelease(int position);

        void onFullScreenModeChanged(boolean isFullScreen, final StyledPlayerView playerView);

        boolean isInFullScreen();
    }
}
