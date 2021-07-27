package awais.instagrabber.customviews.helpers;

import android.graphics.Point;
import android.graphics.Rect;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.exoplayer2.SimpleExoPlayer;

import awais.instagrabber.R;
import awais.instagrabber.adapters.viewholder.feed.FeedVideoViewHolder;
import awais.instagrabber.repositories.responses.Media;

import static androidx.recyclerview.widget.RecyclerView.SCROLL_STATE_DRAGGING;
import static androidx.recyclerview.widget.RecyclerView.SCROLL_STATE_IDLE;

public class VideoAwareRecyclerScroller extends RecyclerView.OnScrollListener {
    private static final String TAG = "VideoAwareRecScroll";
    private static final int FLING_JUMP_LOW_THRESHOLD = 80;
    private static final int FLING_JUMP_HIGH_THRESHOLD = 120;
    private static final Object LOCK = new Object();

    private LinearLayoutManager layoutManager;
    private boolean dragging;
    private boolean isLoadingPaused = false;
    private FeedVideoViewHolder currentlyPlayingViewHolder;

    @Override
    public void onScrollStateChanged(@NonNull final RecyclerView recyclerView, final int newState) {
        dragging = newState == SCROLL_STATE_DRAGGING;
        if (isLoadingPaused) {
            if (newState == SCROLL_STATE_DRAGGING || newState == SCROLL_STATE_IDLE) {
                // user is touchy or the scroll finished, show videos
                isLoadingPaused = false;
            } // settling means the user let the screen go, but it can still be flinging
        }
    }

    @Override
    public void onScrolled(@NonNull final RecyclerView recyclerView, final int dx, final int dy) {
        if (!dragging) {
            // TODO can be made better by a rolling average of last N calls to smooth out patterns like a,b,a
            int currentSpeed = Math.abs(dy);
            if (isLoadingPaused && currentSpeed < FLING_JUMP_LOW_THRESHOLD) {
                isLoadingPaused = false;
            } else if (!isLoadingPaused && FLING_JUMP_HIGH_THRESHOLD < currentSpeed) {
                isLoadingPaused = true;
                // stop playing video
            }
        }
        if (isLoadingPaused) return;
        if (layoutManager == null) {
            final RecyclerView.LayoutManager layoutManager = recyclerView.getLayoutManager();
            if (layoutManager instanceof LinearLayoutManager)
                this.layoutManager = (LinearLayoutManager) layoutManager;
        }
        if (layoutManager == null) {
            return;
        }
        int firstVisibleItemPos = layoutManager.findFirstCompletelyVisibleItemPosition();
        int lastVisibleItemPos = layoutManager.findLastCompletelyVisibleItemPosition();
        if (firstVisibleItemPos == -1 && lastVisibleItemPos == -1) {
            firstVisibleItemPos = layoutManager.findFirstVisibleItemPosition();
            lastVisibleItemPos = layoutManager.findLastVisibleItemPosition();
        }
        synchronized (LOCK) {
            final FeedVideoViewHolder videoHolder = getFirstVideoHolder(recyclerView, firstVisibleItemPos, lastVisibleItemPos);
            if (videoHolder == null || videoHolder.getCurrentFeedModel() == null) {
                if (currentlyPlayingViewHolder != null) {
                    // currentlyPlayingViewHolder.stopPlaying();
                    currentlyPlayingViewHolder = null;
                }
                return;
            }
            if (currentlyPlayingViewHolder != null && currentlyPlayingViewHolder.getCurrentFeedModel().getPk()
                                                                                .equals(videoHolder.getCurrentFeedModel().getPk())) {
                return;
            }
            if (currentlyPlayingViewHolder != null) {
                // currentlyPlayingViewHolder.stopPlaying();
            }
            // videoHolder.startPlaying();
            currentlyPlayingViewHolder = videoHolder;
        }
        // boolean processFirstItem = false, processLastItem = false;
        // View currView;
        // if (firstVisibleItemPos != -1) {
        //     currView = layoutManager.findViewByPosition(firstVisibleItemPos);
        //     if (currView != null && currView.getId() == R.id.videoHolder) {
        //         firstItemView = currView;
        //         // processFirstItem = true;
        //     }
        // }
        // if (lastVisibleItemPos != -1) {
        //     currView = layoutManager.findViewByPosition(lastVisibleItemPos);
        //     if (currView != null && currView.getId() == R.id.videoHolder) {
        //         lastItemView = currView;
        //         // processLastItem = true;
        //     }
        // }
        // if (firstItemView == null && lastItemView == null) {
        //     return;
        // }
        // if (firstItemView != null) {
        //
        //     Log.d(TAG, "view" + viewHolder);
        // }
        // if (lastItemView != null) {
        //     final FeedVideoViewHolder viewHolder = (FeedVideoViewHolder) recyclerView.getChildViewHolder(lastItemView);
        //     Log.d(TAG, "view" + viewHolder);
        // }
        // Log.d(TAG, firstItemView + " " + lastItemView);

        // final Rect visibleItemRect = new Rect();

        // int firstVisibleItemHeight = 0, lastVisibleItemHeight = 0;

        // final boolean isFirstItemVideoHolder = firstItemView != null && firstItemView.getId() == R.id.videoHolder;
        // if (isFirstItemVideoHolder) {
        //     firstItemView.getGlobalVisibleRect(visibleItemRect);
        //     firstVisibleItemHeight = visibleItemRect.height();
        // }
        // final boolean isLastItemVideoHolder = lastItemView != null && lastItemView.getId() == R.id.videoHolder;
        // if (isLastItemVideoHolder) {
        //     lastItemView.getGlobalVisibleRect(visibleItemRect);
        //     lastVisibleItemHeight = visibleItemRect.height();
        // }
        //
        // if (processFirstItem && firstVisibleItemHeight > lastVisibleItemHeight)
        //     videoPosShown = firstVisibleItemPos;
        // else if (processLastItem && lastVisibleItemHeight != 0) videoPosShown = lastVisibleItemPos;
        //
        // if (firstItemView != lastItemView) {
        //     final int mox = lastVisibleItemHeight - firstVisibleItemHeight;
        //     if (processLastItem && lastVisibleItemHeight > firstVisibleItemHeight)
        //         videoPosShown = lastVisibleItemPos;
        //     if ((processFirstItem || processLastItem) && mox >= 0)
        //         videoPosShown = lastVisibleItemPos;
        // }
        //
        // if (lastChangedVideoPos != -1 && lastVideoPos != -1) {
        //     currView = layoutManager.findViewByPosition(lastChangedVideoPos);
        //     if (currView != null && currView.getId() == R.id.videoHolder &&
        //             lastStoppedVideoPos != lastChangedVideoPos && lastPlayedVideoPos != lastChangedVideoPos) {
        //         lastStoppedVideoPos = lastChangedVideoPos;
        //         stopVideo(lastChangedVideoPos, recyclerView, currView);
        //     }
        //
        //     currView = layoutManager.findViewByPosition(lastVideoPos);
        //     if (currView != null && currView.getId() == R.id.videoHolder) {
        //         final Rect rect = new Rect();
        //         currView.getGlobalVisibleRect(rect);
        //
        //         final int holderTop = currView.getTop();
        //         final int holderHeight = currView.getBottom() - holderTop;
        //         final int halfHeight = holderHeight / 2;
        //         //halfHeight -= halfHeight / 5;
        //
        //         if (rect.height() < halfHeight) {
        //             if (lastStoppedVideoPos != lastVideoPos) {
        //                 lastStoppedVideoPos = lastVideoPos;
        //                 stopVideo(lastVideoPos, recyclerView, currView);
        //             }
        //         } else if (lastPlayedVideoPos != lastVideoPos) {
        //             lastPlayedVideoPos = lastVideoPos;
        //             playVideo(lastVideoPos, recyclerView, currView);
        //         }
        //     }
        //
        //     if (lastChangedVideoPos != lastVideoPos) lastChangedVideoPos = lastVideoPos;
        // }
        //
        // if (lastVideoPos != -1 && lastVideoPos != videoPosShown) {
        //     if (videoAttached) {
        //         //if ((currView = layoutManager.findViewByPosition(lastVideoPos)) != null && currView.getId() == R.id.videoHolder)
        //         releaseVideo(lastVideoPos, recyclerView, null);
        //         videoAttached = false;
        //     }
        // }
        // if (videoPosShown != -1) {
        //     lastVideoPos = videoPosShown;
        //     if (!videoAttached) {
        //         if ((currView = layoutManager.findViewByPosition(videoPosShown)) != null && currView.getId() == R.id.videoHolder)
        //             attachVideo(videoPosShown, recyclerView, currView);
        //         videoAttached = true;
        //     }
        // }
    }

    private FeedVideoViewHolder getFirstVideoHolder(final RecyclerView recyclerView, final int firstVisibleItemPos, final int lastVisibleItemPos) {
        final Rect visibleItemRect = new Rect();
        final Point offset = new Point();
        for (int pos = firstVisibleItemPos; pos <= lastVisibleItemPos; pos++) {
            final View view = layoutManager.findViewByPosition(pos);
            if (view != null && view.getId() == R.id.videoHolder) {
                final View viewSwitcher = view.findViewById(R.id.root);
                if (viewSwitcher == null) {
                    continue;
                }
                final boolean result = viewSwitcher.getGlobalVisibleRect(visibleItemRect, offset);
                if (!result) continue;
                final FeedVideoViewHolder viewHolder = (FeedVideoViewHolder) recyclerView.getChildViewHolder(view);
                final Media currentFeedModel = viewHolder.getCurrentFeedModel();
                visibleItemRect.offset(-offset.x, -offset.y);
                final int visibleHeight = visibleItemRect.height();
                if (visibleHeight < currentFeedModel.getOriginalHeight()) {
                    continue;
                }
                // Log.d(TAG, "post:" + currentFeedModel.getPostId() + ", visibleHeight: " + visibleHeight + ", post height: " + currentFeedModel.getImageHeight());
                return viewHolder;
            }
        }
        return null;
    }

    public void startPlaying() {
        if (currentlyPlayingViewHolder == null) {
            return;
        }
        // currentlyPlayingViewHolder.startPlaying();
    }

    public void stopPlaying() {
        if (currentlyPlayingViewHolder == null) {
            return;
        }
        // currentlyPlayingViewHolder.stopPlaying();
    }

    //     private synchronized void attachVideo(final int itemPos, final RecyclerView recyclerView, final View itemView) {
    //         synchronized (LOCK) {
    //             if (recyclerView != null) {
    //                 final RecyclerView.Adapter<?> adapter = recyclerView.getAdapter();
    //                 if (adapter instanceof FeedAdapter) {
    //                     final SimpleExoPlayer pagerPlayer = ((FeedAdapter) adapter).pagerPlayer;
    //                     if (pagerPlayer != null) pagerPlayer.setPlayWhenReady(false);
    //                 }
    //             }
    //             if (itemView == null) {
    //                 return;
    //             }
    //             final boolean shouldAutoplay = settingsHelper.getBoolean(Constants.AUTOPLAY_VIDEOS);
    //             final FeedModel feedModel = feedModels.get(itemPos);
    //             // loadVideo(itemPos, itemView, shouldAutoplay, feedModel);
    //         }
    //     }
    //
    //     private void loadVideo(final int itemPos, final View itemView, final boolean shouldAutoplay, final FeedModel feedModel) {
    //         final PlayerView playerView = itemView.findViewById(R.id.playerView);
    //         if (playerView == null) {
    //             return;
    //         }
    //         if (player != null) {
    //             player.stop(true);
    //             player.release();
    //             player = null;
    //         }
    //
    //         player = new SimpleExoPlayer.Builder(context)
    //                 .setUseLazyPreparation(!shouldAutoplay)
    //                 .build();
    //         player.setPlayWhenReady(shouldAutoplay);
    //
    //         final View btnComments = itemView.findViewById(R.id.btnComments);
    //         if (btnComments != null) {
    //             if (feedModel.getCommentsCount() <= 0) btnComments.setEnabled(false);
    //             else {
    //                 btnComments.setTag(feedModel);
    //                 btnComments.setEnabled(true);
    //                 btnComments.setOnClickListener(commentClickListener);
    //             }
    //         }
    //         playerView.setPlayer(player);
    //         btnMute = itemView.findViewById(R.id.btnMute);
    //         float vol = settingsHelper.getBoolean(Constants.MUTED_VIDEOS) ? 0f : 1f;
    //         if (vol == 0f && Utils.sessionVolumeFull) vol = 1f;
    //         player.setVolume(vol);
    //
    //         if (btnMute != null) {
    //             btnMute.setVisibility(View.VISIBLE);
    //             btnMute.setImageResource(vol == 0f ? R.drawable.vol : R.drawable.mute);
    //             btnMute.setOnClickListener(muteClickListener);
    //         }
    //         final DataSource.Factory factory = cacheDataSourceFactory != null ? cacheDataSourceFactory : dataSourceFactory;
    //         final ProgressiveMediaSource.Factory sourceFactory = new ProgressiveMediaSource.Factory(factory);
    //         final ProgressiveMediaSource mediaSource = sourceFactory.createMediaSource(Uri.parse(feedModel.getDisplayUrl()));
    //
    //         player.setRepeatMode(Player.REPEAT_MODE_ALL);
    //         player.prepare(mediaSource);
    //         player.setVolume(vol);
    //
    //         playerView.setOnClickListener(v -> player.setPlayWhenReady(!player.getPlayWhenReady()));
    //
    //         if (videoChangeCallback != null) videoChangeCallback.playerChanged(itemPos, player);
    //     }
    //
    //     private void releaseVideo(final int itemPos, final RecyclerView recyclerView, final View itemView) {
    // //                    Log.d("AWAISKING_APP", "release: " + itemPos);
    // //         if (player != null) {
    // //             player.stop(true);
    // //             player.release();
    // //         }
    // //         player = null;
    //     }
    //
    //     private void playVideo(final int itemPos, final RecyclerView recyclerView, final View itemView) {
    // //                    if (player != null) {
    // //                        final int playbackState = player.getPlaybackState();
    // //                        if (!player.isPlaying()
    // //                               || playbackState == Player.STATE_READY || playbackState == Player.STATE_ENDED
    // //                        ) {
    // //                            player.setPlayWhenReady(true);
    // //                        }
    // //                    }
    // //                    if (player != null) {
    // //                        player.setPlayWhenReady(true);
    // //                        player.getPlaybackState();
    // //                    }
    //     }
    //
    //     private void stopVideo(final int itemPos, final RecyclerView recyclerView, final View itemView) {
    //         if (player != null) {
    //             player.setPlayWhenReady(false);
    //             player.getPlaybackState();
    //         }
    //     }

    public interface VideoChangeCallback {
        void playerChanged(final int itemPos, final SimpleExoPlayer player);
    }
}