package awais.instagrabber.fragments;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.text.SpannableStringBuilder;
import android.text.style.RelativeSizeSpan;
import android.text.style.StyleSpan;
import android.util.Log;
import android.view.ActionMode;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.activity.OnBackPressedCallback;
import androidx.activity.OnBackPressedDispatcher;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.constraintlayout.motion.widget.MotionLayout;
import androidx.constraintlayout.motion.widget.MotionScene;
import androidx.core.content.PermissionChecker;
import androidx.fragment.app.Fragment;
import androidx.navigation.NavController;
import androidx.navigation.NavDirections;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.RecyclerView;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import com.google.android.material.snackbar.BaseTransientBottomBar;
import com.google.android.material.snackbar.Snackbar;
import com.google.common.collect.ImmutableList;

import java.util.Date;
import java.util.List;
import java.util.Set;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.FeedAdapterV2;
import awais.instagrabber.asyncs.HashtagPostFetchService;
import awais.instagrabber.customviews.PrimaryActionModeCallback;
import awais.instagrabber.databinding.FragmentHashtagBinding;
import awais.instagrabber.databinding.LayoutHashtagDetailsBinding;
import awais.instagrabber.db.datasources.FavoriteDataSource;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.db.repositories.FavoriteRepository;
import awais.instagrabber.db.repositories.RepositoryCallback;
import awais.instagrabber.dialogs.PostsLayoutPreferencesDialogFragment;
import awais.instagrabber.models.PostsLayoutPreferences;
import awais.instagrabber.models.StoryModel;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.models.enums.FollowingType;
import awais.instagrabber.repositories.requests.StoryViewerOptions;
import awais.instagrabber.repositories.responses.Hashtag;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.DownloadUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.webservices.GraphQLService;
import awais.instagrabber.webservices.ServiceCallback;
import awais.instagrabber.webservices.StoriesService;
import awais.instagrabber.webservices.TagsService;

import static androidx.core.content.PermissionChecker.checkSelfPermission;
import static awais.instagrabber.utils.DownloadUtils.WRITE_PERMISSION;
import static awais.instagrabber.utils.Utils.settingsHelper;

public class HashTagFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = "HashTagFragment";
    private static final int STORAGE_PERM_REQUEST_CODE = 8020;
    private static final int STORAGE_PERM_REQUEST_CODE_FOR_SELECTION = 8030;

    public static final String ARG_HASHTAG = "hashtag";

    private MainActivity fragmentActivity;
    private FragmentHashtagBinding binding;
    private MotionLayout root;
    private boolean shouldRefresh = true;
    private boolean hasStories = false;
    private boolean opening = false;
    private String hashtag;
    private Hashtag hashtagModel = null;
    private ActionMode actionMode;
    private StoriesService storiesService;
    private boolean isLoggedIn;
    private TagsService tagsService;
    private GraphQLService graphQLService;
    private boolean storiesFetching;
    private Set<Media> selectedFeedModels;
    private Media downloadFeedModel;
    private int downloadChildPosition = -1;
    private PostsLayoutPreferences layoutPreferences = Utils.getPostsLayoutPreferences(Constants.PREF_HASHTAG_POSTS_LAYOUT);
    private LayoutHashtagDetailsBinding hashtagDetailsBinding;

    private final OnBackPressedCallback onBackPressedCallback = new OnBackPressedCallback(false) {
        @Override
        public void handleOnBackPressed() {
            binding.posts.endSelection();
        }
    };
    private final PrimaryActionModeCallback multiSelectAction = new PrimaryActionModeCallback(
            R.menu.multi_select_download_menu,
            new PrimaryActionModeCallback.CallbacksHelper() {
                @Override
                public void onDestroy(final ActionMode mode) {
                    binding.posts.endSelection();
                }

                @Override
                public boolean onActionItemClicked(final ActionMode mode, final MenuItem item) {
                    if (item.getItemId() == R.id.action_download) {
                        if (HashTagFragment.this.selectedFeedModels == null) return false;
                        final Context context = getContext();
                        if (context == null) return false;
                        if (checkSelfPermission(context, WRITE_PERMISSION) == PermissionChecker.PERMISSION_GRANTED) {
                            DownloadUtils.download(context, ImmutableList.copyOf(HashTagFragment.this.selectedFeedModels));
                            binding.posts.endSelection();
                            return true;
                        }
                        requestPermissions(DownloadUtils.PERMS, STORAGE_PERM_REQUEST_CODE_FOR_SELECTION);
                        return true;
                    }
                    return false;
                }
            });
    private final FeedAdapterV2.FeedItemCallback feedItemCallback = new FeedAdapterV2.FeedItemCallback() {
        @Override
        public void onPostClick(final Media feedModel, final View profilePicView, final View mainPostImage) {
            openPostDialog(feedModel, profilePicView, mainPostImage, -1);
        }

        @Override
        public void onSliderClick(final Media feedModel, final int position) {
            openPostDialog(feedModel, null, null, position);
        }

        @Override
        public void onCommentsClick(final Media feedModel) {
            final NavDirections commentsAction = HashTagFragmentDirections.actionGlobalCommentsViewerFragment(
                    feedModel.getCode(),
                    feedModel.getCode(),
                    feedModel.getUser().getPk()
            );
            NavHostFragment.findNavController(HashTagFragment.this).navigate(commentsAction);
        }

        @Override
        public void onDownloadClick(final Media feedModel, final int childPosition) {
            final Context context = getContext();
            if (context == null) return;
            if (checkSelfPermission(context, WRITE_PERMISSION) == PermissionChecker.PERMISSION_GRANTED) {
                DownloadUtils.showDownloadDialog(context, feedModel, childPosition);
                return;
            }
            downloadFeedModel = feedModel;
            downloadChildPosition = childPosition;
            requestPermissions(DownloadUtils.PERMS, STORAGE_PERM_REQUEST_CODE);
        }

        @Override
        public void onHashtagClick(final String hashtag) {
            final NavDirections action = HashTagFragmentDirections.actionGlobalHashTagFragment(hashtag);
            NavHostFragment.findNavController(HashTagFragment.this).navigate(action);
        }

        @Override
        public void onLocationClick(final Media media) {
            final Location location = media.getLocation();
            if (location == null) return;
            final NavDirections action = HashTagFragmentDirections.actionGlobalLocationFragment(location.getPk());
            NavHostFragment.findNavController(HashTagFragment.this).navigate(action);
        }

        @Override
        public void onMentionClick(final String mention) {
            navigateToProfile(mention.trim());
        }

        @Override
        public void onNameClick(final Media feedModel, final View profilePicView) {
            navigateToProfile("@" + feedModel.getUser().getUsername());
        }

        @Override
        public void onProfilePicClick(final Media feedModel, final View profilePicView) {
            navigateToProfile("@" + feedModel.getUser().getUsername());
        }

        @Override
        public void onURLClick(final String url) {
            Utils.openURL(getContext(), url);
        }

        @Override
        public void onEmailClick(final String emailId) {
            Utils.openEmailAddress(getContext(), emailId);
        }

        private void openPostDialog(@NonNull final Media feedModel,
                                    final View profilePicView,
                                    final View mainPostImage,
                                    final int position) {
            if (opening) return;
            final User user = feedModel.getUser();
            if (user == null) return;
            if (TextUtils.isEmpty(user.getUsername())) {
                // this only happens for anons
                opening = true;
                graphQLService.fetchPost(feedModel.getCode(), new ServiceCallback<Media>() {
                    @Override
                    public void onSuccess(final Media newFeedModel) {
                        opening = false;
                        if (newFeedModel == null) return;
                        openPostDialog(newFeedModel, profilePicView, mainPostImage, position);
                    }

                    @Override
                    public void onFailure(final Throwable t) {
                        opening = false;
                        Log.e(TAG, "Error", t);
                    }
                });
                return;
            }
            opening = true;
            final NavController navController = NavHostFragment.findNavController(HashTagFragment.this);
            final Bundle bundle = new Bundle();
            bundle.putSerializable(PostViewV2Fragment.ARG_MEDIA, feedModel);
            bundle.putInt(PostViewV2Fragment.ARG_SLIDER_POSITION, position);
            try {
                navController.navigate(R.id.action_global_post_view, bundle);
            } catch (Exception e) {
                Log.e(TAG, "openPostDialog: ", e);
            }
            opening = false;
        }
    };
    private final FeedAdapterV2.SelectionModeCallback selectionModeCallback = new FeedAdapterV2.SelectionModeCallback() {

        @Override
        public void onSelectionStart() {
            if (!onBackPressedCallback.isEnabled()) {
                final OnBackPressedDispatcher onBackPressedDispatcher = fragmentActivity.getOnBackPressedDispatcher();
                onBackPressedCallback.setEnabled(true);
                onBackPressedDispatcher.addCallback(getViewLifecycleOwner(), onBackPressedCallback);
            }
            if (actionMode == null) {
                actionMode = fragmentActivity.startActionMode(multiSelectAction);
            }
        }

        @Override
        public void onSelectionChange(final Set<Media> selectedFeedModels) {
            final String title = getString(R.string.number_selected, selectedFeedModels.size());
            if (actionMode != null) {
                actionMode.setTitle(title);
            }
            HashTagFragment.this.selectedFeedModels = selectedFeedModels;
        }

        @Override
        public void onSelectionEnd() {
            if (onBackPressedCallback.isEnabled()) {
                onBackPressedCallback.setEnabled(false);
                onBackPressedCallback.remove();
            }
            if (actionMode != null) {
                actionMode.finish();
                actionMode = null;
            }
        }
    };
    private final ServiceCallback<Hashtag> cb = new ServiceCallback<Hashtag>() {
        @Override
        public void onSuccess(final Hashtag result) {
            hashtagModel = result;
            binding.swipeRefreshLayout.setRefreshing(false);
            setHashtagDetails();
        }

        @Override
        public void onFailure(final Throwable t) {
            setHashtagDetails();
        }
    };

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = (MainActivity) requireActivity();
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) > 0;
        tagsService = isLoggedIn ? TagsService.getInstance() : null;
        storiesService = isLoggedIn ? StoriesService.getInstance(null, 0L, null) : null;
        graphQLService = isLoggedIn ? null : GraphQLService.getInstance();
        setHasOptionsMenu(true);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentHashtagBinding.inflate(inflater, container, false);
        root = binding.getRoot();
        hashtagDetailsBinding = binding.header;
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (!shouldRefresh) return;
        binding.swipeRefreshLayout.setOnRefreshListener(this);
        init();
        shouldRefresh = false;
    }

    @Override
    public void onRefresh() {
        binding.posts.refresh();
        fetchStories();
    }

    @Override
    public void onResume() {
        super.onResume();
        setTitle();
    }

    @Override
    public void onCreateOptionsMenu(@NonNull final Menu menu, @NonNull final MenuInflater inflater) {
        inflater.inflate(R.menu.topic_posts_menu, menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        if (item.getItemId() == R.id.layout) {
            showPostsLayoutPreferences();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        final boolean granted = grantResults[0] == PackageManager.PERMISSION_GRANTED;
        final Context context = getContext();
        if (context == null) return;
        if (requestCode == STORAGE_PERM_REQUEST_CODE && granted) {
            if (downloadFeedModel == null) return;
            DownloadUtils.showDownloadDialog(context, downloadFeedModel, downloadChildPosition);
            downloadFeedModel = null;
            downloadChildPosition = -1;
            return;
        }
        if (requestCode == STORAGE_PERM_REQUEST_CODE_FOR_SELECTION && granted) {
            DownloadUtils.download(context, ImmutableList.copyOf(selectedFeedModels));
            binding.posts.endSelection();
        }
    }

    private void init() {
        if (getArguments() == null) return;
        final HashTagFragmentArgs fragmentArgs = HashTagFragmentArgs.fromBundle(getArguments());
        hashtag = fragmentArgs.getHashtag();
        if (hashtag.charAt(0) == '#') hashtag = hashtag.substring(1);
        fetchHashtagModel();
    }

    private void fetchHashtagModel() {
        binding.swipeRefreshLayout.setRefreshing(true);
        if (isLoggedIn) tagsService.fetch(hashtag, cb);
        else graphQLService.fetchTag(hashtag, cb);
    }

    private void setupPosts() {
        binding.posts.setViewModelStoreOwner(this)
                     .setLifeCycleOwner(this)
                     .setPostFetchService(new HashtagPostFetchService(hashtagModel, isLoggedIn))
                     .setLayoutPreferences(layoutPreferences)
                     .addFetchStatusChangeListener(fetching -> updateSwipeRefreshState())
                     .setFeedItemCallback(feedItemCallback)
                     .setSelectionModeCallback(selectionModeCallback)
                     .init();
        binding.swipeRefreshLayout.setRefreshing(true);
        binding.posts.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrolled(@NonNull final RecyclerView recyclerView, final int dx, final int dy) {
                super.onScrolled(recyclerView, dx, dy);
                final boolean canScrollVertically = recyclerView.canScrollVertically(-1);
                final MotionScene.Transition transition = root.getTransition(R.id.transition);
                if (transition != null) {
                    transition.setEnable(!canScrollVertically);
                }
            }
        });
    }

    private void setHashtagDetails() {
        if (hashtagModel == null) {
            try {
                Toast.makeText(getContext(), R.string.error_loading_hashtag, Toast.LENGTH_SHORT).show();
                binding.swipeRefreshLayout.setEnabled(false);
            } catch (Exception ignored) {}
            return;
        }
        setTitle();
        setupPosts();
        fetchStories();
        if (isLoggedIn) {
            hashtagDetailsBinding.btnFollowTag.setVisibility(View.VISIBLE);
            hashtagDetailsBinding.btnFollowTag.setText(hashtagModel.getFollowing() == FollowingType.FOLLOWING
                                                       ? R.string.unfollow
                                                       : R.string.follow);
            hashtagDetailsBinding.btnFollowTag.setChipIconResource(hashtagModel.getFollowing() == FollowingType.FOLLOWING
                                                                   ? R.drawable.ic_outline_person_add_disabled_24
                                                                   : R.drawable.ic_outline_person_add_24);
            hashtagDetailsBinding.btnFollowTag.setOnClickListener(v -> {
                final String cookie = settingsHelper.getString(Constants.COOKIE);
                final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
                final long userId = CookieUtils.getUserIdFromCookie(cookie);
                final String deviceUuid = Utils.settingsHelper.getString(Constants.DEVICE_UUID);
                if (csrfToken != null && userId != 0) {
                    hashtagDetailsBinding.btnFollowTag.setClickable(false);
                    tagsService.changeFollow(
                            hashtagModel.getFollowing() == FollowingType.FOLLOWING ? "unfollow" : "follow",
                            hashtag,
                            csrfToken,
                            userId,
                            deviceUuid,
                            new ServiceCallback<Boolean>() {
                                @Override
                                public void onSuccess(final Boolean result) {
                                    hashtagDetailsBinding.btnFollowTag.setClickable(true);
                                    if (!result) {
                                        Log.e(TAG, "onSuccess: result is false");
                                        Snackbar.make(root, R.string.downloader_unknown_error, BaseTransientBottomBar.LENGTH_LONG)
                                                .show();
                                        return;
                                    }
                                    hashtagDetailsBinding.btnFollowTag.setText(R.string.unfollow);
                                    hashtagDetailsBinding.btnFollowTag.setChipIconResource(R.drawable.ic_outline_person_add_disabled_24);
                                }

                                @Override
                                public void onFailure(@NonNull final Throwable t) {
                                    hashtagDetailsBinding.btnFollowTag.setClickable(true);
                                    Log.e(TAG, "onFailure: ", t);
                                    final String message = t.getMessage();
                                    Snackbar.make(
                                            root,
                                            message != null ? message : getString(R.string.downloader_unknown_error),
                                            BaseTransientBottomBar.LENGTH_LONG)
                                            .show();
                                }
                            });
                }
            });
        } else {
            hashtagDetailsBinding.btnFollowTag.setVisibility(View.GONE);
        }
        hashtagDetailsBinding.favChip.setVisibility(View.VISIBLE);
        final Context context = getContext();
        if (context == null) return;
        final FavoriteRepository favoriteRepository = FavoriteRepository.getInstance(FavoriteDataSource.getInstance(context));
        favoriteRepository.getFavorite(hashtag, FavoriteType.HASHTAG, new RepositoryCallback<Favorite>() {
            @Override
            public void onSuccess(final Favorite result) {
                favoriteRepository.insertOrUpdateFavorite(new Favorite(
                        result.getId(),
                        hashtag,
                        FavoriteType.HASHTAG,
                        hashtagModel.getName(),
                        "res:/" + R.drawable.ic_hashtag,
                        result.getDateAdded()
                ), new RepositoryCallback<Void>() {
                    @Override
                    public void onSuccess(final Void result) {
                        hashtagDetailsBinding.favChip.setChipIconResource(R.drawable.ic_star_check_24);
                        hashtagDetailsBinding.favChip.setText(R.string.favorite_short);
                    }

                    @Override
                    public void onDataNotAvailable() {}
                });
            }

            @Override
            public void onDataNotAvailable() {
                hashtagDetailsBinding.favChip.setChipIconResource(R.drawable.ic_outline_star_plus_24);
                hashtagDetailsBinding.favChip.setText(R.string.add_to_favorites);
            }
        });
        hashtagDetailsBinding.favChip.setOnClickListener(
                v -> favoriteRepository.getFavorite(hashtag, FavoriteType.HASHTAG, new RepositoryCallback<Favorite>() {
                    @Override
                    public void onSuccess(final Favorite result) {
                        favoriteRepository.deleteFavorite(hashtag, FavoriteType.HASHTAG, new RepositoryCallback<Void>() {
                            @Override
                            public void onSuccess(final Void result) {
                                hashtagDetailsBinding.favChip.setText(R.string.add_to_favorites);
                                hashtagDetailsBinding.favChip.setChipIconResource(R.drawable.ic_outline_star_plus_24);
                                showSnackbar(getString(R.string.removed_from_favs));
                            }

                            @Override
                            public void onDataNotAvailable() {}
                        });
                    }

                    @Override
                    public void onDataNotAvailable() {
                        favoriteRepository.insertOrUpdateFavorite(new Favorite(
                                0,
                                hashtag,
                                FavoriteType.HASHTAG,
                                hashtagModel.getName(),
                                "res:/" + R.drawable.ic_hashtag,
                                new Date()
                        ), new RepositoryCallback<Void>() {
                            @Override
                            public void onSuccess(final Void result) {
                                hashtagDetailsBinding.favChip.setText(R.string.favorite_short);
                                hashtagDetailsBinding.favChip.setChipIconResource(R.drawable.ic_star_check_24);
                                showSnackbar(getString(R.string.added_to_favs));
                            }

                            @Override
                            public void onDataNotAvailable() {}
                        });
                    }
                }));
        hashtagDetailsBinding.mainHashtagImage.setImageURI("res:/" + R.drawable.ic_hashtag);
        final String postCount = String.valueOf(hashtagModel.getMediaCount());
        final SpannableStringBuilder span = new SpannableStringBuilder(getResources().getQuantityString(R.plurals.main_posts_count_inline,
                                                                                                        hashtagModel.getMediaCount() > 2000000000L
                                                                                                        ? 2000000000
                                                                                                        : hashtagModel.getMediaCount().intValue(),
                                                                                                        postCount));
        span.setSpan(new RelativeSizeSpan(1.2f), 0, postCount.length(), 0);
        span.setSpan(new StyleSpan(Typeface.BOLD), 0, postCount.length(), 0);
        hashtagDetailsBinding.mainTagPostCount.setText(span);
        hashtagDetailsBinding.mainTagPostCount.setVisibility(View.VISIBLE);
        hashtagDetailsBinding.mainHashtagImage.setOnClickListener(v -> {
            if (!hasStories) return;
            // show stories
            final NavDirections action = HashTagFragmentDirections
                    .actionHashtagFragmentToStoryViewerFragment(StoryViewerOptions.forHashtag(hashtagModel.getName()));
            NavHostFragment.findNavController(this).navigate(action);
        });
    }

    private void showSnackbar(final String message) {
        @SuppressLint("ShowToast") final Snackbar snackbar = Snackbar.make(root, message, BaseTransientBottomBar.LENGTH_LONG);
        snackbar.setAction(R.string.ok, v1 -> snackbar.dismiss())
                .setAnimationMode(BaseTransientBottomBar.ANIMATION_MODE_SLIDE)
                .setAnchorView(fragmentActivity.getBottomNavView())
                .show();
    }

    private void fetchStories() {
        if (!isLoggedIn) return;
        storiesFetching = true;
        storiesService.getUserStory(
                StoryViewerOptions.forHashtag(hashtagModel.getName()),
                new ServiceCallback<List<StoryModel>>() {
                    @Override
                    public void onSuccess(final List<StoryModel> storyModels) {
                        if (storyModels != null && !storyModels.isEmpty()) {
                            hashtagDetailsBinding.mainHashtagImage.setStoriesBorder(1);
                            hasStories = true;
                        } else {
                            hasStories = false;
                        }
                        storiesFetching = false;
                    }

                    @Override
                    public void onFailure(final Throwable t) {
                        Log.e(TAG, "Error", t);
                        storiesFetching = false;
                    }
                });
    }

    private void setTitle() {
        final ActionBar actionBar = fragmentActivity.getSupportActionBar();
        if (actionBar != null) {
            // Log.d(TAG, "setting title: " + hashtag);
            actionBar.setTitle('#' + hashtag);
            // final Handler handler = new Handler();
            // handler.postDelayed(() -> , 1000);
        }
    }

    private void updateSwipeRefreshState() {
        binding.swipeRefreshLayout.setRefreshing(binding.posts.isFetching() || storiesFetching);
    }

    private void navigateToProfile(final String username) {
        final NavController navController = NavHostFragment.findNavController(this);
        final Bundle bundle = new Bundle();
        bundle.putString("username", username);
        navController.navigate(R.id.action_global_profileFragment, bundle);
    }

    private void showPostsLayoutPreferences() {
        final PostsLayoutPreferencesDialogFragment fragment = new PostsLayoutPreferencesDialogFragment(
                Constants.PREF_HASHTAG_POSTS_LAYOUT,
                preferences -> {
                    layoutPreferences = preferences;
                    new Handler().postDelayed(() -> binding.posts.setLayoutPreferences(preferences), 200);
                });
        fragment.show(getChildFragmentManager(), "posts_layout_preferences");
    }
}
