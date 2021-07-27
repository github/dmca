package awais.instagrabber.customviews;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.drawable.Animatable;
import android.net.Uri;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatImageButton;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.drawee.backends.pipeline.PipelineDraweeControllerBuilder;
import com.facebook.drawee.controller.BaseControllerListener;
import com.facebook.imagepipeline.image.ImageInfo;
import com.facebook.imagepipeline.request.ImageRequest;
import com.facebook.imagepipeline.request.ImageRequestBuilder;
import com.google.android.exoplayer2.ExoPlaybackException;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.SimpleExoPlayer;
import com.google.android.exoplayer2.audio.AudioListener;
import com.google.android.exoplayer2.source.ProgressiveMediaSource;
import com.google.android.exoplayer2.source.TrackGroupArray;
import com.google.android.exoplayer2.trackselection.TrackSelectionArray;
import com.google.android.exoplayer2.ui.AspectRatioFrameLayout;
import com.google.android.exoplayer2.ui.StyledPlayerControlView;
import com.google.android.exoplayer2.ui.StyledPlayerView;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import awais.instagrabber.R;
import awais.instagrabber.databinding.LayoutVideoPlayerWithThumbnailBinding;
import awais.instagrabber.utils.Utils;

public class VideoPlayerViewHelper implements Player.EventListener {
    private static final String TAG = VideoPlayerViewHelper.class.getSimpleName();

    private final Context context;
    private final LayoutVideoPlayerWithThumbnailBinding binding;
    private final float initialVolume;
    private final float thumbnailAspectRatio;
    private final String thumbnailUrl;
    private final boolean loadPlayerOnClick;
    private final VideoPlayerCallback videoPlayerCallback;
    private final String videoUrl;
    private final DefaultDataSourceFactory dataSourceFactory;
    private SimpleExoPlayer player;
    private AppCompatImageButton mute;

    private final AudioListener audioListener = new AudioListener() {
        @Override
        public void onVolumeChanged(final float volume) {
            updateMuteIcon(volume);
        }
    };
    private final View.OnClickListener muteOnClickListener = v -> toggleMute();
    private Object layoutManager;

    public VideoPlayerViewHelper(@NonNull final Context context,
                                 @NonNull final LayoutVideoPlayerWithThumbnailBinding binding,
                                 @NonNull final String videoUrl,
                                 final float initialVolume,
                                 final float thumbnailAspectRatio,
                                 final String thumbnailUrl,
                                 final boolean loadPlayerOnClick,
                                 final VideoPlayerCallback videoPlayerCallback) {
        this.context = context;
        this.binding = binding;
        this.initialVolume = initialVolume;
        this.thumbnailAspectRatio = thumbnailAspectRatio;
        this.thumbnailUrl = thumbnailUrl;
        this.loadPlayerOnClick = loadPlayerOnClick;
        this.videoPlayerCallback = videoPlayerCallback;
        this.videoUrl = videoUrl;
        this.dataSourceFactory = new DefaultDataSourceFactory(binding.getRoot().getContext(), "instagram");
        bind();
    }

    private void bind() {
        binding.thumbnailParent.setOnClickListener(v -> {
            if (videoPlayerCallback != null) {
                videoPlayerCallback.onThumbnailClick();
            }
            if (loadPlayerOnClick) {
                loadPlayer();
            }
        });
        setThumbnail();
    }

    private void setThumbnail() {
        binding.thumbnail.setAspectRatio(thumbnailAspectRatio);
        ImageRequest thumbnailRequest = null;
        if (thumbnailUrl != null) {
            thumbnailRequest = ImageRequestBuilder.newBuilderWithSource(Uri.parse(thumbnailUrl)).build();
        }
        final PipelineDraweeControllerBuilder builder = Fresco
                .newDraweeControllerBuilder()
                .setControllerListener(new BaseControllerListener<ImageInfo>() {
                    @Override
                    public void onFailure(final String id, final Throwable throwable) {
                        if (videoPlayerCallback != null) {
                            videoPlayerCallback.onThumbnailLoaded();
                        }
                    }

                    @Override
                    public void onFinalImageSet(final String id,
                                                final ImageInfo imageInfo,
                                                final Animatable animatable) {
                        if (videoPlayerCallback != null) {
                            videoPlayerCallback.onThumbnailLoaded();
                        }
                    }
                });
        if (thumbnailRequest != null) {
            builder.setImageRequest(thumbnailRequest);
        }
        binding.thumbnail.setController(builder.build());
    }

    private void loadPlayer() {
        if (videoUrl == null) return;
        if (binding.getRoot().getDisplayedChild() == 0) {
            binding.getRoot().showNext();
        }
        if (videoPlayerCallback != null) {
            videoPlayerCallback.onPlayerViewLoaded();
        }
        player = (SimpleExoPlayer) binding.playerView.getPlayer();
        if (player != null) {
            player.release();
        }
        final ViewGroup.LayoutParams playerViewLayoutParams = binding.playerView.getLayoutParams();
        if (playerViewLayoutParams.height > Utils.displayMetrics.heightPixels * 0.8) {
            playerViewLayoutParams.height = (int) (Utils.displayMetrics.heightPixels * 0.8);
        }
        player = new SimpleExoPlayer.Builder(context)
                .setLooper(Looper.getMainLooper())
                .build();
        player.addListener(this);
        player.addAudioListener(audioListener);
        player.setVolume(initialVolume);
        player.setPlayWhenReady(true);
        player.setRepeatMode(Player.REPEAT_MODE_ALL);
        final ProgressiveMediaSource.Factory sourceFactory = new ProgressiveMediaSource.Factory(dataSourceFactory);
        final MediaItem mediaItem = MediaItem.fromUri(videoUrl);
        final ProgressiveMediaSource mediaSource = sourceFactory.createMediaSource(mediaItem);
        player.setMediaSource(mediaSource);
        player.prepare();
        binding.playerView.setPlayer(player);
        binding.playerView.setResizeMode(AspectRatioFrameLayout.RESIZE_MODE_FIT);
        binding.playerView.setShowNextButton(false);
        binding.playerView.setShowPreviousButton(false);
        binding.playerView.setControllerOnFullScreenModeChangedListener(isFullScreen -> {
            if (videoPlayerCallback == null) return;
            videoPlayerCallback.onFullScreenModeChanged(isFullScreen, binding.playerView);
        });
        setupControllerView();
    }

    private void setupControllerView() {
        try {
            final StyledPlayerControlView controllerView = getStyledPlayerControlView();
            if (controllerView == null) return;
            layoutManager = setControlViewLayoutManager(controllerView);
            if (videoPlayerCallback != null && videoPlayerCallback.isInFullScreen()) {
                setControllerViewToFullScreenMode(controllerView);
            }
            final ViewGroup exoBasicControls = controllerView.findViewById(R.id.exo_basic_controls);
            if (exoBasicControls == null) return;
            mute = new AppCompatImageButton(context);
            final Resources resources = context.getResources();
            if (resources == null) return;
            final int width = resources.getDimensionPixelSize(R.dimen.exo_small_icon_width);
            final int height = resources.getDimensionPixelSize(R.dimen.exo_small_icon_height);
            final int margin = resources.getDimensionPixelSize(R.dimen.exo_small_icon_horizontal_margin);
            final int paddingHorizontal = resources.getDimensionPixelSize(R.dimen.exo_small_icon_padding_horizontal);
            final int paddingVertical = resources.getDimensionPixelSize(R.dimen.exo_small_icon_padding_vertical);
            final ViewGroup.MarginLayoutParams layoutParams = new ViewGroup.MarginLayoutParams(width, height);
            layoutParams.setMargins(margin, 0, margin, 0);
            mute.setLayoutParams(layoutParams);
            mute.setPadding(paddingHorizontal, paddingVertical, paddingHorizontal, paddingVertical);
            mute.setScaleType(ImageView.ScaleType.FIT_XY);
            mute.setBackgroundResource(Utils.getAttrResId(context, android.R.attr.selectableItemBackground));
            mute.setImageTintList(ColorStateList.valueOf(resources.getColor(R.color.white)));
            updateMuteIcon(player.getVolume());
            exoBasicControls.addView(mute, 0);
            mute.setOnClickListener(muteOnClickListener);
        } catch (Exception e) {
            Log.e(TAG, "loadPlayer: ", e);
        }
    }

    @Nullable
    private Object setControlViewLayoutManager(@NonNull final StyledPlayerControlView controllerView)
            throws NoSuchFieldException, IllegalAccessException {
        final Field controlViewLayoutManagerField = controllerView.getClass().getDeclaredField("controlViewLayoutManager");
        controlViewLayoutManagerField.setAccessible(true);
        return controlViewLayoutManagerField.get(controllerView);
    }

    private void setControllerViewToFullScreenMode(@NonNull final StyledPlayerControlView controllerView)
            throws NoSuchFieldException, IllegalAccessException, NoSuchMethodException, InvocationTargetException {
        // Exoplayer doesn't expose the fullscreen state, so using reflection
        final Field fullScreenButtonField = controllerView.getClass().getDeclaredField("fullScreenButton");
        fullScreenButtonField.setAccessible(true);
        final ImageView fullScreenButton = (ImageView) fullScreenButtonField.get(controllerView);
        final Field isFullScreen = controllerView.getClass().getDeclaredField("isFullScreen");
        isFullScreen.setAccessible(true);
        isFullScreen.set(controllerView, true);
        final Method updateFullScreenButtonForState = controllerView
                .getClass()
                .getDeclaredMethod("updateFullScreenButtonForState", ImageView.class, boolean.class);
        updateFullScreenButtonForState.setAccessible(true);
        updateFullScreenButtonForState.invoke(controllerView, fullScreenButton, true);

    }

    @Nullable
    private StyledPlayerControlView getStyledPlayerControlView() throws NoSuchFieldException, IllegalAccessException {
        final Field controller = binding.playerView.getClass().getDeclaredField("controller");
        controller.setAccessible(true);
        return (StyledPlayerControlView) controller.get(binding.playerView);
    }

    @Override
    public void onTracksChanged(@NonNull TrackGroupArray trackGroups, @NonNull TrackSelectionArray trackSelections) {
        if (trackGroups.isEmpty()) {
            setHasAudio(false);
            return;
        }
        boolean hasAudio = false;
        for (int i = 0; i < trackGroups.length; i++) {
            for (int g = 0; g < trackGroups.get(i).length; g++) {
                final String sampleMimeType = trackGroups.get(i).getFormat(g).sampleMimeType;
                if (sampleMimeType != null && sampleMimeType.contains("audio")) {
                    hasAudio = true;
                    break;
                }
            }
        }
        setHasAudio(hasAudio);
    }

    private void setHasAudio(final boolean hasAudio) {
        if (mute == null) return;
        mute.setEnabled(hasAudio);
        mute.setAlpha(hasAudio ? 1f : 0.5f);
        updateMuteIcon(hasAudio ? 1f : 0f);
    }

    private void updateMuteIcon(final float volume) {
        if (mute == null) return;
        if (volume == 0) {
            mute.setImageResource(R.drawable.ic_volume_off_24);
            return;
        }
        mute.setImageResource(R.drawable.ic_volume_up_24);
    }

    @Override
    public void onPlayWhenReadyChanged(final boolean playWhenReady, final int reason) {
        if (videoPlayerCallback == null) return;
        if (playWhenReady) {
            videoPlayerCallback.onPlay();
            return;
        }
        videoPlayerCallback.onPause();
    }

    @Override
    public void onPlayerError(@NonNull final ExoPlaybackException error) {
        Log.e(TAG, "onPlayerError", error);
    }

    private void toggleMute() {
        if (player == null) return;
        if (layoutManager != null) {
            try {
                final Method resetHideCallbacks = layoutManager.getClass().getDeclaredMethod("resetHideCallbacks");
                resetHideCallbacks.invoke(layoutManager);
            } catch (Exception e) {
                Log.e(TAG, "toggleMute: ", e);
            }
        }
        final float vol = player.getVolume() == 0f ? 1f : 0f;
        player.setVolume(vol);
    }

    public void releasePlayer() {
        if (videoPlayerCallback != null) {
            videoPlayerCallback.onRelease();
        }
        if (player != null) {
            player.release();
            player = null;
        }
    }

    public void pause() {
        if (player != null) {
            player.pause();
        }
    }

    public interface VideoPlayerCallback {
        void onThumbnailLoaded();

        void onThumbnailClick();

        void onPlayerViewLoaded();

        void onPlay();

        void onPause();

        void onRelease();

        void onFullScreenModeChanged(boolean isFullScreen, final StyledPlayerView playerView);

        boolean isInFullScreen();
    }
}
