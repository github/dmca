package awais.instagrabber.fragments;

import android.animation.ArgbEvaluator;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.ActionMode;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

import androidx.activity.OnBackPressedCallback;
import androidx.activity.OnBackPressedDispatcher;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.core.content.PermissionChecker;
import androidx.core.graphics.ColorUtils;
import androidx.fragment.app.Fragment;
import androidx.navigation.NavController;
import androidx.navigation.NavDirections;
import androidx.navigation.fragment.NavHostFragment;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;
import androidx.transition.ChangeBounds;
import androidx.transition.TransitionInflater;
import androidx.transition.TransitionSet;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.drawee.controller.BaseControllerListener;
import com.facebook.drawee.interfaces.DraweeController;
import com.facebook.imagepipeline.image.ImageInfo;
import com.google.common.collect.ImmutableList;

import java.util.Set;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.FeedAdapterV2;
import awais.instagrabber.asyncs.DiscoverPostFetchService;
import awais.instagrabber.customviews.PrimaryActionModeCallback;
import awais.instagrabber.databinding.FragmentTopicPostsBinding;
import awais.instagrabber.dialogs.PostsLayoutPreferencesDialogFragment;
import awais.instagrabber.fragments.main.DiscoverFragmentDirections;
import awais.instagrabber.models.PostsLayoutPreferences;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.discover.TopicCluster;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.DownloadUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.webservices.DiscoverService;

import static androidx.core.content.PermissionChecker.checkSelfPermission;
import static awais.instagrabber.utils.DownloadUtils.WRITE_PERMISSION;

public class TopicPostsFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = TopicPostsFragment.class.getSimpleName();
    private static final int STORAGE_PERM_REQUEST_CODE = 8020;
    private static final int STORAGE_PERM_REQUEST_CODE_FOR_SELECTION = 8030;

    private MainActivity fragmentActivity;
    private FragmentTopicPostsBinding binding;
    private CoordinatorLayout root;
    private boolean shouldRefresh = true;
    private TopicCluster topicCluster;
    private ActionMode actionMode;
    private Set<Media> selectedFeedModels;
    private Media downloadFeedModel;
    private int downloadChildPosition = -1;
    private PostsLayoutPreferences layoutPreferences = Utils.getPostsLayoutPreferences(Constants.PREF_TOPIC_POSTS_LAYOUT);

    private final OnBackPressedCallback onBackPressedCallback = new OnBackPressedCallback(false) {
        @Override
        public void handleOnBackPressed() {
            binding.posts.endSelection();
        }
    };
    private final PrimaryActionModeCallback multiSelectAction = new PrimaryActionModeCallback(
            R.menu.multi_select_download_menu, new PrimaryActionModeCallback.CallbacksHelper() {
        @Override
        public void onDestroy(final ActionMode mode) {
            binding.posts.endSelection();
        }

        @Override
        public boolean onActionItemClicked(final ActionMode mode,
                                           final MenuItem item) {
            if (item.getItemId() == R.id.action_download) {
                if (TopicPostsFragment.this.selectedFeedModels == null) return false;
                final Context context = getContext();
                if (context == null) return false;
                if (checkSelfPermission(context, WRITE_PERMISSION) == PermissionChecker.PERMISSION_GRANTED) {
                    DownloadUtils.download(context, ImmutableList.copyOf(TopicPostsFragment.this.selectedFeedModels));
                    binding.posts.endSelection();
                    return true;
                }
                requestPermissions(DownloadUtils.PERMS, STORAGE_PERM_REQUEST_CODE_FOR_SELECTION);
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
            final NavDirections commentsAction = DiscoverFragmentDirections.actionGlobalCommentsViewerFragment(
                    feedModel.getCode(),
                    feedModel.getPk(),
                    feedModel.getUser().getPk()
            );
            NavHostFragment.findNavController(TopicPostsFragment.this).navigate(commentsAction);
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
            downloadChildPosition = -1;
            requestPermissions(DownloadUtils.PERMS, STORAGE_PERM_REQUEST_CODE);
        }

        @Override
        public void onHashtagClick(final String hashtag) {
            final NavDirections action = DiscoverFragmentDirections.actionGlobalHashTagFragment(hashtag);
            NavHostFragment.findNavController(TopicPostsFragment.this).navigate(action);
        }

        @Override
        public void onLocationClick(final Media feedModel) {
            final NavDirections action = DiscoverFragmentDirections.actionGlobalLocationFragment(feedModel.getLocation().getPk());
            NavHostFragment.findNavController(TopicPostsFragment.this).navigate(action);
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
            final NavController navController = NavHostFragment.findNavController(TopicPostsFragment.this);
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
            TopicPostsFragment.this.selectedFeedModels = selectedFeedModels;
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

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = (MainActivity) requireActivity();
        final TransitionSet transitionSet = new TransitionSet();
        transitionSet.addTransition(new ChangeBounds())
                     .addTransition(TransitionInflater.from(getContext()).inflateTransition(android.R.transition.move))
                     .setDuration(200);
        setSharedElementEnterTransition(transitionSet);
        postponeEnterTransition();
        setHasOptionsMenu(true);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             @Nullable final ViewGroup container,
                             @Nullable final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentTopicPostsBinding.inflate(inflater, container, false);
        root = binding.getRoot();
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
    public void onResume() {
        super.onResume();
        fragmentActivity.setToolbar(binding.toolbar);
    }

    @Override
    public void onRefresh() {
        binding.posts.refresh();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        resetToolbar();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        resetToolbar();
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

    private void resetToolbar() {
        fragmentActivity.resetToolbar();
    }

    private void init() {
        if (getArguments() == null) return;
        final TopicPostsFragmentArgs fragmentArgs = TopicPostsFragmentArgs.fromBundle(getArguments());
        topicCluster = fragmentArgs.getTopicCluster();
        setupToolbar(fragmentArgs.getTitleColor(), fragmentArgs.getBackgroundColor());
        setupPosts();
    }

    private void setupToolbar(final int titleColor, final int backgroundColor) {
        if (topicCluster == null) {
            return;
        }
        binding.cover.setTransitionName("cover-" + topicCluster.getId());
        fragmentActivity.setToolbar(binding.toolbar);
        binding.collapsingToolbarLayout.setTitle(topicCluster.getTitle());
        final int collapsedTitleTextColor = ColorUtils.setAlphaComponent(titleColor, 0xFF);
        final int expandedTitleTextColor = ColorUtils.setAlphaComponent(titleColor, 0x99);
        binding.collapsingToolbarLayout.setExpandedTitleColor(expandedTitleTextColor);
        binding.collapsingToolbarLayout.setCollapsedTitleTextColor(collapsedTitleTextColor);
        binding.collapsingToolbarLayout.setContentScrimColor(backgroundColor);
        final Drawable navigationIcon = binding.toolbar.getNavigationIcon();
        final Drawable overflowIcon = binding.toolbar.getOverflowIcon();
        if (navigationIcon != null && overflowIcon != null) {
            final Drawable navDrawable = navigationIcon.mutate();
            final Drawable overflowDrawable = overflowIcon.mutate();
            navDrawable.setAlpha(0xFF);
            overflowDrawable.setAlpha(0xFF);
            final ArgbEvaluator argbEvaluator = new ArgbEvaluator();
            binding.appBarLayout.addOnOffsetChangedListener((appBarLayout, verticalOffset) -> {
                final int totalScrollRange = appBarLayout.getTotalScrollRange();
                final float current = totalScrollRange + verticalOffset;
                final float fraction = current / totalScrollRange;
                final int tempColor = (int) argbEvaluator.evaluate(fraction, collapsedTitleTextColor, expandedTitleTextColor);
                navDrawable.setColorFilter(tempColor, PorterDuff.Mode.SRC_ATOP);
                overflowDrawable.setColorFilter(tempColor, PorterDuff.Mode.SRC_ATOP);

            });
        }
        final GradientDrawable gd = new GradientDrawable(
                GradientDrawable.Orientation.TOP_BOTTOM,
                new int[]{Color.TRANSPARENT, backgroundColor});
        binding.background.setBackground(gd);
        setupCover();
    }

    private void setupCover() {
        final String coverUrl = ResponseBodyUtils.getImageUrl(topicCluster.getCoverMedia());
        final DraweeController controller = Fresco
                .newDraweeControllerBuilder()
                .setOldController(binding.cover.getController())
                .setUri(coverUrl)
                .setControllerListener(new BaseControllerListener<ImageInfo>() {

                    @Override
                    public void onFailure(final String id, final Throwable throwable) {
                        super.onFailure(id, throwable);
                        startPostponedEnterTransition();
                    }

                    @Override
                    public void onFinalImageSet(final String id,
                                                @Nullable final ImageInfo imageInfo,
                                                @Nullable final Animatable animatable) {
                        startPostponedEnterTransition();
                    }
                })
                .build();
        binding.cover.setController(controller);
    }

    private void setupPosts() {
        final DiscoverService.TopicalExploreRequest topicalExploreRequest = new DiscoverService.TopicalExploreRequest();
        topicalExploreRequest.setClusterId(topicCluster.getId());
        binding.posts.setViewModelStoreOwner(this)
                     .setLifeCycleOwner(this)
                     .setPostFetchService(new DiscoverPostFetchService(topicalExploreRequest))
                     .setLayoutPreferences(layoutPreferences)
                     .addFetchStatusChangeListener(fetching -> updateSwipeRefreshState())
                     .setFeedItemCallback(feedItemCallback)
                     .setSelectionModeCallback(selectionModeCallback)
                     .init();
        binding.swipeRefreshLayout.setRefreshing(true);
    }

    private void updateSwipeRefreshState() {
        binding.swipeRefreshLayout.setRefreshing(binding.posts.isFetching());
    }

    private void navigateToProfile(final String username) {
        final NavController navController = NavHostFragment.findNavController(this);
        final Bundle bundle = new Bundle();
        bundle.putString("username", username);
        navController.navigate(R.id.action_global_profileFragment, bundle);
    }

    private void showPostsLayoutPreferences() {
        final PostsLayoutPreferencesDialogFragment fragment = new PostsLayoutPreferencesDialogFragment(
                Constants.PREF_TOPIC_POSTS_LAYOUT,
                preferences -> {
                    layoutPreferences = preferences;
                    new Handler().postDelayed(() -> binding.posts.setLayoutPreferences(preferences), 200);
                });
        fragment.show(getChildFragmentManager(), "posts_layout_preferences");
    }
}
