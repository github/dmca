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
import android.widget.EditText;
import android.widget.Toast;

import androidx.activity.OnBackPressedCallback;
import androidx.activity.OnBackPressedDispatcher;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
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
import awais.instagrabber.asyncs.SavedPostFetchService;
import awais.instagrabber.customviews.PrimaryActionModeCallback;
import awais.instagrabber.databinding.FragmentCollectionPostsBinding;
import awais.instagrabber.dialogs.PostsLayoutPreferencesDialogFragment;
import awais.instagrabber.models.PostsLayoutPreferences;
import awais.instagrabber.models.enums.PostItemType;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.saved.SavedCollection;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.DownloadUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.webservices.CollectionService;
import awais.instagrabber.webservices.ServiceCallback;

import static androidx.core.content.PermissionChecker.checkSelfPermission;
import static awais.instagrabber.utils.DownloadUtils.WRITE_PERMISSION;

public class CollectionPostsFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = "CollectionPostsFragment";
    private static final int STORAGE_PERM_REQUEST_CODE = 8020;
    private static final int STORAGE_PERM_REQUEST_CODE_FOR_SELECTION = 8030;

    private MainActivity fragmentActivity;
    private FragmentCollectionPostsBinding binding;
    private CoordinatorLayout root;
    private boolean shouldRefresh = true;
    private SavedCollection savedCollection;
    private ActionMode actionMode;
    private Set<Media> selectedFeedModels;
    private Media downloadFeedModel;
    private int downloadChildPosition = -1;
    private CollectionService collectionService;
    private PostsLayoutPreferences layoutPreferences = Utils.getPostsLayoutPreferences(Constants.PREF_SAVED_POSTS_LAYOUT);

    private final OnBackPressedCallback onBackPressedCallback = new OnBackPressedCallback(false) {
        @Override
        public void handleOnBackPressed() {
            binding.posts.endSelection();
        }
    };
    private final PrimaryActionModeCallback multiSelectAction = new PrimaryActionModeCallback(
            R.menu.saved_collection_select_menu, new PrimaryActionModeCallback.CallbacksHelper() {
        @Override
        public void onDestroy(final ActionMode mode) {
            binding.posts.endSelection();
        }

        @Override
        public boolean onActionItemClicked(final ActionMode mode,
                                           final MenuItem item) {
            if (item.getItemId() == R.id.action_download) {
                if (CollectionPostsFragment.this.selectedFeedModels == null) return false;
                final Context context = getContext();
                if (context == null) return false;
                if (checkSelfPermission(context, WRITE_PERMISSION) == PermissionChecker.PERMISSION_GRANTED) {
                    DownloadUtils.download(context, ImmutableList.copyOf(CollectionPostsFragment.this.selectedFeedModels));
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
            final NavDirections commentsAction = CollectionPostsFragmentDirections.actionGlobalCommentsViewerFragment(
                    feedModel.getCode(),
                    feedModel.getPk(),
                    feedModel.getUser().getPk()
            );
            NavHostFragment.findNavController(CollectionPostsFragment.this).navigate(commentsAction);
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
            final NavDirections action = CollectionPostsFragmentDirections.actionGlobalHashTagFragment(hashtag);
            NavHostFragment.findNavController(CollectionPostsFragment.this).navigate(action);
        }

        @Override
        public void onLocationClick(final Media feedModel) {
            final NavDirections action = CollectionPostsFragmentDirections.actionGlobalLocationFragment(feedModel.getLocation().getPk());
            NavHostFragment.findNavController(CollectionPostsFragment.this).navigate(action);
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
            final NavController navController = NavHostFragment.findNavController(CollectionPostsFragment.this);
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
            CollectionPostsFragment.this.selectedFeedModels = selectedFeedModels;
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
        final Context context = getContext();
        if (context == null) return;
        transitionSet.addTransition(new ChangeBounds())
                     .addTransition(TransitionInflater.from(context).inflateTransition(android.R.transition.move))
                     .setDuration(200);
        setSharedElementEnterTransition(transitionSet);
        postponeEnterTransition();
        setHasOptionsMenu(true);
        final String cookie = Utils.settingsHelper.getString(Constants.COOKIE);
        final long userId = CookieUtils.getUserIdFromCookie(cookie);
        final String deviceUuid = Utils.settingsHelper.getString(Constants.DEVICE_UUID);
        final String csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        collectionService = CollectionService.getInstance(deviceUuid, csrfToken, userId);
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
        binding = FragmentCollectionPostsBinding.inflate(inflater, container, false);
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
        // delaying to make toolbar resume animation smooth, otherwise lags
        binding.getRoot().postDelayed(() -> inflater.inflate(R.menu.collection_posts_menu, menu), 500);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        if (item.getItemId() == R.id.layout) {
            showPostsLayoutPreferences();
            return true;
        } else if (item.getItemId() == R.id.delete) {
            final Context context = getContext();
            if (context == null) return false;
            new AlertDialog.Builder(context)
                    .setTitle(R.string.delete_collection)
                    .setMessage(R.string.delete_collection_note)
                    .setPositiveButton(R.string.confirm, (d, w) -> collectionService.deleteCollection(
                            savedCollection.getId(),
                            new ServiceCallback<String>() {
                                @Override
                                public void onSuccess(final String result) {
                                    SavedCollectionsFragment.pleaseRefresh = true;
                                    NavHostFragment.findNavController(CollectionPostsFragment.this).navigateUp();
                                }

                                @Override
                                public void onFailure(final Throwable t) {
                                    Log.e(TAG, "Error deleting collection", t);
                                    try {
                                        final Context context = getContext();
                                        if (context == null) return;
                                        Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                                    } catch (final Throwable ignored) {}
                                }
                            }))
                    .setNegativeButton(R.string.cancel, null)
                    .show();
        } else if (item.getItemId() == R.id.edit) {
            final Context context = getContext();
            if (context == null) return false;
            final EditText input = new EditText(context);
            new AlertDialog.Builder(context)
                    .setTitle(R.string.edit_collection)
                    .setView(input)
                    .setPositiveButton(R.string.confirm, (d, w) -> collectionService.editCollectionName(
                            savedCollection.getId(),
                            input.getText().toString(),
                            new ServiceCallback<String>() {
                                @Override
                                public void onSuccess(final String result) {
                                    binding.collapsingToolbarLayout.setTitle(input.getText().toString());
                                    SavedCollectionsFragment.pleaseRefresh = true;
                                }

                                @Override
                                public void onFailure(final Throwable t) {
                                    Log.e(TAG, "Error editing collection", t);
                                    try {
                                        final Context context = getContext();
                                        if (context == null) return;
                                        Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                                    } catch (final Throwable ignored) {}
                                }
                            }))
                    .setNegativeButton(R.string.cancel, null)
                    .show();
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
        final CollectionPostsFragmentArgs fragmentArgs = CollectionPostsFragmentArgs.fromBundle(getArguments());
        savedCollection = fragmentArgs.getSavedCollection();
        setupToolbar(fragmentArgs.getTitleColor(), fragmentArgs.getBackgroundColor());
        setupPosts();
    }

    private void setupToolbar(final int titleColor, final int backgroundColor) {
        if (savedCollection == null) {
            return;
        }
        binding.cover.setTransitionName("collection-" + savedCollection.getId());
        fragmentActivity.setToolbar(binding.toolbar);
        binding.collapsingToolbarLayout.setTitle(savedCollection.getTitle());
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
        final String coverUrl = ResponseBodyUtils.getImageUrl(savedCollection.getCoverMedias() == null
                                                              ? savedCollection.getCoverMedia()
                                                              : savedCollection.getCoverMedias().get(0));
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
        binding.posts.setViewModelStoreOwner(this)
                     .setLifeCycleOwner(this)
                     .setPostFetchService(new SavedPostFetchService(0, PostItemType.COLLECTION, true, savedCollection.getId()))
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
