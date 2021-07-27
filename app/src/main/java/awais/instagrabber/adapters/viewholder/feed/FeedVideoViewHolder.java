package awais.instagrabber.adapters.viewholder.feed;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;

import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;
import com.google.android.exoplayer2.upstream.cache.CacheDataSourceFactory;
import com.google.android.exoplayer2.upstream.cache.SimpleCache;

import java.util.List;

import awais.instagrabber.adapters.FeedAdapterV2;
import awais.instagrabber.customviews.VideoPlayerCallbackAdapter;
import awais.instagrabber.customviews.VideoPlayerViewHelper;
import awais.instagrabber.databinding.ItemFeedVideoBinding;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.VideoVersion;
import awais.instagrabber.utils.NumberUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class FeedVideoViewHolder extends FeedItemViewHolder {
    private static final String TAG = "FeedVideoViewHolder";

    private final ItemFeedVideoBinding binding;
    private final FeedAdapterV2.FeedItemCallback feedItemCallback;
    private final Handler handler;
    private final DefaultDataSourceFactory dataSourceFactory;

    private CacheDataSourceFactory cacheDataSourceFactory;
    private Media media;

    // private final Runnable loadRunnable = new Runnable() {
    //     @Override
    //     public void run() {
    //         // loadPlayer(feedModel);
    //     }
    // };

    public FeedVideoViewHolder(@NonNull final ItemFeedVideoBinding binding,
                               final FeedAdapterV2.FeedItemCallback feedItemCallback) {
        super(binding.getRoot(), binding.itemFeedTop, binding.itemFeedBottom, feedItemCallback);
        this.binding = binding;
        this.feedItemCallback = feedItemCallback;
        binding.itemFeedBottom.btnViews.setVisibility(View.VISIBLE);
        handler = new Handler(Looper.getMainLooper());
        final Context context = binding.getRoot().getContext();
        dataSourceFactory = new DefaultDataSourceFactory(context, "instagram");
        final SimpleCache simpleCache = Utils.getSimpleCacheInstance(context);
        if (simpleCache != null) {
            cacheDataSourceFactory = new CacheDataSourceFactory(simpleCache, dataSourceFactory);
        }
    }

    @Override
    public void bindItem(final Media media) {
        // Log.d(TAG, "Binding post: " + feedModel.getPostId());
        this.media = media;
        binding.itemFeedBottom.tvVideoViews.setText(String.valueOf(media.getViewCount()));
        final float vol = settingsHelper.getBoolean(PreferenceKeys.MUTED_VIDEOS) ? 0f : 1f;
        final VideoPlayerViewHelper.VideoPlayerCallback videoPlayerCallback = new VideoPlayerCallbackAdapter() {

            @Override
            public void onThumbnailClick() {
                feedItemCallback.onPostClick(media, binding.itemFeedTop.ivProfilePic, binding.videoPost.thumbnail);
            }

            @Override
            public void onPlayerViewLoaded() {
                final ViewGroup.LayoutParams layoutParams = binding.videoPost.playerView.getLayoutParams();
                final int requiredWidth = Utils.displayMetrics.widthPixels;
                final int resultingHeight = NumberUtils.getResultingHeight(requiredWidth, media.getOriginalHeight(), media.getOriginalWidth());
                layoutParams.width = requiredWidth;
                layoutParams.height = resultingHeight;
                binding.videoPost.playerView.requestLayout();
            }
        };
        final float aspectRatio = (float) media.getOriginalWidth() / media.getOriginalHeight();
        String videoUrl = null;
        final List<VideoVersion> videoVersions = media.getVideoVersions();
        if (videoVersions != null && !videoVersions.isEmpty()) {
            final VideoVersion videoVersion = videoVersions.get(0);
            videoUrl = videoVersion.getUrl();
        }
        final VideoPlayerViewHelper videoPlayerViewHelper = new VideoPlayerViewHelper(binding.getRoot().getContext(),
                                                                                      binding.videoPost,
                                                                                      videoUrl,
                                                                                      vol,
                                                                                      aspectRatio,
                                                                                      ResponseBodyUtils.getThumbUrl(media),
                                                                                      false,
                                                                                      // null,
                                                                                      videoPlayerCallback);
        binding.videoPost.thumbnail.post(() -> {
            if (media.getOriginalHeight() > 0.8 * Utils.displayMetrics.heightPixels) {
                final ViewGroup.LayoutParams layoutParams = binding.videoPost.thumbnail.getLayoutParams();
                layoutParams.height = (int) (0.8 * Utils.displayMetrics.heightPixels);
                binding.videoPost.thumbnail.requestLayout();
            }
        });
    }

    public Media getCurrentFeedModel() {
        return media;
    }

    // public void stopPlaying() {
    //     // Log.d(TAG, "Stopping post: " + feedModel.getPostId() + ", player: " + player + ", player.isPlaying: " + (player != null && player.isPlaying()));
    //     handler.removeCallbacks(loadRunnable);
    //     if (player != null) {
    //         player.release();
    //     }
    //     if (binding.videoPost.root.getDisplayedChild() == 1) {
    //         binding.videoPost.root.showPrevious();
    //     }
    // }
    //
    // public void startPlaying() {
    //     handler.removeCallbacks(loadRunnable);
    //     handler.postDelayed(loadRunnable, 800);
    // }
}
