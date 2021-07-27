package awais.instagrabber.customviews;

import com.google.android.exoplayer2.ui.StyledPlayerView;

public class VideoPlayerCallbackAdapter implements VideoPlayerViewHelper.VideoPlayerCallback {
    @Override
    public void onThumbnailLoaded() {}

    @Override
    public void onThumbnailClick() {}

    @Override
    public void onPlayerViewLoaded() {}

    @Override
    public void onPlay() {}

    @Override
    public void onPause() {}

    @Override
    public void onRelease() {}

    @Override
    public void onFullScreenModeChanged(final boolean isFullScreen, final StyledPlayerView playerView) {}

    @Override
    public boolean isInFullScreen() {
        return false;
    }
}
