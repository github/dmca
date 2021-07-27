package awais.instagrabber.adapters;

import android.view.View;

import com.google.android.exoplayer2.ui.StyledPlayerView;

import awais.instagrabber.repositories.responses.Media;

public class SliderCallbackAdapter implements SliderItemsAdapter.SliderCallback {
    @Override
    public void onThumbnailLoaded(final int position) {}

    @Override
    public void onItemClicked(final int position, final Media media, final View view) {}

    @Override
    public void onPlayerPlay(final int position) {}

    @Override
    public void onPlayerPause(final int position) {}

    @Override
    public void onPlayerRelease(final int position) {}

    @Override
    public void onFullScreenModeChanged(final boolean isFullScreen, final StyledPlayerView playerView) {}

    @Override
    public boolean isInFullScreen() {
        return false;
    }
}
