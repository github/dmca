package awais.instagrabber.fragments.main;

import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
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
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.content.res.AppCompatResources;
import androidx.constraintlayout.motion.widget.MotionLayout;
import androidx.constraintlayout.motion.widget.MotionScene;
import androidx.core.content.PermissionChecker;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.NavDirections;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import com.google.android.material.snackbar.BaseTransientBottomBar;
import com.google.android.material.snackbar.Snackbar;
import com.google.common.collect.ImmutableList;

import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.FeedAdapterV2;
import awais.instagrabber.adapters.HighlightsAdapter;
import awais.instagrabber.asyncs.ProfilePostFetchService;
import awais.instagrabber.customviews.PrimaryActionModeCallback;
import awais.instagrabber.customviews.PrimaryActionModeCallback.CallbacksHelper;
import awais.instagrabber.databinding.FragmentProfileBinding;
import awais.instagrabber.databinding.LayoutProfileDetailsBinding;
import awais.instagrabber.db.datasources.AccountDataSource;
import awais.instagrabber.db.datasources.FavoriteDataSource;
import awais.instagrabber.db.entities.Account;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.db.repositories.AccountRepository;
import awais.instagrabber.db.repositories.FavoriteRepository;
import awais.instagrabber.db.repositories.RepositoryCallback;
import awais.instagrabber.dialogs.PostsLayoutPreferencesDialogFragment;
import awais.instagrabber.dialogs.ProfilePicDialogFragment;
import awais.instagrabber.fragments.PostViewV2Fragment;
import awais.instagrabber.managers.DirectMessagesManager;
import awais.instagrabber.managers.InboxManager;
import awais.instagrabber.models.HighlightModel;
import awais.instagrabber.models.PostsLayoutPreferences;
import awais.instagrabber.models.StoryModel;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.models.enums.PostItemType;
import awais.instagrabber.repositories.requests.StoryViewerOptions;
import awais.instagrabber.repositories.responses.FriendshipChangeResponse;
import awais.instagrabber.repositories.responses.FriendshipRestrictResponse;
import awais.instagrabber.repositories.responses.FriendshipStatus;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.UserProfileContextLink;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.DownloadUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.AppStateViewModel;
import awais.instagrabber.viewmodels.HighlightsViewModel;
import awais.instagrabber.webservices.DirectMessagesService;
import awais.instagrabber.webservices.FriendshipService;
import awais.instagrabber.webservices.GraphQLService;
import awais.instagrabber.webservices.MediaService;
import awais.instagrabber.webservices.ServiceCallback;
import awais.instagrabber.webservices.StoriesService;
import awais.instagrabber.webservices.UserService;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import static androidx.core.content.PermissionChecker.checkSelfPermission;
import static awais.instagrabber.fragments.HashTagFragment.ARG_HASHTAG;
import static awais.instagrabber.utils.DownloadUtils.WRITE_PERMISSION;

public class ProfileFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = "ProfileFragment";
    private static final int STORAGE_PERM_REQUEST_CODE = 8020;
    private static final int STORAGE_PERM_REQUEST_CODE_FOR_SELECTION = 8030;

    private MainActivity fragmentActivity;
    private MotionLayout root;
    private FragmentProfileBinding binding;
    private boolean isLoggedIn;
    private String cookie;
    private String username;
    private User profileModel;
    private ActionMode actionMode;
    private Handler usernameSettingHandler;
    private FriendshipService friendshipService;
    private StoriesService storiesService;
    private MediaService mediaService;
    private UserService userService;
    private GraphQLService graphQLService;
    private DirectMessagesService directMessagesService;
    private boolean shouldRefresh = true;
    private boolean hasStories = false;
    private HighlightsAdapter highlightsAdapter;
    private HighlightsViewModel highlightsViewModel;
    private MenuItem blockMenuItem, restrictMenuItem, chainingMenuItem;
    private MenuItem muteStoriesMenuItem, mutePostsMenuItem, removeFollowerMenuItem;
    private boolean accountIsUpdated = false;
    private boolean postsSetupDone = false;
    private Set<Media> selectedFeedModels;
    private Media downloadFeedModel;
    private int downloadChildPosition = -1;
    private long myId;
    private PostsLayoutPreferences layoutPreferences = Utils.getPostsLayoutPreferences(Constants.PREF_PROFILE_POSTS_LAYOUT);

    private final ServiceCallback<FriendshipChangeResponse> changeCb = new ServiceCallback<FriendshipChangeResponse>() {
        @Override
        public void onSuccess(final FriendshipChangeResponse result) {
            if (result.getFriendshipStatus() != null) {
                profileModel.setFriendshipStatus(result.getFriendshipStatus());
                setProfileDetails();
                return;
            }
            fetchProfileDetails();
        }

        @Override
        public void onFailure(final Throwable t) {
            Log.e(TAG, "Error editing relationship", t);
        }
    };

    private final Runnable usernameSettingRunnable = () -> {
        final ActionBar actionBar = fragmentActivity.getSupportActionBar();
        if (actionBar != null && !TextUtils.isEmpty(username)) {
            final String finalUsername = username.startsWith("@") ? username.substring(1)
                                                                  : username;
            actionBar.setTitle(finalUsername);
            actionBar.setSubtitle(null);
        }
    };
    private final OnBackPressedCallback onBackPressedCallback = new OnBackPressedCallback(false) {
        @Override
        public void handleOnBackPressed() {
            binding.postsRecyclerView.endSelection();
        }
    };
    private final PrimaryActionModeCallback multiSelectAction = new PrimaryActionModeCallback(
            R.menu.multi_select_download_menu,
            new CallbacksHelper() {
                @Override
                public void onDestroy(final ActionMode mode) {
                    binding.postsRecyclerView.endSelection();
                }

                @Override
                public boolean onActionItemClicked(final ActionMode mode, final MenuItem item) {
                    if (item.getItemId() == R.id.action_download) {
                        if (ProfileFragment.this.selectedFeedModels == null) return false;
                        final Context context = getContext();
                        if (context == null) return false;
                        if (checkSelfPermission(context, WRITE_PERMISSION) == PermissionChecker.PERMISSION_GRANTED) {
                            DownloadUtils.download(context, ImmutableList.copyOf(ProfileFragment.this.selectedFeedModels));
                            binding.postsRecyclerView.endSelection();
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
            final NavDirections commentsAction = ProfileFragmentDirections.actionGlobalCommentsViewerFragment(
                    feedModel.getCode(),
                    feedModel.getPk(),
                    feedModel.getUser().getPk()
            );
            NavHostFragment.findNavController(ProfileFragment.this).navigate(commentsAction);
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
            final NavDirections action = FeedFragmentDirections.actionGlobalHashTagFragment(hashtag);
            NavHostFragment.findNavController(ProfileFragment.this).navigate(action);
        }

        @Override
        public void onLocationClick(final Media feedModel) {
            final NavDirections action = FeedFragmentDirections.actionGlobalLocationFragment(feedModel.getLocation().getPk());
            NavHostFragment.findNavController(ProfileFragment.this).navigate(action);
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

        private void openPostDialog(final Media feedModel,
                                    final View profilePicView,
                                    final View mainPostImage,
                                    final int position) {
            final NavController navController = NavHostFragment.findNavController(ProfileFragment.this);
            final Bundle bundle = new Bundle();
            bundle.putSerializable(PostViewV2Fragment.ARG_MEDIA, feedModel);
            bundle.putInt(PostViewV2Fragment.ARG_SLIDER_POSITION, position);
            try {
                navController.navigate(R.id.action_global_post_view, bundle);
            } catch (Exception e) {
                Log.e(TAG, "openPostDialog: ", e);
            }
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
            ProfileFragment.this.selectedFeedModels = selectedFeedModels;
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
    private LayoutProfileDetailsBinding profileDetailsBinding;
    private AccountRepository accountRepository;
    private FavoriteRepository favoriteRepository;
    private AppStateViewModel appStateViewModel;
    private boolean disableDm = false;

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        cookie = Utils.settingsHelper.getString(Constants.COOKIE);
        isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) > 0;
        myId = CookieUtils.getUserIdFromCookie(cookie);
        final String deviceUuid = Utils.settingsHelper.getString(Constants.DEVICE_UUID);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        fragmentActivity = (MainActivity) requireActivity();
        friendshipService = isLoggedIn ? FriendshipService.getInstance(deviceUuid, csrfToken, myId) : null;
        directMessagesService = isLoggedIn ? DirectMessagesService.getInstance(csrfToken, myId, deviceUuid) : null;
        storiesService = isLoggedIn ? StoriesService.getInstance(null, 0L, null) : null;
        mediaService = isLoggedIn ? MediaService.getInstance(null, null, 0) : null;
        userService = isLoggedIn ? UserService.getInstance() : null;
        graphQLService = isLoggedIn ? null : GraphQLService.getInstance();
        final Context context = getContext();
        if (context == null) return;
        accountRepository = AccountRepository.getInstance(AccountDataSource.getInstance(context));
        favoriteRepository = FavoriteRepository.getInstance(FavoriteDataSource.getInstance(context));
        appStateViewModel = new ViewModelProvider(fragmentActivity).get(AppStateViewModel.class);
        setHasOptionsMenu(true);
    }

    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             final ViewGroup container,
                             final Bundle savedInstanceState) {
        if (root != null) {
            if (getArguments() != null) {
                final ProfileFragmentArgs fragmentArgs = ProfileFragmentArgs.fromBundle(getArguments());
                final String username = fragmentArgs.getUsername();
                if (TextUtils.isEmpty(username) && profileModel != null) {
                    final String profileModelUsername = profileModel.getUsername();
                    final boolean isSame = ("@" + profileModelUsername).equals(this.username);
                    if (isSame) {
                        setUsernameDelayed();
                        shouldRefresh = false;
                        return root;
                    }
                }
                if (username == null || !username.equals(this.username)) {
                    shouldRefresh = true;
                    return root;
                }
            }
            setUsernameDelayed();
            shouldRefresh = false;
            return root;
        }
        binding = FragmentProfileBinding.inflate(inflater, container, false);
        root = binding.getRoot();
        profileDetailsBinding = binding.header;
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
    public void onCreateOptionsMenu(@NonNull final Menu menu, @NonNull final MenuInflater inflater) {
        inflater.inflate(R.menu.profile_menu, menu);
        final boolean isNotMe = profileModel != null && isLoggedIn
                                && !Objects.equals(profileModel.getPk(), CookieUtils.getUserIdFromCookie(cookie));
        blockMenuItem = menu.findItem(R.id.block);
        if (blockMenuItem != null) {
            if (isNotMe) {
                blockMenuItem.setVisible(true);
                blockMenuItem.setTitle(profileModel.getFriendshipStatus().isBlocking() ? R.string.unblock : R.string.block);
            } else {
                blockMenuItem.setVisible(false);
            }
        }
        restrictMenuItem = menu.findItem(R.id.restrict);
        if (restrictMenuItem != null) {
            if (isNotMe) {
                restrictMenuItem.setVisible(true);
                restrictMenuItem.setTitle(profileModel.getFriendshipStatus().isRestricted() ? R.string.unrestrict : R.string.restrict);
            } else {
                restrictMenuItem.setVisible(false);
            }
        }
        muteStoriesMenuItem = menu.findItem(R.id.mute_stories);
        if (muteStoriesMenuItem != null) {
            if (isNotMe) {
                muteStoriesMenuItem.setVisible(true);
                muteStoriesMenuItem.setTitle(profileModel.getFriendshipStatus().isMutingReel() ? R.string.mute_stories : R.string.unmute_stories);
            } else {
                muteStoriesMenuItem.setVisible(false);
            }
        }
        mutePostsMenuItem = menu.findItem(R.id.mute_posts);
        if (mutePostsMenuItem != null) {
            if (isNotMe) {
                mutePostsMenuItem.setVisible(true);
                mutePostsMenuItem.setTitle(profileModel.getFriendshipStatus().isMuting() ? R.string.mute_posts : R.string.unmute_posts);
            } else {
                mutePostsMenuItem.setVisible(false);
            }
        }
        chainingMenuItem = menu.findItem(R.id.chaining);
        if (chainingMenuItem != null) {
            chainingMenuItem.setVisible(isNotMe && profileModel.hasChaining());
        }
        removeFollowerMenuItem = menu.findItem(R.id.remove_follower);
        if (removeFollowerMenuItem != null) {
            removeFollowerMenuItem.setVisible(isNotMe && profileModel.getFriendshipStatus().isFollowedBy());
        }
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        if (item.getItemId() == R.id.layout) {
            showPostsLayoutPreferences();
            return true;
        }
        if (item.getItemId() == R.id.restrict) {
            if (!isLoggedIn) return false;
            final String action = profileModel.getFriendshipStatus().isRestricted() ? "Unrestrict" : "Restrict";
            friendshipService.toggleRestrict(
                    profileModel.getPk(),
                    !profileModel.getFriendshipStatus().isRestricted(),
                    new ServiceCallback<FriendshipRestrictResponse>() {
                        @Override
                        public void onSuccess(final FriendshipRestrictResponse result) {
                            Log.d(TAG, action + " success: " + result);
                            fetchProfileDetails();
                        }

                        @Override
                        public void onFailure(final Throwable t) {
                            Log.e(TAG, "Error while performing " + action, t);
                        }
                    });
            return true;
        }
        if (item.getItemId() == R.id.block) {
            if (!isLoggedIn) return false;
            friendshipService.changeBlock(profileModel.getFriendshipStatus().isBlocking(), profileModel.getPk(), changeCb);
            return true;
        }
        if (item.getItemId() == R.id.chaining) {
            if (!isLoggedIn) return false;
            final Bundle bundle = new Bundle();
            bundle.putString("type", "chaining");
            bundle.putLong("targetId", profileModel.getPk());
            NavHostFragment.findNavController(this).navigate(R.id.action_global_notificationsViewerFragment, bundle);
            return true;
        }
        if (item.getItemId() == R.id.mute_stories) {
            if (!isLoggedIn) return false;
            final String action = profileModel.getFriendshipStatus().isMutingReel() ? "Unmute stories" : "Mute stories";
            friendshipService.changeMute(
                    profileModel.getFriendshipStatus().isMutingReel(),
                    profileModel.getPk(),
                    true,
                    changeCb);
            return true;
        }
        if (item.getItemId() == R.id.mute_posts) {
            if (!isLoggedIn) return false;
            final String action = profileModel.getFriendshipStatus().isMuting() ? "Unmute stories" : "Mute stories";
            friendshipService.changeMute(
                    profileModel.getFriendshipStatus().isMuting(),
                    profileModel.getPk(),
                    false,
                    changeCb);
            return true;
        }
        if (item.getItemId() == R.id.remove_follower) {
            if (!isLoggedIn) return false;
            friendshipService.removeFollower(profileModel.getPk(), changeCb);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onRefresh() {
        profileDetailsBinding.countsDivider.getRoot().setVisibility(View.GONE);
        profileDetailsBinding.mainProfileImage.setVisibility(View.INVISIBLE);
        fetchProfileDetails();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (usernameSettingHandler != null) {
            usernameSettingHandler.removeCallbacks(usernameSettingRunnable);
        }
        if (highlightsViewModel != null) {
            highlightsViewModel.getList().postValue(Collections.emptyList());
        }
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
            binding.postsRecyclerView.endSelection();
        }
    }

    private void init() {
        disableDm = !Utils.isNavRootInCurrentTabs("direct_messages_nav_graph");
        if (getArguments() != null) {
            final ProfileFragmentArgs fragmentArgs = ProfileFragmentArgs.fromBundle(getArguments());
            username = fragmentArgs.getUsername();
            setUsernameDelayed();
        }
        if (TextUtils.isEmpty(username) && !isLoggedIn) {
            binding.header.getRoot().setVisibility(View.GONE);
            binding.swipeRefreshLayout.setEnabled(false);
            binding.privatePage1.setImageResource(R.drawable.ic_outline_info_24);
            binding.privatePage2.setText(R.string.no_acc);
            binding.privatePage.setVisibility(View.VISIBLE);
            return;
        }
        binding.swipeRefreshLayout.setEnabled(true);
        setupHighlights();
        setupCommonListeners();
        fetchProfileDetails();
    }

    private void fetchProfileDetails() {
        accountIsUpdated = false;
        String usernameTemp = username.trim();
        if (usernameTemp.startsWith("@")) {
            usernameTemp = usernameTemp.substring(1);
        }
        if (TextUtils.isEmpty(usernameTemp)) {
            appStateViewModel.getCurrentUserLiveData().observe(getViewLifecycleOwner(), user -> {
                if (user == null) return;
                profileModel = user;
                username = profileModel.getUsername();
                setUsernameDelayed();
                setProfileDetails();
            });
            return;
        }
        if (isLoggedIn) {
            userService.getUsernameInfo(usernameTemp, new ServiceCallback<User>() {
                @Override
                public void onSuccess(final User user) {
                    userService.getUserFriendship(user.getPk(), new ServiceCallback<FriendshipStatus>() {
                        @Override
                        public void onSuccess(final FriendshipStatus status) {
                            user.setFriendshipStatus(status);
                            profileModel = user;
                            setProfileDetails();
                        }

                        @Override
                        public void onFailure(final Throwable t) {
                            Log.e(TAG, "Error fetching profile relationship", t);
                            final Context context = getContext();
                            try {
                                if (t == null)
                                    Toast.makeText(context, R.string.error_loading_profile_loggedin, Toast.LENGTH_LONG).show();
                                else
                                    Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                            } catch (final Throwable ignored) {
                            }
                        }
                    });
                }

                @Override
                public void onFailure(final Throwable t) {
                    Log.e(TAG, "Error fetching profile", t);
                    final Context context = getContext();
                    try {
                        if (t == null)
                            Toast.makeText(context, R.string.error_loading_profile_loggedin, Toast.LENGTH_LONG).show();
                        else Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                    } catch (final Throwable ignored) {
                    }
                }
            });
            return;
        }
        graphQLService.fetchUser(usernameTemp, new ServiceCallback<User>() {
            @Override
            public void onSuccess(final User user) {
                profileModel = user;
                setProfileDetails();
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "Error fetching profile", t);
                final Context context = getContext();
                try {
                    if (t == null)
                        Toast.makeText(context, R.string.error_loading_profile, Toast.LENGTH_LONG).show();
                    else Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                } catch (final Throwable ignored) {
                }
            }
        });
    }

    private void setProfileDetails() {
        final Context context = getContext();
        if (context == null) return;
        if (profileModel == null) {
            binding.swipeRefreshLayout.setRefreshing(false);
            Toast.makeText(context, R.string.error_loading_profile, Toast.LENGTH_SHORT).show();
            return;
        }
        final long profileId = profileModel.getPk();
        if (!isReallyPrivate()) {
            if (!postsSetupDone) {
                setupPosts();
            } else {
                binding.postsRecyclerView.refresh();
            }
            if (isLoggedIn) {
                fetchStoryAndHighlights(profileId);
            }
        }

        profileDetailsBinding.isVerified.setVisibility(profileModel.isVerified() ? View.VISIBLE : View.GONE);
        profileDetailsBinding.isPrivate.setVisibility(profileModel.isPrivate() ? View.VISIBLE : View.GONE);

        setupButtons(profileId);
        final FavoriteRepository favoriteRepository = FavoriteRepository.getInstance(FavoriteDataSource.getInstance(getContext()));
        favoriteRepository.getFavorite(profileModel.getUsername(), FavoriteType.USER, new RepositoryCallback<Favorite>() {
            @Override
            public void onSuccess(final Favorite result) {
                profileDetailsBinding.favChip.setChipIconResource(R.drawable.ic_star_check_24);
                profileDetailsBinding.favChip.setText(R.string.favorite_short);
                favoriteRepository.insertOrUpdateFavorite(new Favorite(
                        result.getId(),
                        profileModel.getUsername(),
                        FavoriteType.USER,
                        profileModel.getFullName(),
                        profileModel.getProfilePicUrl(),
                        result.getDateAdded()
                ), new RepositoryCallback<Void>() {
                    @Override
                    public void onSuccess(final Void result) {
                    }

                    @Override
                    public void onDataNotAvailable() {
                    }
                });
            }

            @Override
            public void onDataNotAvailable() {
                profileDetailsBinding.favChip.setChipIconResource(R.drawable.ic_outline_star_plus_24);
                profileDetailsBinding.favChip.setText(R.string.add_to_favorites);
            }
        });
        profileDetailsBinding.favChip.setOnClickListener(
                v -> favoriteRepository.getFavorite(profileModel.getUsername(), FavoriteType.USER, new RepositoryCallback<Favorite>() {
                    @Override
                    public void onSuccess(final Favorite result) {
                        favoriteRepository.deleteFavorite(profileModel.getUsername(), FavoriteType.USER, new RepositoryCallback<Void>() {
                            @Override
                            public void onSuccess(final Void result) {
                                profileDetailsBinding.favChip.setText(R.string.add_to_favorites);
                                profileDetailsBinding.favChip.setChipIconResource(R.drawable.ic_outline_star_plus_24);
                                showSnackbar(getString(R.string.removed_from_favs));
                            }

                            @Override
                            public void onDataNotAvailable() {
                            }
                        });
                    }

                    @Override
                    public void onDataNotAvailable() {
                        favoriteRepository.insertOrUpdateFavorite(new Favorite(
                                0,
                                profileModel.getUsername(),
                                FavoriteType.USER,
                                profileModel.getFullName(),
                                profileModel.getProfilePicUrl(),
                                new Date()
                        ), new RepositoryCallback<Void>() {
                            @Override
                            public void onSuccess(final Void result) {
                                profileDetailsBinding.favChip.setText(R.string.favorite_short);
                                profileDetailsBinding.favChip.setChipIconResource(R.drawable.ic_star_check_24);
                                showSnackbar(getString(R.string.added_to_favs));
                            }

                            @Override
                            public void onDataNotAvailable() {
                            }
                        });
                    }
                }));
        profileDetailsBinding.mainProfileImage.setImageURI(profileModel.getProfilePicUrl());
        profileDetailsBinding.mainProfileImage.setVisibility(View.VISIBLE);

        profileDetailsBinding.countsDivider.getRoot().setVisibility(View.VISIBLE);

        final long followersCount = profileModel.getFollowerCount();
        final long followingCount = profileModel.getFollowingCount();

        final String postCount = String.valueOf(profileModel.getMediaCount());

        SpannableStringBuilder span = new SpannableStringBuilder(getResources().getQuantityString(
                R.plurals.main_posts_count,
                profileModel.getMediaCount() > 2000000000L ? 2000000000 : (int) profileModel.getMediaCount(),
                postCount)
        );
        span.setSpan(new RelativeSizeSpan(1.2f), 0, postCount.length(), 0);
        span.setSpan(new StyleSpan(Typeface.BOLD), 0, postCount.length(), 0);
        profileDetailsBinding.mainPostCount.setText(span);
        profileDetailsBinding.mainPostCount.setVisibility(View.VISIBLE);

        final String followersCountStr = String.valueOf(followersCount);
        final int followersCountStrLen = followersCountStr.length();
        span = new SpannableStringBuilder(getResources().getQuantityString(
                R.plurals.main_posts_followers,
                followersCount > 2000000000L ? 2000000000 : (int) followersCount,
                followersCountStr)
        );
        span.setSpan(new RelativeSizeSpan(1.2f), 0, followersCountStrLen, 0);
        span.setSpan(new StyleSpan(Typeface.BOLD), 0, followersCountStrLen, 0);
        profileDetailsBinding.mainFollowers.setText(span);
        profileDetailsBinding.mainFollowers.setVisibility(View.VISIBLE);

        final String followingCountStr = String.valueOf(followingCount);
        final int followingCountStrLen = followingCountStr.length();
        span = new SpannableStringBuilder(getString(R.string.main_posts_following, followingCountStr));
        span.setSpan(new RelativeSizeSpan(1.2f), 0, followingCountStrLen, 0);
        span.setSpan(new StyleSpan(Typeface.BOLD), 0, followingCountStrLen, 0);
        profileDetailsBinding.mainFollowing.setText(span);
        profileDetailsBinding.mainFollowing.setVisibility(View.VISIBLE);

        profileDetailsBinding.mainFullName.setText(TextUtils.isEmpty(profileModel.getFullName()) ? profileModel.getUsername()
                                                                                                 : profileModel.getFullName());

        final String biography = profileModel.getBiography();
        if (TextUtils.isEmpty(biography)) {
            profileDetailsBinding.mainBiography.setVisibility(View.GONE);
        } else {
            profileDetailsBinding.mainBiography.setVisibility(View.VISIBLE);
            profileDetailsBinding.mainBiography.setText(biography);
            profileDetailsBinding.mainBiography.addOnHashtagListener(autoLinkItem -> {
                final NavController navController = NavHostFragment.findNavController(this);
                final Bundle bundle = new Bundle();
                final String originalText = autoLinkItem.getOriginalText().trim();
                bundle.putString(ARG_HASHTAG, originalText);
                navController.navigate(R.id.action_global_hashTagFragment, bundle);
            });
            profileDetailsBinding.mainBiography.addOnMentionClickListener(autoLinkItem -> {
                final String originalText = autoLinkItem.getOriginalText().trim();
                navigateToProfile(originalText);
            });
            profileDetailsBinding.mainBiography.addOnEmailClickListener(autoLinkItem -> Utils.openEmailAddress(
                    getContext(), autoLinkItem.getOriginalText().trim()));
            profileDetailsBinding.mainBiography
                    .addOnURLClickListener(autoLinkItem -> Utils.openURL(getContext(), autoLinkItem.getOriginalText().trim()));
            profileDetailsBinding.mainBiography.setOnLongClickListener(v -> {
                String[] commentDialogList;
                if (!TextUtils.isEmpty(cookie)) {
                    commentDialogList = new String[]{
                            getResources().getString(R.string.bio_copy),
                            getResources().getString(R.string.bio_translate)
                    };
                } else {
                    commentDialogList = new String[]{
                            getResources().getString(R.string.bio_copy)
                    };
                }
                new AlertDialog.Builder(context)
                        .setItems(commentDialogList, (d, w) -> {
                            switch (w) {
                                case 0:
                                    Utils.copyText(context, biography);
                                    break;
                                case 1:
                                    mediaService.translate(String.valueOf(profileModel.getPk()), "3", new ServiceCallback<String>() {
                                        @Override
                                        public void onSuccess(final String result) {
                                            if (TextUtils.isEmpty(result)) {
                                                Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                                                return;
                                            }
                                            new AlertDialog.Builder(context)
                                                    .setTitle(profileModel.getUsername())
                                                    .setMessage(result)
                                                    .setPositiveButton(R.string.ok, null)
                                                    .show();
                                        }

                                        @Override
                                        public void onFailure(final Throwable t) {
                                            Log.e(TAG, "Error translating bio", t);
                                            Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                                        }
                                    });
                                    break;
                            }
                        })
                        .setNegativeButton(R.string.cancel, null)
                        .show();
                return true;
            });
        }

        String profileContext = profileModel.getProfileContext();
        if (TextUtils.isEmpty(profileContext)) {
            profileDetailsBinding.profileContext.setVisibility(View.GONE);
        } else {
            profileDetailsBinding.profileContext.setVisibility(View.VISIBLE);
            final List<UserProfileContextLink> userProfileContextLinks = profileModel.getProfileContextLinks();
            for (int i = 0; i < userProfileContextLinks.size(); i++) {
                final UserProfileContextLink link = userProfileContextLinks.get(i);
                if (link.getUsername() != null)
                    profileContext = profileContext.substring(0, link.getStart() + i)
                            + "@" + profileContext.substring(link.getStart() + i);
            }
            profileDetailsBinding.profileContext.setText(profileContext);
            profileDetailsBinding.profileContext.addOnMentionClickListener(autoLinkItem -> {
                final String originalText = autoLinkItem.getOriginalText().trim();
                navigateToProfile(originalText);
            });
        }

        final String url = profileModel.getExternalUrl();
        if (TextUtils.isEmpty(url)) {
            profileDetailsBinding.mainUrl.setVisibility(View.GONE);
        } else {
            profileDetailsBinding.mainUrl.setVisibility(View.VISIBLE);
            profileDetailsBinding.mainUrl.setText(url);
            profileDetailsBinding.mainUrl.addOnURLClickListener(autoLinkItem -> Utils.openURL(getContext(), autoLinkItem.getOriginalText().trim()));
            profileDetailsBinding.mainUrl.setOnLongClickListener(v -> {
                Utils.copyText(context, url);
                return true;
            });
        }
        final MotionScene.Transition transition = root.getTransition(R.id.transition);
        if (!isReallyPrivate()) {
            if (isLoggedIn) {
                profileDetailsBinding.mainFollowing.setClickable(true);
                profileDetailsBinding.mainFollowers.setClickable(true);
                final View.OnClickListener followClickListener = v -> {
                    final NavDirections action = ProfileFragmentDirections.actionProfileFragmentToFollowViewerFragment(
                            profileId,
                            v == profileDetailsBinding.mainFollowers,
                            profileModel.getUsername());
                    NavHostFragment.findNavController(this).navigate(action);
                };
                profileDetailsBinding.mainFollowers.setOnClickListener(followersCount > 0 ? followClickListener : null);
                profileDetailsBinding.mainFollowing.setOnClickListener(followingCount > 0 ? followClickListener : null);
            }
            binding.postsRecyclerView.setVisibility(View.VISIBLE);
        } else {
            profileDetailsBinding.mainFollowers.setClickable(false);
            profileDetailsBinding.mainFollowing.setClickable(false);
            binding.privatePage1.setImageResource(R.drawable.lock);
            binding.privatePage2.setText(R.string.priv_acc);
            binding.privatePage.setVisibility(View.VISIBLE);
            binding.privatePage1.setVisibility(View.VISIBLE);
            binding.privatePage2.setVisibility(View.VISIBLE);
            binding.postsRecyclerView.setVisibility(View.GONE);
            binding.swipeRefreshLayout.setRefreshing(false);
            if (transition != null) {
                transition.setEnable(false);
            }
        }
        if (profileModel.getMediaCount() == 0 && transition != null) {
            transition.setEnable(false);
        }
    }

    private void setupButtons(final long profileId) {
        profileDetailsBinding.btnTagged.setVisibility(isReallyPrivate() ? View.GONE : View.VISIBLE);
        if (isLoggedIn) {
            if (Objects.equals(profileId, myId)) {
                profileDetailsBinding.btnTagged.setVisibility(View.VISIBLE);
                profileDetailsBinding.btnSaved.setVisibility(View.VISIBLE);
                profileDetailsBinding.btnLiked.setVisibility(View.VISIBLE);
                profileDetailsBinding.btnDM.setVisibility(View.GONE);
                profileDetailsBinding.favChip.setVisibility(View.GONE);
                profileDetailsBinding.btnSaved.setText(R.string.saved);
                if (!accountIsUpdated) updateAccountInfo();
                return;
            }
            profileDetailsBinding.btnSaved.setVisibility(View.GONE);
            profileDetailsBinding.btnLiked.setVisibility(View.GONE);
            profileDetailsBinding.btnDM.setVisibility(disableDm ? View.GONE : View.VISIBLE);
            profileDetailsBinding.btnFollow.setVisibility(View.VISIBLE);
            profileDetailsBinding.favChip.setVisibility(View.VISIBLE);
            final Context context = getContext();
            if (context == null) return;
            if (profileModel.getFriendshipStatus().isFollowing() || profileModel.getFriendshipStatus().isFollowedBy()) {
                profileDetailsBinding.mainStatus.setVisibility(View.VISIBLE);
                if (!profileModel.getFriendshipStatus().isFollowing()) {
                    profileDetailsBinding.mainStatus.setChipBackgroundColor(AppCompatResources.getColorStateList(context, R.color.blue_800));
                    profileDetailsBinding.mainStatus.setText(R.string.status_follower);
                } else if (!profileModel.getFriendshipStatus().isFollowedBy()) {
                    profileDetailsBinding.mainStatus.setChipBackgroundColor(AppCompatResources.getColorStateList(context, R.color.deep_orange_800));
                    profileDetailsBinding.mainStatus.setText(R.string.status_following);
                } else {
                    profileDetailsBinding.mainStatus.setChipBackgroundColor(AppCompatResources.getColorStateList(context, R.color.green_800));
                    profileDetailsBinding.mainStatus.setText(R.string.status_mutual);
                }
            }
            else profileDetailsBinding.mainStatus.setVisibility(View.GONE);
            if (profileModel.getFriendshipStatus().isFollowing()) {
                profileDetailsBinding.btnFollow.setText(R.string.unfollow);
                profileDetailsBinding.btnFollow.setChipIconResource(R.drawable.ic_outline_person_add_disabled_24);
            } else if (profileModel.getFriendshipStatus().isOutgoingRequest()) {
                profileDetailsBinding.btnFollow.setText(R.string.cancel);
                profileDetailsBinding.btnFollow.setChipIconResource(R.drawable.ic_outline_person_add_disabled_24);
            } else {
                profileDetailsBinding.btnFollow.setText(R.string.follow);
                profileDetailsBinding.btnFollow.setChipIconResource(R.drawable.ic_outline_person_add_24);
            }
            if (restrictMenuItem != null) {
                restrictMenuItem.setVisible(true);
                restrictMenuItem.setTitle(profileModel.getFriendshipStatus().isRestricted() ? R.string.unrestrict : R.string.restrict);
            }
            if (blockMenuItem != null) {
                blockMenuItem.setVisible(true);
                blockMenuItem.setTitle(profileModel.getFriendshipStatus().isBlocking() ? R.string.unblock : R.string.block);
            }
            if (muteStoriesMenuItem != null) {
                muteStoriesMenuItem.setVisible(true);
                muteStoriesMenuItem.setTitle(profileModel.getFriendshipStatus().isMutingReel() ? R.string.unmute_stories : R.string.mute_stories);
            }
            if (mutePostsMenuItem != null) {
                mutePostsMenuItem.setVisible(true);
                mutePostsMenuItem.setTitle(profileModel.getFriendshipStatus().isMuting() ? R.string.unmute_posts : R.string.mute_posts);
            }
            if (chainingMenuItem != null) {
                chainingMenuItem.setVisible(profileModel.hasChaining());
            }
            if (removeFollowerMenuItem != null) {
                removeFollowerMenuItem.setVisible(profileModel.getFriendshipStatus().isFollowedBy());
            }
        }
    }

    private void updateAccountInfo() {
        if (profileModel == null) return;
        accountRepository.insertOrUpdateAccount(
                profileModel.getPk(),
                profileModel.getUsername(),
                cookie,
                profileModel.getFullName(),
                profileModel.getProfilePicUrl(),
                new RepositoryCallback<Account>() {
                    @Override
                    public void onSuccess(final Account result) {
                        accountIsUpdated = true;
                    }

                    @Override
                    public void onDataNotAvailable() {
                        Log.e(TAG, "onDataNotAvailable: insert failed");
                    }
                });
    }

    private void fetchStoryAndHighlights(final long profileId) {
        storiesService.getUserStory(
                StoryViewerOptions.forUser(profileId, profileModel.getFullName()),
                new ServiceCallback<List<StoryModel>>() {
                    @Override
                    public void onSuccess(final List<StoryModel> storyModels) {
                        if (storyModels != null && !storyModels.isEmpty()) {
                            profileDetailsBinding.mainProfileImage.setStoriesBorder(1);
                            hasStories = true;
                        }
                    }

                    @Override
                    public void onFailure(final Throwable t) {
                        Log.e(TAG, "Error", t);
                    }
                });
        storiesService.fetchHighlights(profileId,
                                       new ServiceCallback<List<HighlightModel>>() {
                                           @Override
                                           public void onSuccess(final List<HighlightModel> result) {
                                               if (result != null) {
                                                   profileDetailsBinding.highlightsList.setVisibility(View.VISIBLE);
                                                   highlightsViewModel.getList().postValue(result);
                                               } else profileDetailsBinding.highlightsList.setVisibility(View.GONE);
                                           }

                                           @Override
                                           public void onFailure(final Throwable t) {
                                               profileDetailsBinding.highlightsList.setVisibility(View.GONE);
                                               Log.e(TAG, "Error", t);
                                           }
                                       });
    }

    private void setupCommonListeners() {
        final Context context = getContext();
        profileDetailsBinding.btnFollow.setOnClickListener(v -> {
            if (profileModel.getFriendshipStatus().isFollowing() && profileModel.isPrivate()) {
                new AlertDialog.Builder(context)
                        .setTitle(R.string.priv_acc)
                        .setMessage(R.string.priv_acc_confirm)
                        .setPositiveButton(R.string.confirm, (d, w) ->
                                friendshipService.unfollow(profileModel.getPk(), changeCb))
                        .setNegativeButton(R.string.cancel, null)
                        .show();
            } else if (profileModel.getFriendshipStatus().isFollowing() || profileModel.getFriendshipStatus().isOutgoingRequest()) {
                friendshipService.unfollow(profileModel.getPk(), changeCb);
            } else {
                friendshipService.follow(profileModel.getPk(), changeCb);
            }
        });
        profileDetailsBinding.btnSaved.setOnClickListener(v -> {
            final NavDirections action = ProfileFragmentDirections.actionGlobalSavedCollectionsFragment(false);
            NavHostFragment.findNavController(this).navigate(action);
        });
        profileDetailsBinding.btnLiked.setOnClickListener(v -> {
            final NavDirections action = ProfileFragmentDirections.actionProfileFragmentToSavedViewerFragment(profileModel.getUsername(),
                                                                                                              profileModel.getPk(),
                                                                                                              PostItemType.LIKED);
            NavHostFragment.findNavController(this).navigate(action);
        });
        profileDetailsBinding.btnTagged.setOnClickListener(v -> {
            final NavDirections action = ProfileFragmentDirections.actionProfileFragmentToSavedViewerFragment(profileModel.getUsername(),
                                                                                                              profileModel.getPk(),
                                                                                                              PostItemType.TAGGED);
            NavHostFragment.findNavController(this).navigate(action);
        });
        if (!disableDm) {
            profileDetailsBinding.btnDM.setOnClickListener(v -> {
                profileDetailsBinding.btnDM.setEnabled(false);
                final Call<DirectThread> createThreadRequest =
                        directMessagesService.createThread(Collections.singletonList(profileModel.getPk()), null);
                createThreadRequest.enqueue(new Callback<DirectThread>() {
                    @Override
                    public void onResponse(@NonNull final Call<DirectThread> call, @NonNull final Response<DirectThread> response) {
                        profileDetailsBinding.btnDM.setEnabled(true);
                        if (!response.isSuccessful() || response.body() == null) {
                            Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                            return;
                        }
                        final InboxManager inboxManager = DirectMessagesManager.getInstance().getInboxManager();
                        final DirectThread thread = response.body();
                        if (!inboxManager.containsThread(thread.getThreadId())) {
                            thread.setTemp(true);
                            inboxManager.addThread(thread, 0);
                        }
                        fragmentActivity.navigateToThread(thread.getThreadId(), profileModel.getUsername());
                    }

                    @Override
                    public void onFailure(@NonNull final Call<DirectThread> call, @NonNull final Throwable t) {
                        Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                    }
                });
            });
        }
        profileDetailsBinding.mainProfileImage.setOnClickListener(v -> {
            if (!hasStories) {
                // show profile pic
                showProfilePicDialog();
                return;
            }
            // show dialog
            final String[] options = {getString(R.string.view_pfp), getString(R.string.show_stories)};
            final DialogInterface.OnClickListener profileDialogListener = (dialog, which) -> {
                if (which == AlertDialog.BUTTON_NEUTRAL) {
                    dialog.dismiss();
                    return;
                }
                if (which == 1) {
                    // show stories
                    final NavDirections action = ProfileFragmentDirections
                            .actionProfileFragmentToStoryViewerFragment(StoryViewerOptions.forUser(profileModel.getPk(),
                                                                                                   profileModel.getFullName()));
                    NavHostFragment.findNavController(this).navigate(action);
                    return;
                }
                showProfilePicDialog();
            };
            if (context == null) return;
            new AlertDialog.Builder(context)
                    .setItems(options, profileDialogListener)
                    .setNegativeButton(R.string.cancel, null)
                    .show();
        });
    }

    private void showSnackbar(final String message) {
        final Snackbar snackbar = Snackbar.make(root, message, BaseTransientBottomBar.LENGTH_LONG);
        snackbar.setAction(R.string.ok, v -> snackbar.dismiss())
                .setAnimationMode(BaseTransientBottomBar.ANIMATION_MODE_SLIDE)
                .setAnchorView(fragmentActivity.getBottomNavView())
                .show();
    }

    private void showProfilePicDialog() {
        if (profileModel != null) {
            final FragmentManager fragmentManager = getParentFragmentManager();
            final ProfilePicDialogFragment fragment = ProfilePicDialogFragment.getInstance(profileModel.getPk(),
                                                                                           username,
                                                                                           profileModel.getProfilePicUrl());
            final FragmentTransaction ft = fragmentManager.beginTransaction();
            ft.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_OPEN)
              .add(fragment, "profilePicDialog")
              .commit();
        }
    }

    private void setUsernameDelayed() {
        if (usernameSettingHandler == null) {
            usernameSettingHandler = new Handler(Looper.getMainLooper());
        }
        usernameSettingHandler.postDelayed(usernameSettingRunnable, 200);
    }

    private void setupPosts() {
        binding.postsRecyclerView.setViewModelStoreOwner(this)
                                 .setLifeCycleOwner(this)
                                 .setPostFetchService(new ProfilePostFetchService(profileModel, isLoggedIn))
                                 .setLayoutPreferences(layoutPreferences)
                                 .addFetchStatusChangeListener(fetching -> updateSwipeRefreshState())
                                 .setFeedItemCallback(feedItemCallback)
                                 .setSelectionModeCallback(selectionModeCallback)
                                 .init();
        binding.postsRecyclerView.addOnScrollListener(new RecyclerView.OnScrollListener() {
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
        binding.swipeRefreshLayout.setRefreshing(true);
        postsSetupDone = true;
    }

    private void updateSwipeRefreshState() {
        binding.swipeRefreshLayout.setRefreshing(binding.postsRecyclerView.isFetching());
    }

    private void setupHighlights() {
        highlightsViewModel = new ViewModelProvider(fragmentActivity).get(HighlightsViewModel.class);
        highlightsAdapter = new HighlightsAdapter((model, position) -> {
            final StoryViewerOptions options = StoryViewerOptions.forHighlight(model.getTitle());
            options.setCurrentFeedStoryIndex(position);
            final NavDirections action = ProfileFragmentDirections.actionProfileFragmentToStoryViewerFragment(options);
            NavHostFragment.findNavController(this).navigate(action);
        });
        final Context context = getContext();
        if (context == null) return;
        final RecyclerView.LayoutManager layoutManager = new LinearLayoutManager(context, RecyclerView.HORIZONTAL, false);
        profileDetailsBinding.highlightsList.setLayoutManager(layoutManager);
        profileDetailsBinding.highlightsList.setAdapter(highlightsAdapter);
        highlightsViewModel.getList().observe(getViewLifecycleOwner(), highlightModels -> highlightsAdapter.submitList(highlightModels));
    }

    private void navigateToProfile(final String username) {
        final NavController navController = NavHostFragment.findNavController(this);
        final Bundle bundle = new Bundle();
        bundle.putString("username", username);
        navController.navigate(R.id.action_global_profileFragment, bundle);
    }

    private void showPostsLayoutPreferences() {
        final PostsLayoutPreferencesDialogFragment fragment = new PostsLayoutPreferencesDialogFragment(
                Constants.PREF_PROFILE_POSTS_LAYOUT,
                preferences -> {
                    layoutPreferences = preferences;
                    new Handler().postDelayed(() -> binding.postsRecyclerView.setLayoutPreferences(preferences), 200);
                });
        fragment.show(getChildFragmentManager(), "posts_layout_preferences");
    }

    private boolean isReallyPrivate() {
        if (profileModel.getPk() == myId) return false;
        final FriendshipStatus friendshipStatus = profileModel.getFriendshipStatus();
        return !friendshipStatus.isFollowing() && profileModel.isPrivate();
    }
}
