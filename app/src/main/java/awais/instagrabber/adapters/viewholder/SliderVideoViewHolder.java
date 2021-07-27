package awais.instagrabber.adapters.viewholder;

import android.annotation.SuppressLint;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ViewGroup;

import androidx.annotation.NonNull;

import com.google.android.exoplayer2.ui.StyledPlayerView;

import java.util.List;

import awais.instagrabber.adapters.SliderItemsAdapter;
import awais.instagrabber.customviews.VideoPlayerCallbackAdapter;
import awais.instagrabber.customviews.VideoPlayerViewHelper;
import awais.instagrabber.databinding.LayoutVideoPlayerWithThumbnailBinding;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.VideoVersion;
import awais.instagrabber.utils.NumberUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class SliderVideoViewHolder extends SliderItemViewHolder {
    private static final String TAG = "SliderVideoViewHolder";

    private final LayoutVideoPlayerWithThumbnailBinding binding;
    private final boolean loadVideoOnItemClick;

    private VideoPlayerViewHelper videoPlayerViewHelper;

    @SuppressLint("ClickableViewAccessibility")
    public SliderVideoViewHolder(@NonNull final LayoutVideoPlayerWithThumbnailBinding binding,
                                 final boolean loadVideoOnItemClick) {
        super(binding.getRoot());
        this.binding = binding;
        this.loadVideoOnItemClick = loadVideoOnItemClick;
        final GestureDetector.OnGestureListener videoPlayerViewGestureListener = new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onSingleTapConfirmed(final MotionEvent e) {
                binding.playerView.performClick();
                return true;
            }
        };
        final GestureDetector gestureDetector = new GestureDetector(itemView.getContext(), videoPlayerViewGestureListener);
        binding.playerView.setOnTouchListener((v, event) -> {
            gestureDetector.onTouchEvent(event);
            return true;
        });
    }

    public void bind(@NonNull final Media media,
                     final int position,
                     final SliderItemsAdapter.SliderCallback sliderCallback) {
        final float vol = settingsHelper.getBoolean(PreferenceKeys.MUTED_VIDEOS) ? 0f : 1f;
        final VideoPlayerViewHelper.VideoPlayerCallback videoPlayerCallback = new VideoPlayerCallbackAdapter() {

            @Override
            public void onThumbnailClick() {
                if (sliderCallback != null) {
                    sliderCallback.onItemClicked(position, media, binding.getRoot());
                }
            }

            @Override
            public void onThumbnailLoaded() {
                if (sliderCallback != null) {
                    sliderCallback.onThumbnailLoaded(position);
                }
            }

            @Override
            public void onPlayerViewLoaded() {
                // binding.itemFeedBottom.btnMute.setVisibility(View.VISIBLE);
                final ViewGroup.LayoutParams layoutParams = binding.playerView.getLayoutParams();
                final int requiredWidth = Utils.displayMetrics.widthPixels;
                final int resultingHeight = NumberUtils.getResultingHeight(requiredWidth, media.getOriginalHeight(), media.getOriginalWidth());
                layoutParams.width = requiredWidth;
                layoutParams.height = resultingHeight;
                binding.playerView.requestLayout();
                // setMuteIcon(vol == 0f && Utils.sessionVolumeFull ? 1f : vol);
            }

            @Override
            public void onPlay() {
                if (sliderCallback != null) {
                    sliderCallback.onPlayerPlay(position);
                }
            }

            @Override
            public void onPause() {
                if (sliderCallback != null) {
                    sliderCallback.onPlayerPause(position);
                }
            }

            @Override
            public void onRelease() {
                if (sliderCallback != null) {
                    sliderCallback.onPlayerRelease(position);
                }
            }

            @Override
            public void onFullScreenModeChanged(final boolean isFullScreen, final StyledPlayerView playerView) {
                if (sliderCallback != null) {
                    sliderCallback.onFullScreenModeChanged(isFullScreen, playerView);
                }
            }

            @Override
            public boolean isInFullScreen() {
                if (sliderCallback != null) {
                    return sliderCallback.isInFullScreen();
                }
                return false;
            }
        };
        final float aspectRatio = (float) media.getOriginalWidth() / media.getOriginalHeight();
        String videoUrl = null;
        final List<VideoVersion> videoVersions = media.getVideoVersions();
        if (videoVersions != null && !videoVersions.isEmpty()) {
            final VideoVersion videoVersion = videoVersions.get(0);
            if (videoVersion != null) {
                videoUrl = videoVersion.getUrl();
            }
        }
        if (videoUrl == null) return;
        videoPlayerViewHelper = new VideoPlayerViewHelper(binding.getRoot().getContext(),
                                                          binding,
                                                          videoUrl,
                                                          vol,
                                                          aspectRatio,
                                                          ResponseBodyUtils.getThumbUrl(media),
                                                          loadVideoOnItemClick,
                                                          videoPlayerCallback);
        binding.playerView.setOnClickListener(v -> {
            if (sliderCallback != null) {
                sliderCallback.onItemClicked(position, media, binding.getRoot());
            }
        });
    }

    public void pause() {
        if (videoPlayerViewHelper == null) return;
        videoPlayerViewHelper.pause();
    }

    public void releasePlayer() {
        if (videoPlayerViewHelper == null) return;
        videoPlayerViewHelper.releasePlayer();
    }
}
