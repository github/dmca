package awais.instagrabber.adapters.viewholder.feed;

import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;

import androidx.annotation.NonNull;
import androidx.viewpager2.widget.ViewPager2;

import java.util.List;

import awais.instagrabber.adapters.FeedAdapterV2;
import awais.instagrabber.adapters.SliderCallbackAdapter;
import awais.instagrabber.adapters.SliderItemsAdapter;
import awais.instagrabber.databinding.ItemFeedSliderBinding;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.utils.NumberUtils;
import awais.instagrabber.utils.Utils;

public class FeedSliderViewHolder extends FeedItemViewHolder {
    private static final String TAG = "FeedSliderViewHolder";
    // private static final boolean shouldAutoPlay = settingsHelper.getBoolean(Constants.AUTOPLAY_VIDEOS);

    private final ItemFeedSliderBinding binding;
    private final FeedAdapterV2.FeedItemCallback feedItemCallback;

    public FeedSliderViewHolder(@NonNull final ItemFeedSliderBinding binding,
                                final FeedAdapterV2.FeedItemCallback feedItemCallback) {
        super(binding.getRoot(), binding.itemFeedTop, binding.itemFeedBottom, feedItemCallback);
        this.binding = binding;
        this.feedItemCallback = feedItemCallback;
        binding.itemFeedBottom.btnViews.setVisibility(View.GONE);
        // binding.itemFeedBottom.btnMute.setVisibility(View.GONE);
        final ViewGroup.LayoutParams layoutParams = binding.mediaList.getLayoutParams();
        layoutParams.height = Utils.displayMetrics.widthPixels + 1;
        binding.mediaList.setLayoutParams(layoutParams);
        // final Context context = binding.getRoot().getContext();
    }

    @Override
    public void bindItem(final Media feedModel) {
        final List<Media> sliderItems = feedModel.getCarouselMedia();
        final int sliderItemLen = sliderItems != null ? sliderItems.size() : 0;
        if (sliderItemLen <= 0) return;
        final String text = "1/" + sliderItemLen;
        binding.mediaCounter.setText(text);
        binding.mediaList.setOffscreenPageLimit(1);
        final SliderItemsAdapter adapter = new SliderItemsAdapter(false, new SliderCallbackAdapter() {
            @Override
            public void onItemClicked(final int position, final Media media, final View view) {
                feedItemCallback.onSliderClick(feedModel, position);
            }
        });
        binding.mediaList.setAdapter(adapter);
        binding.mediaList.registerOnPageChangeCallback(new ViewPager2.OnPageChangeCallback() {
            @Override
            public void onPageSelected(final int position) {
                if (position >= sliderItemLen) return;
                final String text = (position + 1) + "/" + sliderItemLen;
                binding.mediaCounter.setText(text);
                setDimensions(binding.mediaList, sliderItems.get(position));
                binding.itemFeedBottom.btnDownload.setOnClickListener(v -> feedItemCallback.onDownloadClick(feedModel, position));
            }
        });
        setDimensions(binding.mediaList, sliderItems.get(0));
        binding.itemFeedBottom.btnDownload.setOnClickListener(v -> feedItemCallback.onDownloadClick(feedModel, 0));
        adapter.submitList(sliderItems);
    }

    private void setDimensions(final View view, final Media model) {
        final ViewGroup.LayoutParams layoutParams = binding.mediaList.getLayoutParams();
        int requiredWidth = layoutParams.width;
        if (requiredWidth <= 0) {
            final ViewTreeObserver.OnPreDrawListener preDrawListener = new ViewTreeObserver.OnPreDrawListener() {
                @Override
                public boolean onPreDraw() {
                    view.getViewTreeObserver().removeOnPreDrawListener(this);
                    setLayoutParamDimens(binding.mediaList, model);
                    return true;
                }
            };
            view.getViewTreeObserver().addOnPreDrawListener(preDrawListener);
            return;
        }
        setLayoutParamDimens(binding.mediaList, model);
    }

    private void setLayoutParamDimens(final View view, final Media model) {
        final int requiredWidth = view.getMeasuredWidth();
        final ViewGroup.LayoutParams layoutParams = view.getLayoutParams();
        final int spanHeight = NumberUtils.getResultingHeight(requiredWidth, model.getOriginalHeight(), model.getOriginalWidth());
        layoutParams.height = spanHeight == 0 ? requiredWidth + 1 : spanHeight;
        view.requestLayout();
    }

    // private void autoPlay(final int position) {
    // if (!shouldAutoPlay) {
    //     return;
    // }
    // final ChildMediaItemsAdapter adapter = (ChildMediaItemsAdapter) binding.mediaList.getAdapter();
    // if (adapter == null) {
    //     return;
    // }
    // final ViewerPostModel sliderItem = adapter.getItemAtPosition(position);
    // if (sliderItem.getItemType() != MediaItemType.MEDIA_TYPE_VIDEO) {
    //     return;
    // }
    // final ViewSwitcher viewSwitcher = (ViewSwitcher) binding.mediaList.getChildAt(position);
    // loadPlayer(binding.getRoot().getContext(),
    //            position, sliderItem.getDisplayUrl(),
    //            viewSwitcher,
    //            cacheDataSourceFactory != null ? cacheDataSourceFactory : dataSourceFactory,
    //            playerChangeListener);
    // }

    // public void startPlayingVideo() {
    //     final int playerPosition = 0;
    //     autoPlay(playerPosition);
    // }
    //
    // public void stopPlayingVideo() {
    //     if (pagerPlayer == null) {
    //         return;
    //     }
    //     pagerPlayer.setPlayWhenReady(false);
    // }

    // private interface PlayerChangeListener {
    //     void playerChanged(final int position, final SimpleExoPlayer player);
    // }
    //
    // private static void loadPlayer(final Context context,
    //                                final int position,
    //                                final String displayUrl,
    //                                final ViewSwitcher viewSwitcher,
    //                                final DataSource.Factory factory,
    //                                final PlayerChangeListener playerChangeListener) {
    //     if (viewSwitcher == null) {
    //         return;
    //     }
    //     SimpleExoPlayer player = (SimpleExoPlayer) viewSwitcher.getTag();
    //     if (player != null) {
    //         player.setPlayWhenReady(true);
    //         return;
    //     }
    //     player = new SimpleExoPlayer.Builder(context).build();
    //     final PlayerView playerView = (PlayerView) viewSwitcher.getChildAt(1);
    //     playerView.setPlayer(player);
    //     if (viewSwitcher.getDisplayedChild() == 0) {
    //         viewSwitcher.showNext();
    //     }
    //     playerView.setControllerShowTimeoutMs(1000);
    //     float vol = settingsHelper.getBoolean(Constants.MUTED_VIDEOS) ? 0f : 1f;
    //     if (vol == 0f && Utils.sessionVolumeFull) vol = 1f;
    //     player.setVolume(vol);
    //     player.setPlayWhenReady(Utils.settingsHelper.getBoolean(Constants.AUTOPLAY_VIDEOS));
    //     final MediaItem mediaItem = MediaItem.fromUri(displayUrl);
    //     final ProgressiveMediaSource mediaSource = new ProgressiveMediaSource.Factory(factory).createMediaSource(mediaItem);
    //     player.setRepeatMode(Player.REPEAT_MODE_ALL);
    //     player.setMediaSource(mediaSource);
    //     player.prepare();
    //     player.setVolume(vol);
    //     playerChangeListener.playerChanged(position, player);
    //     viewSwitcher.setTag(player);
    // }
}
