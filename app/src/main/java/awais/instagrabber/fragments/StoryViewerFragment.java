package awais.instagrabber.fragments;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.drawable.Animatable;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.view.GestureDetectorCompat;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.NavDirections;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.drawee.controller.BaseControllerListener;
import com.facebook.drawee.interfaces.DraweeController;
import com.facebook.imagepipeline.image.ImageInfo;
import com.facebook.imagepipeline.request.ImageRequest;
import com.facebook.imagepipeline.request.ImageRequestBuilder;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.SimpleExoPlayer;
import com.google.android.exoplayer2.source.LoadEventInfo;
import com.google.android.exoplayer2.source.MediaLoadData;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.MediaSourceEventListener;
import com.google.android.exoplayer2.source.ProgressiveMediaSource;
import com.google.android.exoplayer2.source.dash.DashMediaSource;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.R;
import awais.instagrabber.adapters.StoriesAdapter;
import awais.instagrabber.customviews.helpers.SwipeGestureListener;
import awais.instagrabber.databinding.FragmentStoryViewerBinding;
import awais.instagrabber.fragments.main.ProfileFragmentDirections;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.interfaces.SwipeEvent;
import awais.instagrabber.models.FeedStoryModel;
import awais.instagrabber.models.HighlightModel;
import awais.instagrabber.models.StoryModel;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.models.stickers.PollModel;
import awais.instagrabber.models.stickers.QuestionModel;
import awais.instagrabber.models.stickers.QuizModel;
import awais.instagrabber.models.stickers.SliderModel;
import awais.instagrabber.models.stickers.SwipeUpModel;
import awais.instagrabber.repositories.requests.StoryViewerOptions;
import awais.instagrabber.repositories.requests.StoryViewerOptions.Type;
import awais.instagrabber.repositories.requests.directmessages.BroadcastOptions;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.StoryStickerResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadBroadcastResponse;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.DownloadUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.ArchivesViewModel;
import awais.instagrabber.viewmodels.FeedStoriesViewModel;
import awais.instagrabber.viewmodels.HighlightsViewModel;
import awais.instagrabber.viewmodels.StoriesViewModel;
import awais.instagrabber.webservices.DirectMessagesService;
import awais.instagrabber.webservices.MediaService;
import awais.instagrabber.webservices.ServiceCallback;
import awais.instagrabber.webservices.StoriesService;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import static awais.instagrabber.customviews.helpers.SwipeGestureListener.SWIPE_THRESHOLD;
import static awais.instagrabber.customviews.helpers.SwipeGestureListener.SWIPE_VELOCITY_THRESHOLD;
import static awais.instagrabber.fragments.settings.PreferenceKeys.MARK_AS_SEEN;
import static awais.instagrabber.utils.Utils.settingsHelper;

public class StoryViewerFragment extends Fragment {
    private static final String TAG = "StoryViewerFragment";

    private AppCompatActivity fragmentActivity;
    private View root;
    private FragmentStoryViewerBinding binding;
    private String currentStoryUsername;
    private String highlightTitle;
    private StoriesAdapter storiesAdapter;
    private SwipeEvent swipeEvent;
    private GestureDetectorCompat gestureDetector;
    private StoriesService storiesService;
    private MediaService mediaService;
    private StoryModel currentStory;
    private int slidePos;
    private int lastSlidePos;
    private String url;
    private PollModel poll;
    private QuestionModel question;
    private String[] mentions;
    private QuizModel quiz;
    private SliderModel slider;
    private MenuItem menuDownload, menuDm, menuProfile;
    private SimpleExoPlayer player;
    // private boolean isHashtag;
    // private boolean isLoc;
    // private String highlight;
    private String actionBarTitle, actionBarSubtitle;
    private boolean fetching = false, sticking = false, shouldRefresh = true;
    private boolean downloadVisible = false, dmVisible = false, profileVisible = true;
    private int currentFeedStoryIndex;
    private double sliderValue;
    private StoriesViewModel storiesViewModel;
    private ViewModel viewModel;
    // private boolean isHighlight;
    // private boolean isArchive;
    // private boolean isNotification;
    private DirectMessagesService directMessagesService;

    private final String cookie = settingsHelper.getString(Constants.COOKIE);
    private StoryViewerOptions options;

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        if (csrfToken == null) return;
        final long userIdFromCookie = CookieUtils.getUserIdFromCookie(cookie);
        final String deviceId = settingsHelper.getString(Constants.DEVICE_UUID);
        fragmentActivity = (AppCompatActivity) requireActivity();
        storiesService = StoriesService.getInstance(csrfToken, userIdFromCookie, deviceId);
        mediaService = MediaService.getInstance(null, null, 0);
        directMessagesService = DirectMessagesService.getInstance(csrfToken, userIdFromCookie, deviceId);
        setHasOptionsMenu(true);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentStoryViewerBinding.inflate(inflater, container, false);
        root = binding.getRoot();
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (!shouldRefresh) return;
        init();
        shouldRefresh = false;
    }

    @Override
    public void onCreateOptionsMenu(@NonNull final Menu menu, final MenuInflater menuInflater) {
        menuInflater.inflate(R.menu.story_menu, menu);
        menuDownload = menu.findItem(R.id.action_download);
        menuDm = menu.findItem(R.id.action_dms);
        menuProfile = menu.findItem(R.id.action_profile);
        menuDownload.setVisible(downloadVisible);
        menuDm.setVisible(dmVisible);
        menuProfile.setVisible(profileVisible);
    }

    @Override
    public void onPrepareOptionsMenu(@NonNull final Menu menu) {
        // hide menu items from activity
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        final Context context = getContext();
        if (context == null) return false;
        int itemId = item.getItemId();
        if (itemId == R.id.action_download) {
            if (ContextCompat.checkSelfPermission(context, DownloadUtils.PERMS[0]) == PackageManager.PERMISSION_GRANTED)
                downloadStory();
            else
                ActivityCompat.requestPermissions(requireActivity(), DownloadUtils.PERMS, 8020);
            return true;
        }
        if (itemId == R.id.action_dms) {
            final EditText input = new EditText(context);
            input.setHint(R.string.reply_hint);
            new AlertDialog.Builder(context)
                    .setTitle(R.string.reply_story)
                    .setView(input)
                    .setPositiveButton(R.string.confirm, (d, w) -> {
                        final Call<DirectThread> createThreadRequest =
                                directMessagesService.createThread(Collections.singletonList(currentStory.getUserId()), null);
                        createThreadRequest.enqueue(new Callback<DirectThread>() {
                            @Override
                            public void onResponse(@NonNull final Call<DirectThread> call, @NonNull final Response<DirectThread> response) {
                                if (!response.isSuccessful() || response.body() == null) {
                                    Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                    return;
                                }
                                final DirectThread thread = response.body();
                                try {
                                    final Call<DirectThreadBroadcastResponse> request = directMessagesService
                                            .broadcastStoryReply(BroadcastOptions.ThreadIdOrUserIds.of(thread.getThreadId()),
                                                    input.getText().toString(),
                                                    currentStory.getStoryMediaId(),
                                                    String.valueOf(currentStory.getUserId()));
                                    request.enqueue(new Callback<DirectThreadBroadcastResponse>() {
                                        @Override
                                        public void onResponse(@NonNull final Call<DirectThreadBroadcastResponse> call,
                                                               @NonNull final Response<DirectThreadBroadcastResponse> response) {
                                            if (!response.isSuccessful()) {
                                                Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                                return;
                                            }
                                            Toast.makeText(context, R.string.answered_story, Toast.LENGTH_SHORT).show();
                                        }

                                        @Override
                                        public void onFailure(@NonNull final Call<DirectThreadBroadcastResponse> call, @NonNull final Throwable t) {
                                            try {
                                                Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                                Log.e(TAG, "onFailure: ", t);
                                            } catch (Throwable ignored) {
                                            }
                                        }
                                    });
                                } catch (UnsupportedEncodingException e) {
                                    Log.e(TAG, "Error", e);
                                }
                            }

                            @Override
                            public void onFailure(@NonNull final Call<DirectThread> call, @NonNull final Throwable t) {
                                Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                            }
                        });
                    })
                    .setNegativeButton(R.string.cancel, null)
                    .show();
            return true;
        }
        if (itemId == R.id.action_profile) {
            openProfile("@" + currentStory.getUsername());
        }
        return false;
    }

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 8020 && grantResults[0] == PackageManager.PERMISSION_GRANTED)
            downloadStory();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (player != null) {
            player.pause();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        final ActionBar actionBar = fragmentActivity.getSupportActionBar();
        if (actionBar != null) {
            actionBar.setTitle(actionBarTitle);
            actionBar.setSubtitle(actionBarSubtitle);
        }
        setHasOptionsMenu(true);
    }

    @Override
    public void onDestroy() {
        releasePlayer();
        // reset subtitle
        final ActionBar actionBar = fragmentActivity.getSupportActionBar();
        if (actionBar != null) {
            actionBar.setSubtitle(null);
        }
        super.onDestroy();
    }

    private void init() {
        if (getArguments() == null) return;
        final StoryViewerFragmentArgs fragmentArgs = StoryViewerFragmentArgs.fromBundle(getArguments());
        options = fragmentArgs.getOptions();
        currentFeedStoryIndex = options.getCurrentFeedStoryIndex();
        // highlight = fragmentArgs.getHighlight();
        // isHighlight = !TextUtils.isEmpty(highlight);
        // isArchive = fragmentArgs.getIsArchive();
        // isNotification = fragmentArgs.getIsNotification();
        final Type type = options.getType();
        if (currentFeedStoryIndex >= 0) {
            switch (type) {
                case HIGHLIGHT:
                    viewModel = new ViewModelProvider(fragmentActivity).get(HighlightsViewModel.class);
                    break;
                case STORY_ARCHIVE:
                    viewModel = new ViewModelProvider(fragmentActivity).get(ArchivesViewModel.class);
                    break;
                default:
                case FEED_STORY_POSITION:
                    viewModel = new ViewModelProvider(fragmentActivity).get(FeedStoriesViewModel.class);
                    break;
            }
        }
        setupStories();
    }

    private void setupStories() {
        storiesViewModel = new ViewModelProvider(this).get(StoriesViewModel.class);
        setupListeners();
        final Context context = getContext();
        if (context == null) return;
        binding.storiesList.setLayoutManager(new LinearLayoutManager(context, LinearLayoutManager.HORIZONTAL, false));
        storiesAdapter = new StoriesAdapter((model, position) -> {
            currentStory = model;
            slidePos = position;
            refreshStory();
        });
        binding.storiesList.setAdapter(storiesAdapter);
        storiesViewModel.getList().observe(fragmentActivity, storiesAdapter::submitList);
        resetView();
    }

    @SuppressLint("ClickableViewAccessibility")
    private void setupListeners() {
        final boolean hasFeedStories;
        List<?> models = null;
        if (currentFeedStoryIndex >= 0) {
            final Type type = options.getType();
            switch (type) {
                case HIGHLIGHT:
                    final HighlightsViewModel highlightsViewModel = (HighlightsViewModel) viewModel;
                    models = highlightsViewModel.getList().getValue();
                    break;
                case FEED_STORY_POSITION:
                    final FeedStoriesViewModel feedStoriesViewModel = (FeedStoriesViewModel) viewModel;
                    models = feedStoriesViewModel.getList().getValue();
                    break;
                case STORY_ARCHIVE:
                    final ArchivesViewModel archivesViewModel = (ArchivesViewModel) viewModel;
                    models = archivesViewModel.getList().getValue();
                    break;
            }
        }
        hasFeedStories = models != null && !models.isEmpty();
        final List<?> finalModels = models;
        final Context context = getContext();
        if (context == null) return;
        swipeEvent = isRightSwipe -> {
            final List<StoryModel> storyModels = storiesViewModel.getList().getValue();
            final int storiesLen = storyModels == null ? 0 : storyModels.size();
            if (sticking) {
                Toast.makeText(context, R.string.follower_wait_to_load, Toast.LENGTH_SHORT).show();
                return;
            }
            if (storiesLen <= 0) return;
            final boolean isLeftSwipe = !isRightSwipe;
            final boolean endOfCurrentStories = slidePos + 1 >= storiesLen;
            final boolean swipingBeyondCurrentStories = (endOfCurrentStories && isLeftSwipe) || (slidePos == 0 && isRightSwipe);
            if (swipingBeyondCurrentStories && hasFeedStories) {
                final int index = currentFeedStoryIndex;
                if ((isRightSwipe && index == 0) || (isLeftSwipe && index == finalModels.size() - 1)) {
                    Toast.makeText(context, R.string.no_more_stories, Toast.LENGTH_SHORT).show();
                    return;
                }
                final Object feedStoryModel = isRightSwipe
                                              ? finalModels.get(index - 1)
                                              : finalModels.size() == index + 1 ? null : finalModels.get(index + 1);
                paginateStories(feedStoryModel, finalModels.get(index), context, isRightSwipe, currentFeedStoryIndex == finalModels.size() - 2);
                return;
            }
            if (isRightSwipe) {
                if (--slidePos <= 0) {
                    slidePos = 0;
                }
            } else if (++slidePos >= storiesLen) {
                slidePos = storiesLen - 1;
            }
            currentStory = storyModels.get(slidePos);
            refreshStory();
        };
        gestureDetector = new GestureDetectorCompat(context, new SwipeGestureListener(swipeEvent));
        binding.playerView.setOnTouchListener((v, event) -> gestureDetector.onTouchEvent(event));
        final GestureDetector.SimpleOnGestureListener simpleOnGestureListener = new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onFling(final MotionEvent e1, final MotionEvent e2, final float velocityX, final float velocityY) {
                final float diffX = e2.getX() - e1.getX();
                try {
                    if (Math.abs(diffX) > Math.abs(e2.getY() - e1.getY()) && Math.abs(diffX) > SWIPE_THRESHOLD
                            && Math.abs(velocityX) > SWIPE_VELOCITY_THRESHOLD) {
                        swipeEvent.onSwipe(diffX > 0);
                        return true;
                    }
                } catch (final Exception e) {
                    // if (logCollector != null)
                    //     logCollector.appendException(e, LogCollector.LogFile.ACTIVITY_STORY_VIEWER, "setupListeners",
                    //                                  new Pair<>("swipeEvent", swipeEvent),
                    //                                  new Pair<>("diffX", diffX));
                    if (BuildConfig.DEBUG) Log.e(TAG, "Error", e);
                }
                return false;
            }
        };

        if (hasFeedStories) {
            binding.btnBackward.setVisibility(currentFeedStoryIndex == 0 ? View.INVISIBLE : View.VISIBLE);
            binding.btnForward.setVisibility(currentFeedStoryIndex == finalModels.size() - 1 ? View.INVISIBLE : View.VISIBLE);
            binding.btnBackward.setOnClickListener(v -> paginateStories(finalModels.get(currentFeedStoryIndex - 1),
                                                                        finalModels.get(currentFeedStoryIndex),
                                                                        context, true, false));
            binding.btnForward.setOnClickListener(v -> paginateStories(finalModels.get(currentFeedStoryIndex + 1),
                                                                       finalModels.get(currentFeedStoryIndex),
                                                                       context, false,
                                                                       currentFeedStoryIndex == finalModels.size() - 2));
        }

        binding.imageViewer.setTapListener(simpleOnGestureListener);
        binding.spotify.setOnClickListener(v -> {
            final Object tag = v.getTag();
            if (tag instanceof CharSequence) {
                Utils.openURL(context, tag.toString());
            }
        });
        binding.swipeUp.setOnClickListener(v -> {
            final Object tag = v.getTag();
            if (tag instanceof CharSequence) {
                new AlertDialog.Builder(context)
                        .setTitle(R.string.swipe_up_confirmation)
                        .setMessage(tag.toString()).setPositiveButton(R.string.yes, (d, w) -> Utils.openURL(context, tag.toString()))
                        .setNegativeButton(R.string.no, (d, w) -> d.dismiss()).show();
            }
        });
        binding.viewStoryPost.setOnClickListener(v -> {
            final Object tag = v.getTag();
            if (!(tag instanceof CharSequence)) return;
            final String mediaId = tag.toString();
            final AlertDialog alertDialog = new AlertDialog.Builder(context)
                    .setCancelable(false)
                    .setView(R.layout.dialog_opening_post)
                    .create();
            alertDialog.show();
            mediaService.fetch(Long.parseLong(mediaId), new ServiceCallback<Media>() {
                @Override
                public void onSuccess(final Media feedModel) {
                    final NavController navController = NavHostFragment.findNavController(StoryViewerFragment.this);
                    final Bundle bundle = new Bundle();
                    bundle.putSerializable(PostViewV2Fragment.ARG_MEDIA, feedModel);
                    try {
                        navController.navigate(R.id.action_global_post_view, bundle);
                        alertDialog.dismiss();
                    } catch (Exception e) {
                        Log.e(TAG, "openPostDialog: ", e);
                    }
                }

                @Override
                public void onFailure(final Throwable t) {
                    alertDialog.dismiss();
                    Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                }
            });
        });
        final View.OnClickListener storyActionListener = v -> {
            final Object tag = v.getTag();
            if (tag instanceof PollModel) {
                poll = (PollModel) tag;
                if (poll.getMyChoice() > -1) {
                    new AlertDialog.Builder(context)
                            .setTitle(R.string.voted_story_poll)
                            .setAdapter(new ArrayAdapter<>(
                                                context,
                                                android.R.layout.simple_list_item_1,
                                                new String[]{
                                                        (poll.getMyChoice() == 0 ? "√ " : "") + poll.getLeftChoice() + " (" + poll.getLeftCount() + ")",
                                                        (poll.getMyChoice() == 1 ? "√ " : "") + poll.getRightChoice() + " (" + poll.getRightCount() + ")"
                                                }),
                                        null)
                            .setPositiveButton(R.string.ok, null)
                            .show();
                } else {
                    new AlertDialog.Builder(context)
                            .setTitle(poll.getQuestion())
                            .setAdapter(new ArrayAdapter<>(context, android.R.layout.simple_list_item_1, new String[]{
                                    poll.getLeftChoice() + " (" + poll.getLeftCount() + ")",
                                    poll.getRightChoice() + " (" + poll.getRightCount() + ")"
                            }), (d, w) -> {
                                sticking = true;
                                storiesService.respondToPoll(
                                        currentStory.getStoryMediaId().split("_")[0],
                                        poll.getId(),
                                        w,
                                        new ServiceCallback<StoryStickerResponse>() {
                                            @Override
                                            public void onSuccess(final StoryStickerResponse result) {
                                                sticking = false;
                                                try {
                                                    poll.setMyChoice(w);
                                                    Toast.makeText(context, R.string.votef_story_poll, Toast.LENGTH_SHORT).show();
                                                } catch (Exception ignored) {}
                                            }

                                            @Override
                                            public void onFailure(final Throwable t) {
                                                sticking = false;
                                                Log.e(TAG, "Error responding", t);
                                                try {
                                                    Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                                } catch (Exception ignored) {}
                                            }
                                        });
                            })
                            .setPositiveButton(R.string.cancel, null)
                            .show();
                }
            } else if (tag instanceof QuestionModel) {
                question = (QuestionModel) tag;
                final EditText input = new EditText(context);
                input.setHint(R.string.answer_hint);
                new AlertDialog.Builder(context)
                        .setTitle(question.getQuestion())
                        .setView(input)
                        .setPositiveButton(R.string.confirm, (d, w) -> {
                            sticking = true;
                            storiesService.respondToQuestion(
                                    currentStory.getStoryMediaId().split("_")[0],
                                    question.getId(),
                                    input.getText().toString(),
                                    new ServiceCallback<StoryStickerResponse>() {
                                        @Override
                                        public void onSuccess(final StoryStickerResponse result) {
                                            sticking = false;
                                            try {
                                                Toast.makeText(context, R.string.answered_story, Toast.LENGTH_SHORT).show();
                                            } catch (Exception ignored) {}
                                        }

                                        @Override
                                        public void onFailure(final Throwable t) {
                                            sticking = false;
                                            Log.e(TAG, "Error responding", t);
                                            try {
                                                Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                            } catch (Exception ignored) {}
                                        }
                                    });
                        })
                        .setNegativeButton(R.string.cancel, null)
                        .show();
            } else if (tag instanceof String[]) {
                mentions = (String[]) tag;
                new AlertDialog.Builder(context)
                        .setTitle(R.string.story_mentions)
                        .setAdapter(new ArrayAdapter<>(context, android.R.layout.simple_list_item_1, mentions), (d, w) -> openProfile(mentions[w]))
                        .setPositiveButton(R.string.cancel, null)
                        .show();
            } else if (tag instanceof QuizModel) {
                String[] choices = new String[quiz.getChoices().length];
                for (int q = 0; q < choices.length; ++q) {
                    choices[q] = (quiz.getMyChoice() == q ? "√ " : "") + quiz.getChoices()[q] + " (" + quiz.getCounts()[q] + ")";
                }
                new AlertDialog.Builder(context)
                        .setTitle(quiz.getMyChoice() > -1 ? getString(R.string.story_quizzed) : quiz.getQuestion())
                        .setAdapter(new ArrayAdapter<>(context, android.R.layout.simple_list_item_1, choices), (d, w) -> {
                            if (quiz.getMyChoice() == -1) {
                                sticking = true;
                                storiesService.respondToQuiz(
                                        currentStory.getStoryMediaId().split("_")[0],
                                        quiz.getId(),
                                        w,
                                        new ServiceCallback<StoryStickerResponse>() {
                                            @Override
                                            public void onSuccess(final StoryStickerResponse result) {
                                                sticking = false;
                                                try {
                                                    quiz.setMyChoice(w);
                                                    Toast.makeText(context, R.string.answered_story, Toast.LENGTH_SHORT).show();
                                                } catch (Exception ignored) {}
                                            }

                                            @Override
                                            public void onFailure(final Throwable t) {
                                                sticking = false;
                                                Log.e(TAG, "Error responding", t);
                                                try {
                                                    Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                                } catch (Exception ignored) {}
                                            }
                                        });
                            }
                        })
                        .setPositiveButton(R.string.cancel, null)
                        .show();
            } else if (tag instanceof SliderModel) {
                slider = (SliderModel) tag;
                NumberFormat percentage = NumberFormat.getPercentInstance();
                percentage.setMaximumFractionDigits(2);
                LinearLayout sliderView = new LinearLayout(context);
                sliderView.setLayoutParams(new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT));
                sliderView.setOrientation(LinearLayout.VERTICAL);
                TextView tv = new TextView(context);
                tv.setGravity(Gravity.CENTER_HORIZONTAL);
                final SeekBar input = new SeekBar(context);
                double avg = slider.getAverage() * 100;
                input.setProgress((int) avg);
                sliderView.addView(input);
                sliderView.addView(tv);
                if (slider.getMyChoice().isNaN() && slider.canVote()) {
                    input.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                        @Override
                        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                            sliderValue = progress / 100.0;
                            tv.setText(percentage.format(sliderValue));
                        }

                        @Override
                        public void onStartTrackingTouch(SeekBar seekBar) {
                        }

                        @Override
                        public void onStopTrackingTouch(SeekBar seekBar) {
                        }
                    });
                    new AlertDialog.Builder(context)
                            .setTitle(TextUtils.isEmpty(slider.getQuestion()) ? slider.getEmoji() : slider.getQuestion())
                            .setMessage(getResources().getQuantityString(R.plurals.slider_info,
                                                                         slider.getVoteCount(),
                                                                         slider.getVoteCount(),
                                                                         percentage.format(slider.getAverage())))
                            .setView(sliderView)
                            .setPositiveButton(R.string.confirm, (d, w) -> {
                                sticking = true;
                                storiesService.respondToSlider(
                                        currentStory.getStoryMediaId().split("_")[0],
                                        slider.getId(),
                                        sliderValue,
                                        new ServiceCallback<StoryStickerResponse>() {
                                            @Override
                                            public void onSuccess(final StoryStickerResponse result) {
                                                sticking = false;
                                                try {
                                                    slider.setMyChoice(sliderValue);
                                                    Toast.makeText(context, R.string.answered_story, Toast.LENGTH_SHORT).show();
                                                } catch (Exception ignored) {}
                                            }

                                            @Override
                                            public void onFailure(final Throwable t) {
                                                sticking = false;
                                                Log.e(TAG, "Error responding", t);
                                                try {
                                                    Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                                } catch (Exception ignored) {}
                                            }
                                        });
                            })
                            .setNegativeButton(R.string.cancel, null)
                            .show();
                } else {
                    input.setEnabled(false);
                    tv.setText(getString(R.string.slider_answer, percentage.format(slider.getMyChoice())));
                    new AlertDialog.Builder(context)
                            .setTitle(TextUtils.isEmpty(slider.getQuestion()) ? slider.getEmoji() : slider.getQuestion())
                            .setMessage(getResources().getQuantityString(R.plurals.slider_info,
                                                                         slider.getVoteCount(),
                                                                         slider.getVoteCount(),
                                                                         percentage.format(slider.getAverage())))
                            .setView(sliderView)
                            .setPositiveButton(R.string.ok, null)
                            .show();
                }
            }
        };
        binding.poll.setOnClickListener(storyActionListener);
        binding.answer.setOnClickListener(storyActionListener);
        binding.mention.setOnClickListener(storyActionListener);
        binding.quiz.setOnClickListener(storyActionListener);
        binding.slider.setOnClickListener(storyActionListener);
    }

    private void resetView() {
        final Context context = getContext();
        if (context == null) return;
        StoryModel live = null;
        slidePos = 0;
        lastSlidePos = 0;
        if (menuDownload != null) menuDownload.setVisible(false);
        if (menuDm != null) menuDm.setVisible(false);
        if (menuProfile != null) menuProfile.setVisible(false);
        downloadVisible = false;
        dmVisible = false;
        profileVisible = false;
        binding.imageViewer.setController(null);
        releasePlayer();
        String currentStoryMediaId = null;
        final Type type = options.getType();
        StoryViewerOptions fetchOptions = null;
        switch (type) {
            case HIGHLIGHT: {
                final HighlightsViewModel highlightsViewModel = (HighlightsViewModel) viewModel;
                final List<HighlightModel> models = highlightsViewModel.getList().getValue();
                if (models == null || models.isEmpty() || currentFeedStoryIndex >= models.size() || currentFeedStoryIndex < 0) {
                    Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                    return;
                }
                final HighlightModel model = models.get(currentFeedStoryIndex);
                currentStoryMediaId = model.getId();
                fetchOptions = StoryViewerOptions.forHighlight(model.getId());
                highlightTitle = model.getTitle();
                break;
            }
            case FEED_STORY_POSITION: {
                final FeedStoriesViewModel feedStoriesViewModel = (FeedStoriesViewModel) viewModel;
                final List<FeedStoryModel> models = feedStoriesViewModel.getList().getValue();
                if (models == null || currentFeedStoryIndex >= models.size() || currentFeedStoryIndex < 0) return;
                final FeedStoryModel model = models.get(currentFeedStoryIndex);
                currentStoryMediaId = model.getStoryMediaId();
                currentStoryUsername = model.getProfileModel().getUsername();
                fetchOptions = StoryViewerOptions.forUser(Long.parseLong(currentStoryMediaId), currentStoryUsername);
                if (model.isLive()) {
                    live = model.getFirstStoryModel();
                }
                break;
            }
            case STORY_ARCHIVE: {
                final ArchivesViewModel archivesViewModel = (ArchivesViewModel) viewModel;
                final List<HighlightModel> models = archivesViewModel.getList().getValue();
                if (models == null || models.isEmpty() || currentFeedStoryIndex >= models.size() || currentFeedStoryIndex < 0) {
                    Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                    return;
                }
                final HighlightModel model = models.get(currentFeedStoryIndex);
                currentStoryMediaId = parseStoryMediaId(model.getId());
                currentStoryUsername = model.getTitle();
                fetchOptions = StoryViewerOptions.forStoryArchive(model.getId());
                break;
            }
        }
        if (type == Type.USER) {
            currentStoryMediaId = String.valueOf(options.getId());
            currentStoryUsername = options.getName();
            fetchOptions = StoryViewerOptions.forUser(options.getId(), currentStoryUsername);
        }
        setTitle(type);
        storiesViewModel.getList().setValue(Collections.emptyList());
        if (type == Type.STORY) {
            storiesService.fetch(options.getId(), new ServiceCallback<StoryModel>() {
                @Override
                public void onSuccess(final StoryModel storyModel) {
                    fetching = false;
                    binding.storiesList.setVisibility(View.GONE);
                    if (storyModel == null) {
                        storiesViewModel.getList().setValue(Collections.emptyList());
                        currentStory = null;
                        return;
                    }
                    storiesViewModel.getList().setValue(Collections.singletonList(storyModel));
                    currentStory = storyModel;
                    refreshStory();
                }

                @Override
                public void onFailure(final Throwable t) {
                    Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                    Log.e(TAG, "Error", t);
                }
            });
            return;
        }
        if (currentStoryMediaId == null) return;
        final ServiceCallback<List<StoryModel>> storyCallback = new ServiceCallback<List<StoryModel>>() {
            @Override
            public void onSuccess(final List<StoryModel> storyModels) {
                fetching = false;
                if (storyModels == null || storyModels.isEmpty()) {
                    storiesViewModel.getList().setValue(Collections.emptyList());
                    currentStory = null;
                    binding.storiesList.setVisibility(View.GONE);
                    return;
                }
                binding.storiesList.setVisibility((storyModels.size() == 1 && currentFeedStoryIndex == -1) ? View.GONE : View.VISIBLE);
                if (currentFeedStoryIndex == -1) {
                    binding.btnBackward.setVisibility(View.GONE);
                    binding.btnForward.setVisibility(View.GONE);
                }
                storiesViewModel.getList().setValue(storyModels);
                currentStory = storyModels.get(0);
                refreshStory();
            }

            @Override
            public void onFailure(final Throwable t) {
                Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                Log.e(TAG, "Error", t);
            }
        };
        if (live != null) {
            storyCallback.onSuccess(Collections.singletonList(live));
            return;
        }
        storiesService.getUserStory(fetchOptions, storyCallback);
    }

    private void setTitle(final Type type) {
        final boolean hasUsername = !TextUtils.isEmpty(currentStoryUsername);
        if (type == Type.HIGHLIGHT) {
            final ActionBar actionBar = fragmentActivity.getSupportActionBar();
            if (actionBar != null) {
                actionBarTitle = highlightTitle;
                actionBar.setTitle(highlightTitle);
            }
        } else if (hasUsername) {
            currentStoryUsername = currentStoryUsername.replace("@", "");
            final ActionBar actionBar = fragmentActivity.getSupportActionBar();
            if (actionBar != null) {
                actionBarTitle = currentStoryUsername;
                actionBar.setTitle(currentStoryUsername);
            }
        }
    }

    private synchronized void refreshStory() {
        if (binding.storiesList.getVisibility() == View.VISIBLE) {
            final List<StoryModel> storyModels = storiesViewModel.getList().getValue();
            if (storyModels != null && storyModels.size() > 0) {
                StoryModel item = storyModels.get(lastSlidePos);
                if (item != null) {
                    item.setCurrentSlide(false);
                    storiesAdapter.notifyItemChanged(lastSlidePos, item);
                }
                item = storyModels.get(slidePos);
                if (item != null) {
                    item.setCurrentSlide(true);
                    storiesAdapter.notifyItemChanged(slidePos, item);
                }
            }
        }
        lastSlidePos = slidePos;

        final MediaItemType itemType = currentStory.getItemType();

        url = itemType == MediaItemType.MEDIA_TYPE_IMAGE ? currentStory.getStoryUrl() : currentStory.getVideoUrl();

        if (itemType != MediaItemType.MEDIA_TYPE_LIVE) {
            final String shortCode = currentStory.getTappableShortCode();
            binding.viewStoryPost.setVisibility(shortCode != null ? View.VISIBLE : View.GONE);
            binding.viewStoryPost.setTag(shortCode);

            final String spotify = currentStory.getSpotify();
            binding.spotify.setVisibility(spotify != null ? View.VISIBLE : View.GONE);
            binding.spotify.setTag(spotify);

            poll = currentStory.getPoll();
            binding.poll.setVisibility(poll != null ? View.VISIBLE : View.GONE);
            binding.poll.setTag(poll);

            question = currentStory.getQuestion();
            binding.answer.setVisibility((question != null) ? View.VISIBLE : View.GONE);
            binding.answer.setTag(question);

            mentions = currentStory.getMentions();
            binding.mention.setVisibility((mentions != null && mentions.length > 0) ? View.VISIBLE : View.GONE);
            binding.mention.setTag(mentions);

            quiz = currentStory.getQuiz();
            binding.quiz.setVisibility(quiz != null ? View.VISIBLE : View.GONE);
            binding.quiz.setTag(quiz);

            slider = currentStory.getSlider();
            binding.slider.setVisibility(slider != null ? View.VISIBLE : View.GONE);
            binding.slider.setTag(slider);

            final SwipeUpModel swipeUp = currentStory.getSwipeUp();
            if (swipeUp != null) {
                binding.swipeUp.setVisibility(View.VISIBLE);
                binding.swipeUp.setText(swipeUp.getText());
                binding.swipeUp.setTag(swipeUp.getUrl());
            } else binding.swipeUp.setVisibility(View.GONE);
        }

        releasePlayer();
        final Type type = options.getType();
        if (type == Type.HASHTAG || type == Type.LOCATION) {
            final ActionBar actionBar = fragmentActivity.getSupportActionBar();
            if (actionBar != null) {
                actionBarTitle = currentStory.getUsername();
                actionBar.setTitle(currentStory.getUsername());
            }
        }
        if (itemType == MediaItemType.MEDIA_TYPE_VIDEO) setupVideo();
        else if (itemType == MediaItemType.MEDIA_TYPE_LIVE) setupLive();
        else setupImage();

        final ActionBar actionBar = fragmentActivity.getSupportActionBar();
        actionBarSubtitle = Utils.datetimeParser.format(new Date(currentStory.getTimestamp() * 1000L));
        if (actionBar != null) {
            try {
                actionBar.setSubtitle(actionBarSubtitle);
            } catch (Exception e) {
                Log.e(TAG, "refreshStory: ", e);
            }
        }

        if (settingsHelper.getBoolean(MARK_AS_SEEN))
            storiesService.seen(currentStory.getStoryMediaId(),
                                currentStory.getTimestamp(),
                                System.currentTimeMillis() / 1000,
                                null);
    }

    private void downloadStory() {
        final Context context = getContext();
        if (context == null) return;
        if (currentStory == null) {
            Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
            return;
        }
        DownloadUtils.download(context, currentStory);
    }

    private void setupImage() {
        binding.progressView.setVisibility(View.VISIBLE);
        binding.playerView.setVisibility(View.GONE);
        binding.imageViewer.setVisibility(View.VISIBLE);
        final ImageRequest requestBuilder = ImageRequestBuilder.newBuilderWithSource(Uri.parse(url))
                                                               .setLocalThumbnailPreviewsEnabled(true)
                                                               .setProgressiveRenderingEnabled(true)
                                                               .build();
        final DraweeController controller = Fresco.newDraweeControllerBuilder()
                                                  .setImageRequest(requestBuilder)
                                                  .setOldController(binding.imageViewer.getController())
                                                  .setControllerListener(new BaseControllerListener<ImageInfo>() {

                                                      @Override
                                                      public void onFailure(final String id, final Throwable throwable) {
                                                          binding.progressView.setVisibility(View.GONE);
                                                      }

                                                      @Override
                                                      public void onFinalImageSet(final String id,
                                                                                  final ImageInfo imageInfo,
                                                                                  final Animatable animatable) {
                                                          if (menuDownload != null) {
                                                              downloadVisible = true;
                                                              menuDownload.setVisible(true);
                                                          }
                                                          if (currentStory.canReply() && menuDm != null) {
                                                              dmVisible = true;
                                                              menuDm.setVisible(true);
                                                          }
                                                          if (!TextUtils.isEmpty(currentStory.getUsername())) {
                                                              profileVisible = true;
                                                              menuProfile.setVisible(true);
                                                          }
                                                          binding.progressView.setVisibility(View.GONE);
                                                      }
                                                  })
                                                  .build();
        binding.imageViewer.setController(controller);
    }

    private void setupVideo() {
        binding.playerView.setVisibility(View.VISIBLE);
        binding.progressView.setVisibility(View.GONE);
        binding.imageViewer.setVisibility(View.GONE);
        binding.imageViewer.setController(null);

        final Context context = getContext();
        if (context == null) return;
        player = new SimpleExoPlayer.Builder(context).build();
        binding.playerView.setPlayer(player);
        player.setPlayWhenReady(settingsHelper.getBoolean(PreferenceKeys.AUTOPLAY_VIDEOS));

        final Uri uri = Uri.parse(url);
        final MediaItem mediaItem = MediaItem.fromUri(uri);
        final ProgressiveMediaSource mediaSource = new ProgressiveMediaSource.Factory(new DefaultDataSourceFactory(context, "instagram"))
                .createMediaSource(mediaItem);
        mediaSource.addEventListener(new Handler(), new MediaSourceEventListener() {
            @Override
            public void onLoadCompleted(final int windowIndex,
                                        @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                        @NonNull final LoadEventInfo loadEventInfo,
                                        @NonNull final MediaLoadData mediaLoadData) {
                if (menuDownload != null) {
                    downloadVisible = true;
                    menuDownload.setVisible(true);
                }
                if (currentStory.canReply() && menuDm != null) {
                    dmVisible = true;
                    menuDm.setVisible(true);
                }
                if (!TextUtils.isEmpty(currentStory.getUsername()) && menuProfile != null) {
                    profileVisible = true;
                    menuProfile.setVisible(true);
                }
                binding.progressView.setVisibility(View.GONE);
            }

            @Override
            public void onLoadStarted(final int windowIndex,
                                      @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                      @NonNull final LoadEventInfo loadEventInfo,
                                      @NonNull final MediaLoadData mediaLoadData) {
                if (menuDownload != null) {
                    downloadVisible = true;
                    menuDownload.setVisible(true);
                }
                if (currentStory.canReply() && menuDm != null) {
                    dmVisible = true;
                    menuDm.setVisible(true);
                }
                if (!TextUtils.isEmpty(currentStory.getUsername()) && menuProfile != null) {
                    profileVisible = true;
                    menuProfile.setVisible(true);
                }
                binding.progressView.setVisibility(View.VISIBLE);
            }

            @Override
            public void onLoadCanceled(final int windowIndex,
                                       @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                       @NonNull final LoadEventInfo loadEventInfo,
                                       @NonNull final MediaLoadData mediaLoadData) {
                binding.progressView.setVisibility(View.GONE);
            }

            @Override
            public void onLoadError(final int windowIndex,
                                    @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                    @NonNull final LoadEventInfo loadEventInfo,
                                    @NonNull final MediaLoadData mediaLoadData,
                                    @NonNull final IOException error,
                                    final boolean wasCanceled) {
                if (menuDownload != null) {
                    downloadVisible = false;
                    menuDownload.setVisible(false);
                }
                if (menuDm != null) {
                    dmVisible = false;
                    menuDm.setVisible(false);
                }
                if (menuProfile != null) {
                    profileVisible = false;
                    menuProfile.setVisible(false);
                }
                binding.progressView.setVisibility(View.GONE);
            }
        });
        player.setMediaSource(mediaSource);
        player.prepare();

        binding.playerView.setOnClickListener(v -> {
            if (player != null) {
                if (player.getPlaybackState() == Player.STATE_ENDED) player.seekTo(0);
                player.setPlayWhenReady(player.getPlaybackState() == Player.STATE_ENDED || !player.isPlaying());
            }
        });
    }

    private void setupLive() {
        binding.playerView.setVisibility(View.VISIBLE);
        binding.progressView.setVisibility(View.GONE);
        binding.imageViewer.setVisibility(View.GONE);
        binding.imageViewer.setController(null);

        if (menuDownload != null) menuDownload.setVisible(false);
        if (menuDm != null) menuDm.setVisible(false);

        final Context context = getContext();
        if (context == null) return;
        player = new SimpleExoPlayer.Builder(context).build();
        binding.playerView.setPlayer(player);
        player.setPlayWhenReady(settingsHelper.getBoolean(PreferenceKeys.AUTOPLAY_VIDEOS));

        final Uri uri = Uri.parse(url);
        final MediaItem mediaItem = MediaItem.fromUri(uri);
        final DashMediaSource mediaSource = new DashMediaSource.Factory(new DefaultDataSourceFactory(context, "instagram"))
                .createMediaSource(mediaItem);
        mediaSource.addEventListener(new Handler(), new MediaSourceEventListener() {
            @Override
            public void onLoadCompleted(final int windowIndex,
                                        @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                        @NonNull final LoadEventInfo loadEventInfo,
                                        @NonNull final MediaLoadData mediaLoadData) {
                binding.progressView.setVisibility(View.GONE);
            }

            @Override
            public void onLoadStarted(final int windowIndex,
                                      @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                      @NonNull final LoadEventInfo loadEventInfo,
                                      @NonNull final MediaLoadData mediaLoadData) {
                binding.progressView.setVisibility(View.VISIBLE);
            }

            @Override
            public void onLoadCanceled(final int windowIndex,
                                       @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                       @NonNull final LoadEventInfo loadEventInfo,
                                       @NonNull final MediaLoadData mediaLoadData) {
                binding.progressView.setVisibility(View.GONE);
            }

            @Override
            public void onLoadError(final int windowIndex,
                                    @Nullable final MediaSource.MediaPeriodId mediaPeriodId,
                                    @NonNull final LoadEventInfo loadEventInfo,
                                    @NonNull final MediaLoadData mediaLoadData,
                                    @NonNull final IOException error,
                                    final boolean wasCanceled) {
                binding.progressView.setVisibility(View.GONE);
            }
        });
        player.setMediaSource(mediaSource);
        player.prepare();

        binding.playerView.setOnClickListener(v -> {
            if (player != null) {
                if (player.getPlaybackState() == Player.STATE_ENDED) player.seekTo(0);
                player.setPlayWhenReady(player.getPlaybackState() == Player.STATE_ENDED || !player.isPlaying());
            }
        });
    }

    private void openProfile(final String username) {
        final ActionBar actionBar = fragmentActivity.getSupportActionBar();
        if (actionBar != null) {
            actionBar.setSubtitle(null);
        }
        final char t = username.charAt(0);
        if (t == '@') {
            final NavDirections action = HashTagFragmentDirections.actionGlobalProfileFragment(username);
            NavHostFragment.findNavController(this).navigate(action);
        } else if (t == '#') {
            final NavDirections action = HashTagFragmentDirections.actionGlobalHashTagFragment(username.substring(1));
            NavHostFragment.findNavController(this).navigate(action);
        } else {
            final NavDirections action = ProfileFragmentDirections
                    .actionGlobalLocationFragment(Long.parseLong(username.split(" \\(")[1].replace(")", "")));
            NavHostFragment.findNavController(this).navigate(action);
        }
    }

    private void releasePlayer() {
        if (player == null) return;
        try { player.stop(true); } catch (Exception ignored) { }
        try { player.release(); } catch (Exception ignored) { }
        player = null;
    }

    private void paginateStories(Object newFeedStory, Object oldFeedStory, Context context, boolean backward, boolean last) {
        if (newFeedStory != null) {
            if (fetching) {
                Toast.makeText(context, R.string.be_patient, Toast.LENGTH_SHORT).show();
                return;
            }
            if (settingsHelper.getBoolean(MARK_AS_SEEN)
                    && oldFeedStory instanceof FeedStoryModel
                    && viewModel instanceof FeedStoriesViewModel) {
                final FeedStoriesViewModel feedStoriesViewModel = (FeedStoriesViewModel) viewModel;
                final FeedStoryModel oldFeedStoryModel = (FeedStoryModel) oldFeedStory;
                if (!oldFeedStoryModel.isFullyRead()) {
                    oldFeedStoryModel.setFullyRead(true);
                    final List<FeedStoryModel> models = feedStoriesViewModel.getList().getValue();
                    final List<FeedStoryModel> modelsCopy = models == null ? new ArrayList<>() : new ArrayList<>(models);
                    modelsCopy.set(currentFeedStoryIndex, oldFeedStoryModel);
                    feedStoriesViewModel.getList().postValue(models);
                }
            }
            fetching = true;
            binding.btnBackward.setVisibility(currentFeedStoryIndex == 1 && backward ? View.INVISIBLE : View.VISIBLE);
            binding.btnForward.setVisibility(last ? View.INVISIBLE : View.VISIBLE);
            currentFeedStoryIndex = backward ? (currentFeedStoryIndex - 1) : (currentFeedStoryIndex + 1);
            resetView();
        }
    }

    /**
     * Parses the Story's media ID. For user stories this is a number, but for archive stories
     * this is "archiveDay:" plus a number.
     */
    private static String parseStoryMediaId(String rawId) {
        final String regex = "(?:archiveDay:)?(.+)";
        final Pattern pattern = Pattern.compile(regex);
        final Matcher matcher = pattern.matcher(rawId);

        if (matcher.matches() && matcher.groupCount() >= 1) {
            return matcher.group(1);
        }

        return rawId;
    }
}
