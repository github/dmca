package awais.instagrabber.fragments.directmessages;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.text.Editable;
import android.util.Log;
import android.util.Pair;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.Toast;

import androidx.activity.OnBackPressedCallback;
import androidx.activity.OnBackPressedDispatcher;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.view.menu.ActionMenuItemView;
import androidx.core.content.ContextCompat;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsAnimationCompat;
import androidx.core.view.WindowInsetsAnimationControlListenerCompat;
import androidx.core.view.WindowInsetsAnimationControllerCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavBackStackEntry;
import androidx.navigation.NavController;
import androidx.navigation.NavDirections;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.SimpleItemAnimator;
import androidx.transition.TransitionManager;
import androidx.vectordrawable.graphics.drawable.Animatable2Compat;
import androidx.vectordrawable.graphics.drawable.AnimatedVectorDrawableCompat;

import com.google.android.material.badge.BadgeDrawable;
import com.google.android.material.badge.BadgeUtils;
import com.google.android.material.internal.ToolbarUtils;
import com.google.android.material.snackbar.Snackbar;
import com.google.common.collect.ImmutableList;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.function.Function;

import awais.instagrabber.ProfileNavGraphDirections;
import awais.instagrabber.R;
import awais.instagrabber.UserSearchNavGraphDirections;
import awais.instagrabber.activities.CameraActivity;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.DirectItemsAdapter;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemLongClickListener;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemOrHeader;
import awais.instagrabber.adapters.DirectReactionsAdapter;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemViewHolder;
import awais.instagrabber.animations.CubicBezierInterpolator;
import awais.instagrabber.customviews.InsetsAnimationLinearLayout;
import awais.instagrabber.customviews.KeyNotifyingEmojiEditText;
import awais.instagrabber.customviews.RecordView;
import awais.instagrabber.customviews.Tooltip;
import awais.instagrabber.customviews.emoji.Emoji;
import awais.instagrabber.customviews.emoji.EmojiBottomSheetDialog;
import awais.instagrabber.customviews.emoji.EmojiPicker;
import awais.instagrabber.customviews.helpers.ControlFocusInsetsAnimationCallback;
import awais.instagrabber.customviews.helpers.EmojiPickerInsetsAnimationCallback;
import awais.instagrabber.customviews.helpers.HeaderItemDecoration;
import awais.instagrabber.customviews.helpers.RecyclerLazyLoaderAtEdge;
import awais.instagrabber.customviews.helpers.SimpleImeAnimationController;
import awais.instagrabber.customviews.helpers.SwipeAndRestoreItemTouchHelperCallback;
import awais.instagrabber.customviews.helpers.TextWatcherAdapter;
import awais.instagrabber.customviews.helpers.TranslateDeferringInsetsAnimationCallback;
import awais.instagrabber.databinding.FragmentDirectMessagesThreadBinding;
import awais.instagrabber.dialogs.DirectItemReactionDialogFragment;
import awais.instagrabber.dialogs.GifPickerBottomDialogFragment;
import awais.instagrabber.dialogs.MediaPickerBottomDialogFragment;
import awais.instagrabber.fragments.PostViewV2Fragment;
import awais.instagrabber.fragments.UserSearchFragment;
import awais.instagrabber.fragments.UserSearchFragmentDirections;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.models.Resource;
import awais.instagrabber.models.enums.DirectItemType;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.requests.StoryViewerOptions;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemEmojiReaction;
import awais.instagrabber.repositories.responses.directmessages.DirectItemStoryShare;
import awais.instagrabber.repositories.responses.directmessages.DirectItemVisualMedia;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipient;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.DMUtils;
import awais.instagrabber.utils.DownloadUtils;
import awais.instagrabber.utils.PermissionUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.AppStateViewModel;
import awais.instagrabber.viewmodels.DirectThreadViewModel;
import awais.instagrabber.viewmodels.factories.DirectThreadViewModelFactory;

public class DirectMessageThreadFragment extends Fragment implements DirectReactionsAdapter.OnReactionClickListener,
        EmojiPicker.OnEmojiClickListener {
    private static final String TAG = DirectMessageThreadFragment.class.getSimpleName();
    private static final int STORAGE_PERM_REQUEST_CODE = 8020;
    private static final int AUDIO_RECORD_PERM_REQUEST_CODE = 1000;
    private static final int CAMERA_REQUEST_CODE = 200;
    private static final String TRANSLATION_Y = "translationY";

    private DirectItemsAdapter itemsAdapter;
    private MainActivity fragmentActivity;
    private DirectThreadViewModel viewModel;
    private InsetsAnimationLinearLayout root;
    private boolean shouldRefresh = true;
    private List<DirectItemOrHeader> itemOrHeaders;
    private List<User> users;
    private FragmentDirectMessagesThreadBinding binding;
    private Tooltip tooltip;
    private float initialSendX;
    private ActionBar actionBar;
    private AppStateViewModel appStateViewModel;
    private Runnable prevTitleRunnable;
    private AnimatorSet animatorSet;
    private boolean isRecording;
    private DirectItemReactionDialogFragment reactionDialogFragment;
    private DirectItem itemToForward;
    private MutableLiveData<Object> backStackSavedStateResultLiveData;
    private int prevLength;
    private BadgeDrawable pendingRequestCountBadgeDrawable;
    private boolean isPendingRequestCountBadgeAttached = false;
    private ItemTouchHelper itemTouchHelper;
    private LiveData<Boolean> pendingLiveData;
    private LiveData<DirectThread> threadLiveData;
    private LiveData<Integer> inputModeLiveData;
    private LiveData<String> threadTitleLiveData;
    private LiveData<Resource<Object>> fetchingLiveData;
    private LiveData<List<DirectItem>> itemsLiveData;
    private LiveData<DirectItem> replyToItemLiveData;
    private LiveData<Integer> pendingRequestsCountLiveData;
    private LiveData<List<User>> usersLiveData;
    private boolean autoMarkAsSeen = false;
    private MenuItem markAsSeenMenuItem;
    private Media tempMedia;
    private DirectItem addReactionItem;
    private TranslateDeferringInsetsAnimationCallback inputHolderAnimationCallback;
    private TranslateDeferringInsetsAnimationCallback chatsAnimationCallback;
    private EmojiPickerInsetsAnimationCallback emojiPickerAnimationCallback;
    private boolean hasKbOpenedOnce;
    private boolean wasToggled;

    private final AppExecutors appExecutors = AppExecutors.getInstance();
    private final Animatable2Compat.AnimationCallback micToSendAnimationCallback = new Animatable2Compat.AnimationCallback() {
        @Override
        public void onAnimationEnd(final Drawable drawable) {
            AnimatedVectorDrawableCompat.unregisterAnimationCallback(drawable, this);
            setSendToMicIcon();
        }
    };
    private final Animatable2Compat.AnimationCallback sendToMicAnimationCallback = new Animatable2Compat.AnimationCallback() {
        @Override
        public void onAnimationEnd(final Drawable drawable) {
            AnimatedVectorDrawableCompat.unregisterAnimationCallback(drawable, this);
            setMicToSendIcon();
        }
    };
    private final DirectItemCallback directItemCallback = new DirectItemCallback() {
        @Override
        public void onHashtagClick(final String hashtag) {
            final NavDirections action = DirectMessageThreadFragmentDirections.actionGlobalHashTagFragment(hashtag);
            NavHostFragment.findNavController(DirectMessageThreadFragment.this).navigate(action);
        }

        @Override
        public void onMentionClick(final String mention) {
            navigateToUser(mention);
        }

        @Override
        public void onLocationClick(final long locationId) {
            final NavDirections action = DirectMessageThreadFragmentDirections.actionGlobalLocationFragment(locationId);
            NavHostFragment.findNavController(DirectMessageThreadFragment.this).navigate(action);
        }

        @Override
        public void onURLClick(final String url) {
            final Context context = getContext();
            if (context == null) return;
            Utils.openURL(context, url);
        }

        @Override
        public void onEmailClick(final String email) {
            final Context context = getContext();
            if (context == null) return;
            Utils.openEmailAddress(context, email);
        }

        @Override
        public void onMediaClick(final Media media) {
            if (media.isReelMedia()) {
                final String pk = media.getPk();
                try {
                    final long mediaId = Long.parseLong(pk);
                    final User user = media.getUser();
                    if (user == null) return;
                    final String username = user.getUsername();
                    final NavDirections action = DirectMessageThreadFragmentDirections
                            .actionThreadToStory(StoryViewerOptions.forStory(mediaId, username));
                    NavHostFragment.findNavController(DirectMessageThreadFragment.this).navigate(action);
                } catch (NumberFormatException e) {
                    Log.e(TAG, "onMediaClick (story): ", e);
                }
                return;
            }
            final NavController navController = NavHostFragment.findNavController(DirectMessageThreadFragment.this);
            final Bundle bundle = new Bundle();
            bundle.putSerializable(PostViewV2Fragment.ARG_MEDIA, media);
            try {
                navController.navigate(R.id.action_global_post_view, bundle);
            } catch (Exception e) {
                Log.e(TAG, "openPostDialog: ", e);
            }
        }

        @Override
        public void onStoryClick(final DirectItemStoryShare storyShare) {
            final String pk = storyShare.getReelId();
            try {
                final long mediaId = Long.parseLong(pk);
                final User user = storyShare.getMedia().getUser();
                if (user == null) return;
                final String username = user.getUsername();
                final NavDirections action = DirectMessageThreadFragmentDirections
                        .actionThreadToStory(StoryViewerOptions.forUser(mediaId, username));
                NavHostFragment.findNavController(DirectMessageThreadFragment.this).navigate(action);
            } catch (NumberFormatException e) {
                Log.e(TAG, "onStoryClick: ", e);
            }
        }

        @Override
        public void onReaction(final DirectItem item, final Emoji emoji) {
            if (item == null) return;
            final LiveData<Resource<Object>> resourceLiveData = viewModel.sendReaction(item, emoji);
            if (resourceLiveData != null) {
                resourceLiveData.observe(getViewLifecycleOwner(), directItemResource -> handleSentMessage(resourceLiveData));
            }
        }

        @Override
        public void onReactionClick(final DirectItem item, final int position) {
            showReactionsDialog(item);
        }

        @Override
        public void onOptionSelect(final DirectItem item, final int itemId, final Function<DirectItem, Void> cb) {
            if (itemId == R.id.unsend) {
                handleSentMessage(viewModel.unsend(item));
                return;
            }
            if (itemId == R.id.forward) {
                itemToForward = item;
                final UserSearchNavGraphDirections.ActionGlobalUserSearch actionGlobalUserSearch = UserSearchFragmentDirections
                        .actionGlobalUserSearch()
                        .setTitle(getString(R.string.forward))
                        .setActionLabel(getString(R.string.send))
                        .setShowGroups(true)
                        .setMultiple(true)
                        .setSearchMode(UserSearchFragment.SearchMode.RAVEN);
                final NavController navController = NavHostFragment.findNavController(DirectMessageThreadFragment.this);
                navController.navigate(actionGlobalUserSearch);
            }
            if (itemId == R.id.download) {
                downloadItem(item);
                return;
            }
            // otherwise call callback if present
            if (cb != null) {
                cb.apply(item);
            }
        }

        @Override
        public void onAddReactionListener(final DirectItem item) {
            if (item == null) return;
            addReactionItem = item;
            final EmojiBottomSheetDialog emojiBottomSheetDialog = EmojiBottomSheetDialog.newInstance();
            emojiBottomSheetDialog.show(getChildFragmentManager(), EmojiBottomSheetDialog.TAG);
        }
    };
    private final DirectItemLongClickListener directItemLongClickListener = position -> {
        // viewModel.setSelectedPosition(position);
    };
    private final Observer<Object> backStackSavedStateObserver = result -> {
        if (result == null) return;
        if (result instanceof Uri) {
            final Uri uri = (Uri) result;
            handleSentMessage(viewModel.sendUri(uri));
        } else if ((result instanceof RankedRecipient)) {
            // Log.d(TAG, "result: " + result);
            if (itemToForward != null) {
                viewModel.forward((RankedRecipient) result, itemToForward);
            }
        } else if ((result instanceof Set)) {
            try {
                // Log.d(TAG, "result: " + result);
                if (itemToForward != null) {
                    //noinspection unchecked
                    viewModel.forward((Set<RankedRecipient>) result, itemToForward);
                }
            } catch (Exception e) {
                Log.e(TAG, "forward result: ", e);
            }
        }
        // clear result
        backStackSavedStateResultLiveData.postValue(null);
    };
    private final MutableLiveData<Integer> inputLength = new MutableLiveData<>(0);
    private final MutableLiveData<Boolean> emojiPickerVisible = new MutableLiveData<>(false);
    private final MutableLiveData<Boolean> kbVisible = new MutableLiveData<>(false);
    private final OnBackPressedCallback onEmojiPickerBackPressedCallback = new OnBackPressedCallback(false) {
        @Override
        public void handleOnBackPressed() {
            emojiPickerVisible.postValue(false);
        }
    };

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = (MainActivity) requireActivity();
        appStateViewModel = new ViewModelProvider(fragmentActivity).get(AppStateViewModel.class);
        autoMarkAsSeen = Utils.settingsHelper.getBoolean(PreferenceKeys.DM_MARK_AS_SEEN);
        final Bundle arguments = getArguments();
        if (arguments == null) return;
        final DirectMessageThreadFragmentArgs fragmentArgs = DirectMessageThreadFragmentArgs.fromBundle(arguments);
        final User currentUser = appStateViewModel.getCurrentUser();
        if (currentUser == null) return;
        final DirectThreadViewModelFactory viewModelFactory = new DirectThreadViewModelFactory(
                fragmentActivity.getApplication(),
                fragmentArgs.getThreadId(),
                fragmentArgs.getPending(),
                currentUser
        );
        viewModel = new ViewModelProvider(this, viewModelFactory).get(DirectThreadViewModel.class);
        setHasOptionsMenu(true);
    }

    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             final ViewGroup container,
                             final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentDirectMessagesThreadBinding.inflate(inflater, container, false);
        binding.send.setRecordView(binding.recordView);
        root = binding.getRoot();
        final Context context = getContext();
        if (context == null) {
            return root;
        }
        tooltip = new Tooltip(context, root, getResources().getColor(R.color.grey_400), getResources().getColor(R.color.black));
        // todo check has camera and remove view
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        // WindowCompat.setDecorFitsSystemWindows(fragmentActivity.getWindow(), false);
        if (!shouldRefresh) return;
        init();
        binding.send.post(() -> initialSendX = binding.send.getX());
        shouldRefresh = false;
    }

    @Override
    public void onCreateOptionsMenu(@NonNull final Menu menu, @NonNull final MenuInflater inflater) {
        inflater.inflate(R.menu.dm_thread_menu, menu);
        markAsSeenMenuItem = menu.findItem(R.id.mark_as_seen);
        if (markAsSeenMenuItem != null) {
            if (autoMarkAsSeen) {
                markAsSeenMenuItem.setVisible(false);
            } else {
                markAsSeenMenuItem.setEnabled(false);
            }
        }
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        final int itemId = item.getItemId();
        if (itemId == R.id.info) {
            final DirectMessageThreadFragmentDirections.ActionThreadToSettings directions = DirectMessageThreadFragmentDirections
                    .actionThreadToSettings(viewModel.getThreadId(), null);
            final Boolean pending = viewModel.isPending().getValue();
            directions.setPending(pending != null && pending);
            NavHostFragment.findNavController(this).navigate(directions);
            return true;
        }
        if (itemId == R.id.mark_as_seen) {
            handleMarkAsSeen(item);
            return true;
        }
        if (itemId == R.id.refresh && viewModel != null) {
            viewModel.refreshChats();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void handleMarkAsSeen(@NonNull final MenuItem item) {
        final LiveData<Resource<Object>> resourceLiveData = viewModel.markAsSeen();
        resourceLiveData.observe(getViewLifecycleOwner(), new Observer<Resource<Object>>() {
            @Override
            public void onChanged(final Resource<Object> resource) {
                try {
                    if (resource == null) return;
                    final Context context = getContext();
                    if (context == null) return;
                    switch (resource.status) {
                        case SUCCESS:
                            Toast.makeText(context, R.string.marked_as_seen, Toast.LENGTH_SHORT).show();
                        case LOADING:
                            item.setEnabled(false);
                            break;
                        case ERROR:
                            item.setEnabled(true);
                            if (resource.message != null) {
                                Snackbar.make(context, binding.getRoot(), resource.message, Snackbar.LENGTH_LONG).show();
                                return;
                            }
                            if (resource.resId != 0) {
                                Snackbar.make(binding.getRoot(), resource.resId, Snackbar.LENGTH_LONG).show();
                                return;
                            }
                            break;
                    }
                } finally {
                    resourceLiveData.removeObserver(this);
                }
            }
        });
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == CAMERA_REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            if (data == null || data.getData() == null) {
                Log.w(TAG, "data is null!");
                return;
            }
            final Uri uri = data.getData();
            navigateToImageEditFragment(uri);
        }
    }

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        final Context context = getContext();
        if (context == null) return;
        if (requestCode == STORAGE_PERM_REQUEST_CODE && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            if (tempMedia == null) return;
            downloadItem(context, tempMedia);
            return;
        }
        if (requestCode == AUDIO_RECORD_PERM_REQUEST_CODE) {
            if (PermissionUtils.hasAudioRecordPerms(context)) {
                Toast.makeText(context, "You can send voice messages now!", Toast.LENGTH_LONG).show();
                return;
            }
            Toast.makeText(context, "Require RECORD_AUDIO and WRITE_EXTERNAL_STORAGE permissions", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onPause() {
        if (isRecording) {
            binding.recordView.cancelRecording(binding.send);
        }
        emojiPickerVisible.postValue(false);
        kbVisible.postValue(false);
        binding.inputHolder.setTranslationY(0);
        binding.chats.setTranslationY(0);
        binding.emojiPicker.setTranslationY(0);
        removeObservers();
        super.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        if (initialSendX != 0) {
            binding.send.setX(initialSendX);
        }
        binding.send.stopScale();
        final OnBackPressedDispatcher onBackPressedDispatcher = fragmentActivity.getOnBackPressedDispatcher();
        onBackPressedDispatcher.addCallback(onEmojiPickerBackPressedCallback);
        setupBackStackResultObserver();
        setObservers();
        // attachPendingRequestsBadge(viewModel.getPendingRequestsCount().getValue());
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        cleanup();
    }

    @Override
    public void onDestroy() {
        viewModel.deleteThreadIfRequired();
        super.onDestroy();
    }

    @SuppressLint("UnsafeOptInUsageError")
    private void cleanup() {
        if (prevTitleRunnable != null) {
            appExecutors.mainThread().cancel(prevTitleRunnable);
        }
        for (int childCount = binding.chats.getChildCount(), i = 0; i < childCount; ++i) {
            final RecyclerView.ViewHolder holder = binding.chats.getChildViewHolder(binding.chats.getChildAt(i));
            if (holder == null) continue;
            if (holder instanceof DirectItemViewHolder) {
                ((DirectItemViewHolder) holder).cleanup();
            }
        }
        isPendingRequestCountBadgeAttached = false;
        if (pendingRequestCountBadgeDrawable != null) {
            @SuppressLint("RestrictedApi") final ActionMenuItemView menuItemView = ToolbarUtils
                    .getActionMenuItemView(fragmentActivity.getToolbar(), R.id.info);
            if (menuItemView != null) {
                BadgeUtils.detachBadgeDrawable(pendingRequestCountBadgeDrawable, fragmentActivity.getToolbar(), R.id.info);
            }
            pendingRequestCountBadgeDrawable = null;
        }
    }

    private void init() {
        final Context context = getContext();
        if (context == null) return;
        if (getArguments() == null) return;
        actionBar = fragmentActivity.getSupportActionBar();
        setupList();
        root.post(this::setupInput);
    }

    private void setupList() {
        final Context context = getContext();
        if (context == null) return;
        binding.chats.setItemViewCacheSize(20);
        final LinearLayoutManager layoutManager = new LinearLayoutManager(context);
        layoutManager.setReverseLayout(true);
        // layoutManager.setStackFromEnd(false);
        // binding.messageList.addItemDecoration(new VerticalSpaceItemDecoration(3));
        final RecyclerView.ItemAnimator animator = binding.chats.getItemAnimator();
        if (animator instanceof SimpleItemAnimator) {
            final SimpleItemAnimator itemAnimator = (SimpleItemAnimator) animator;
            itemAnimator.setSupportsChangeAnimations(false);
        }
        binding.chats.setLayoutManager(layoutManager);
        binding.chats.addOnScrollListener(new RecyclerLazyLoaderAtEdge(layoutManager, true, page -> viewModel.fetchChats()));
        final HeaderItemDecoration headerItemDecoration = new HeaderItemDecoration(binding.chats, itemPosition -> {
            if (itemOrHeaders == null || itemOrHeaders.isEmpty()) return false;
            try {
                final DirectItemOrHeader itemOrHeader = itemOrHeaders.get(itemPosition);
                return itemOrHeader.isHeader();
            } catch (IndexOutOfBoundsException e) {
                return false;
            }
        });
        binding.chats.addItemDecoration(headerItemDecoration);
        final SwipeAndRestoreItemTouchHelperCallback touchHelperCallback = new SwipeAndRestoreItemTouchHelperCallback(
                context,
                (adapterPosition, viewHolder) -> {
                    if (itemsAdapter == null) return;
                    final DirectItemOrHeader directItemOrHeader = itemsAdapter.getList().get(adapterPosition);
                    if (directItemOrHeader.isHeader()) return;
                    viewModel.setReplyToItem(directItemOrHeader.item);
                }
        );
        final Integer inputMode = viewModel.getInputMode().getValue();
        if (inputMode != null && inputMode != 1) {
            itemTouchHelper = new ItemTouchHelper(touchHelperCallback);
            itemTouchHelper.attachToRecyclerView(binding.chats);
        }
    }

    private void setObservers() {
        threadLiveData = viewModel.getThread();
        if (threadLiveData == null) {
            final NavController navController = NavHostFragment.findNavController(this);
            navController.navigateUp();
            return;
        }
        pendingLiveData = viewModel.isPending();
        pendingLiveData.observe(getViewLifecycleOwner(), isPending -> {
            if (isPending == null) {
                hideInput();
                return;
            }
            if (isPending) {
                showPendingOptions();
                return;
            }
            hidePendingOptions();
            final Integer inputMode = viewModel.getInputMode().getValue();
            if (inputMode != null && inputMode == 1) return;
            showInput();
        });
        inputModeLiveData = viewModel.getInputMode();
        inputModeLiveData.observe(getViewLifecycleOwner(), inputMode -> {
            final Boolean isPending = viewModel.isPending().getValue();
            if (isPending != null && isPending) return;
            if (inputMode == null || inputMode == 0) return;
            if (inputMode == 1) {
                hideInput();
            }
        });
        threadTitleLiveData = viewModel.getThreadTitle();
        threadTitleLiveData.observe(getViewLifecycleOwner(), this::setTitle);
        fetchingLiveData = viewModel.isFetching();
        fetchingLiveData.observe(getViewLifecycleOwner(), fetchingResource -> {
            if (fetchingResource == null) return;
            switch (fetchingResource.status) {
                case SUCCESS:
                case ERROR:
                    setTitle(viewModel.getThreadTitle().getValue());
                    if (fetchingResource.message != null) {
                        Snackbar.make(binding.getRoot(), fetchingResource.message, Snackbar.LENGTH_LONG).show();
                    }
                    if (fetchingResource.resId != 0) {
                        Snackbar.make(binding.getRoot(), fetchingResource.resId, Snackbar.LENGTH_LONG).show();
                    }
                    break;
                case LOADING:
                    setTitle(getString(R.string.dms_thread_updating));
                    break;
            }
        });
        // final ItemsAdapterDataMerger itemsAdapterDataMerger = new ItemsAdapterDataMerger(appStateViewModel.getCurrentUser(), viewModel.getThread());
        // itemsAdapterDataMerger.observe(getViewLifecycleOwner(), userThreadPair -> {
        //     viewModel.setCurrentUser(userThreadPair.first);
        //     setupItemsAdapter(userThreadPair.first, userThreadPair.second);
        // });
        threadLiveData.observe(getViewLifecycleOwner(), this::setupItemsAdapter);
        itemsLiveData = viewModel.getItems();
        itemsLiveData.observe(getViewLifecycleOwner(), this::submitItemsToAdapter);
        replyToItemLiveData = viewModel.getReplyToItem();
        replyToItemLiveData.observe(getViewLifecycleOwner(), item -> {
            if (item == null) {
                if (binding.input.length() == 0) {
                    showExtraInputOption(true);
                }
                binding.getRoot().post(() -> {
                    TransitionManager.beginDelayedTransition(binding.getRoot());
                    binding.replyBg.setVisibility(View.GONE);
                    binding.replyInfo.setVisibility(View.GONE);
                    binding.replyPreviewImage.setVisibility(View.GONE);
                    binding.replyCancel.setVisibility(View.GONE);
                    binding.replyPreviewText.setVisibility(View.GONE);
                });
                return;
            }
            showExtraInputOption(false);
            binding.getRoot().postDelayed(() -> {
                binding.replyBg.setVisibility(View.VISIBLE);
                binding.replyInfo.setVisibility(View.VISIBLE);
                binding.replyPreviewImage.setVisibility(View.VISIBLE);
                binding.replyCancel.setVisibility(View.VISIBLE);
                binding.replyPreviewText.setVisibility(View.VISIBLE);
                if (item.getUserId() == viewModel.getViewerId()) {
                    binding.replyInfo.setText(R.string.replying_to_yourself);
                } else {
                    final User user = viewModel.getUser(item.getUserId());
                    if (user != null) {
                        binding.replyInfo.setText(getString(R.string.replying_to_user, user.getFullName()));
                    } else {
                        binding.replyInfo.setVisibility(View.GONE);
                    }
                }
                final String previewText = getDirectItemPreviewText(item);
                binding.replyPreviewText.setText(TextUtils.isEmpty(previewText) ? getString(R.string.message) : previewText);
                final String previewImageUrl = getDirectItemPreviewImageUrl(item);
                if (TextUtils.isEmpty(previewImageUrl)) {
                    binding.replyPreviewImage.setVisibility(View.GONE);
                } else {
                    binding.replyPreviewImage.setImageURI(previewImageUrl);
                }
                binding.replyCancel.setOnClickListener(v -> viewModel.setReplyToItem(null));
            }, 200);
        });
        inputLength.observe(getViewLifecycleOwner(), length -> {
            if (length == null) return;
            final boolean hasReplyToItem = viewModel.getReplyToItem().getValue() != null;
            if (hasReplyToItem) {
                prevLength = length;
                return;
            }
            if ((prevLength == 0 && length != 0) || (prevLength != 0 && length == 0)) {
                showExtraInputOption(length == 0);
            }
            prevLength = length;
        });
        pendingRequestsCountLiveData = viewModel.getPendingRequestsCount();
        pendingRequestsCountLiveData.observe(getViewLifecycleOwner(), this::attachPendingRequestsBadge);
        usersLiveData = viewModel.getUsers();
        usersLiveData.observe(getViewLifecycleOwner(), users -> {
            if (users == null || users.isEmpty()) return;
            final User user = users.get(0);
            binding.acceptPendingRequestQuestion.setText(getString(R.string.accept_request_from_user, user.getUsername(), user.getFullName()));
        });
    }

    private void removeObservers() {
        pendingLiveData.removeObservers(getViewLifecycleOwner());
        inputModeLiveData.removeObservers(getViewLifecycleOwner());
        threadTitleLiveData.removeObservers(getViewLifecycleOwner());
        fetchingLiveData.removeObservers(getViewLifecycleOwner());
        threadLiveData.removeObservers(getViewLifecycleOwner());
        itemsLiveData.removeObservers(getViewLifecycleOwner());
        replyToItemLiveData.removeObservers(getViewLifecycleOwner());
        inputLength.removeObservers(getViewLifecycleOwner());
        pendingRequestsCountLiveData.removeObservers(getViewLifecycleOwner());
        usersLiveData.removeObservers(getViewLifecycleOwner());

    }

    private void hidePendingOptions() {
        binding.acceptPendingRequestQuestion.setVisibility(View.GONE);
        binding.decline.setVisibility(View.GONE);
        binding.accept.setVisibility(View.GONE);
    }

    private void showPendingOptions() {
        binding.acceptPendingRequestQuestion.setVisibility(View.VISIBLE);
        binding.decline.setVisibility(View.VISIBLE);
        binding.accept.setVisibility(View.VISIBLE);
        binding.accept.setOnClickListener(v -> {
            final LiveData<Resource<Object>> resourceLiveData = viewModel.acceptRequest();
            handlePendingChangeResource(resourceLiveData, false);
        });
        binding.decline.setOnClickListener(v -> {
            final LiveData<Resource<Object>> resourceLiveData = viewModel.declineRequest();
            handlePendingChangeResource(resourceLiveData, true);
        });
    }

    private void handlePendingChangeResource(final LiveData<Resource<Object>> resourceLiveData, final boolean isDecline) {
        resourceLiveData.observe(getViewLifecycleOwner(), resource -> {
            if (resource == null) return;
            final Resource.Status status = resource.status;
            switch (status) {
                case SUCCESS:
                    resourceLiveData.removeObservers(getViewLifecycleOwner());
                    if (isDecline) {
                        removeObservers();
                        viewModel.removeThread();
                        final NavController navController = NavHostFragment.findNavController(this);
                        navController.navigateUp();
                        return;
                    }
                    removeObservers();
                    viewModel.moveFromPending();
                    setObservers();
                    break;
                case LOADING:
                    break;
                case ERROR:
                    if (resource.message != null) {
                        Snackbar.make(binding.getRoot(), resource.message, Snackbar.LENGTH_LONG).show();
                    }
                    if (resource.resId != 0) {
                        Snackbar.make(binding.getRoot(), resource.resId, Snackbar.LENGTH_LONG).show();
                    }
                    resourceLiveData.removeObservers(getViewLifecycleOwner());
                    break;
            }
        });
    }

    private void hideInput() {
        binding.emojiToggle.setVisibility(View.GONE);
        binding.gif.setVisibility(View.GONE);
        binding.camera.setVisibility(View.GONE);
        binding.gallery.setVisibility(View.GONE);
        binding.input.setVisibility(View.GONE);
        binding.inputBg.setVisibility(View.GONE);
        binding.recordView.setVisibility(View.GONE);
        binding.send.setVisibility(View.GONE);
        if (itemTouchHelper != null) {
            itemTouchHelper.attachToRecyclerView(null);
        }
    }

    private void showInput() {
        binding.emojiToggle.setVisibility(View.VISIBLE);
        binding.gif.setVisibility(View.VISIBLE);
        binding.camera.setVisibility(View.VISIBLE);
        binding.gallery.setVisibility(View.VISIBLE);
        binding.input.setVisibility(View.VISIBLE);
        binding.inputBg.setVisibility(View.VISIBLE);
        binding.recordView.setVisibility(View.VISIBLE);
        binding.send.setVisibility(View.VISIBLE);
        if (itemTouchHelper != null) {
            itemTouchHelper.attachToRecyclerView(binding.chats);
        }
    }

    @SuppressLint("UnsafeOptInUsageError")
    private void attachPendingRequestsBadge(@Nullable final Integer count) {
        if (pendingRequestCountBadgeDrawable == null) {
            final Context context = getContext();
            if (context == null) return;
            pendingRequestCountBadgeDrawable = BadgeDrawable.create(context);
        }
        if (count == null || count == 0) {
            @SuppressLint("RestrictedApi") final ActionMenuItemView menuItemView = ToolbarUtils
                    .getActionMenuItemView(fragmentActivity.getToolbar(), R.id.info);
            if (menuItemView != null) {
                BadgeUtils.detachBadgeDrawable(pendingRequestCountBadgeDrawable, fragmentActivity.getToolbar(), R.id.info);
            }
            isPendingRequestCountBadgeAttached = false;
            pendingRequestCountBadgeDrawable.setNumber(0);
            return;
        }
        if (pendingRequestCountBadgeDrawable.getNumber() == count) return;
        pendingRequestCountBadgeDrawable.setNumber(count);
        if (!isPendingRequestCountBadgeAttached) {
            BadgeUtils.attachBadgeDrawable(pendingRequestCountBadgeDrawable, fragmentActivity.getToolbar(), R.id.info);
            isPendingRequestCountBadgeAttached = true;
        }
    }

    private void showExtraInputOption(final boolean show) {
        if (show) {
            if (!binding.send.isListenForRecord()) {
                binding.send.setListenForRecord(true);
                startIconAnimation();
            }
            binding.gif.setVisibility(View.VISIBLE);
            binding.camera.setVisibility(View.VISIBLE);
            binding.gallery.setVisibility(View.VISIBLE);
            return;
        }
        if (binding.send.isListenForRecord()) {
            binding.send.setListenForRecord(false);
            startIconAnimation();
        }
        binding.gif.setVisibility(View.GONE);
        binding.camera.setVisibility(View.GONE);
        binding.gallery.setVisibility(View.GONE);
    }

    private String getDirectItemPreviewText(final DirectItem item) {
        switch (item.getItemType()) {
            case TEXT:
                return item.getText();
            case LINK:
                return item.getLink().getText();
            case MEDIA: {
                final Media media = item.getMedia();
                return getMediaPreviewTextString(media);
            }
            case RAVEN_MEDIA: {
                final DirectItemVisualMedia visualMedia = item.getVisualMedia();
                final Media media = visualMedia.getMedia();
                return getMediaPreviewTextString(media);
            }
            case VOICE_MEDIA:
                return getString(R.string.voice_message);
            case MEDIA_SHARE:
                return getString(R.string.post);
            case REEL_SHARE:
                return item.getReelShare().getText();
        }
        return "";
    }

    @NonNull
    private String getMediaPreviewTextString(final Media media) {
        final MediaItemType mediaType = media.getMediaType();
        switch (mediaType) {
            case MEDIA_TYPE_IMAGE:
                return getString(R.string.photo);
            case MEDIA_TYPE_VIDEO:
                return getString(R.string.video);
            default:
                return "";
        }
    }

    private String getDirectItemPreviewImageUrl(final DirectItem item) {
        switch (item.getItemType()) {
            case TEXT:
            case LINK:
            case VOICE_MEDIA:
            case REEL_SHARE:
                return null;
            case MEDIA: {
                final Media media = item.getMedia();
                return ResponseBodyUtils.getThumbUrl(media);
            }
            case RAVEN_MEDIA: {
                final DirectItemVisualMedia visualMedia = item.getVisualMedia();
                final Media media = visualMedia.getMedia();
                return ResponseBodyUtils.getThumbUrl(media);
            }
            case MEDIA_SHARE: {
                final Media media = item.getMediaShare();
                return ResponseBodyUtils.getThumbUrl(media);
            }
        }
        return null;
    }

    private void setupBackStackResultObserver() {
        final NavController navController = NavHostFragment.findNavController(this);
        final NavBackStackEntry backStackEntry = navController.getCurrentBackStackEntry();
        if (backStackEntry != null) {
            backStackSavedStateResultLiveData = backStackEntry.getSavedStateHandle().getLiveData("result");
            backStackSavedStateResultLiveData.observe(getViewLifecycleOwner(), backStackSavedStateObserver);
        }
    }

    private void submitItemsToAdapter(final List<DirectItem> items) {
        binding.chats.post(() -> {
            if (autoMarkAsSeen) {
                viewModel.markAsSeen();
                return;
            }
            final DirectThread thread = threadLiveData.getValue();
            if (thread == null) return;
            if (markAsSeenMenuItem != null) {
                markAsSeenMenuItem.setEnabled(!DMUtils.isRead(thread));
            }
        });
        if (itemsAdapter == null) return;
        itemsAdapter.submitList(items, () -> {
            itemOrHeaders = itemsAdapter.getList();
            binding.chats.post(() -> {
                final RecyclerView.LayoutManager layoutManager = binding.chats.getLayoutManager();
                if (layoutManager instanceof LinearLayoutManager) {
                    final int position = ((LinearLayoutManager) layoutManager).findLastCompletelyVisibleItemPosition();
                    if (position < 0) return;
                    if (position == itemsAdapter.getItemCount() - 1) {
                        viewModel.fetchChats();
                    }
                }
            });
        });
    }

    private void setupItemsAdapter(final DirectThread thread) {
        if (thread == null) return;
        if (itemsAdapter != null) {
            if (itemsAdapter.getThread() == thread) return;
            itemsAdapter.setThread(thread);
            return;
        }
        final User currentUser = appStateViewModel.getCurrentUser();
        if (currentUser == null) return;
        itemsAdapter = new DirectItemsAdapter(currentUser, thread, directItemCallback, directItemLongClickListener);
        itemsAdapter.setHasStableIds(true);
        itemsAdapter.setStateRestorationPolicy(RecyclerView.Adapter.StateRestorationPolicy.PREVENT_WHEN_EMPTY);
        binding.chats.setAdapter(itemsAdapter);
        registerDataObserver();
        users = thread.getUsers();
        final List<DirectItem> items = viewModel.getItems().getValue();
        if (items != null && itemsAdapter.getItems() != items) {
            submitItemsToAdapter(items);
        }
    }

    private void registerDataObserver() {
        itemsAdapter.registerAdapterDataObserver(new RecyclerView.AdapterDataObserver() {

            @Override
            public void onItemRangeInserted(final int positionStart, final int itemCount) {
                super.onItemRangeInserted(positionStart, itemCount);
                final LinearLayoutManager layoutManager = (LinearLayoutManager) binding.chats.getLayoutManager();
                if (layoutManager == null) return;
                int firstVisiblePosition = layoutManager.findFirstCompletelyVisibleItemPosition();
                if ((firstVisiblePosition == -1 || firstVisiblePosition == 0) && (positionStart == 0)) {
                    binding.chats.scrollToPosition(0);
                }
            }
        });
    }

    private void setupInput() {
        final Integer inputMode = viewModel.getInputMode().getValue();
        if (inputMode != null && inputMode == 1) return;
        final Context context = getContext();
        if (context == null) return;
        tooltip.setText(R.string.dms_thread_audio_hint);
        setMicToSendIcon();
        binding.recordView.setMinMillis(1000);
        binding.recordView.setOnRecordListener(new RecordView.OnRecordListener() {
            @Override
            public void onStart() {
                isRecording = true;
                binding.input.setHint(null);
                binding.gif.setVisibility(View.GONE);
                binding.camera.setVisibility(View.GONE);
                binding.gallery.setVisibility(View.GONE);
                if (PermissionUtils.hasAudioRecordPerms(context)) {
                    viewModel.startRecording();
                    return;
                }
                PermissionUtils.requestAudioRecordPerms(DirectMessageThreadFragment.this, AUDIO_RECORD_PERM_REQUEST_CODE);
            }

            @Override
            public void onCancel() {
                Log.d(TAG, "onCancel");
                // binding.input.setHint("Message");
                viewModel.stopRecording(true);
                isRecording = false;
            }

            @Override
            public void onFinish(final long recordTime) {
                Log.d(TAG, "onFinish");
                binding.input.setHint("Message");
                binding.gif.setVisibility(View.VISIBLE);
                binding.camera.setVisibility(View.VISIBLE);
                binding.gallery.setVisibility(View.VISIBLE);
                viewModel.stopRecording(false);
                isRecording = false;
            }

            @Override
            public void onLessThanMin() {
                Log.d(TAG, "onLessThanMin");
                binding.input.setHint("Message");
                if (PermissionUtils.hasAudioRecordPerms(context)) {
                    tooltip.show(binding.send);
                }
                binding.gif.setVisibility(View.VISIBLE);
                binding.camera.setVisibility(View.VISIBLE);
                binding.gallery.setVisibility(View.VISIBLE);
                viewModel.stopRecording(true);
                isRecording = false;
            }
        });
        binding.recordView.setOnBasketAnimationEndListener(() -> {
            binding.input.setHint(R.string.dms_thread_message_hint);
            binding.gif.setVisibility(View.VISIBLE);
            binding.camera.setVisibility(View.VISIBLE);
            binding.gallery.setVisibility(View.VISIBLE);
        });
        binding.input.addTextChangedListener(new TextWatcherAdapter() {
            // int prevLength = 0;

            @Override
            public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {
                final int length = s.length();
                inputLength.postValue(length);
            }
        });
        binding.send.setOnRecordClickListener(v -> {
            final Editable text = binding.input.getText();
            if (TextUtils.isEmpty(text)) return;
            final LiveData<Resource<Object>> resourceLiveData = viewModel.sendText(text.toString());
            resourceLiveData.observe(getViewLifecycleOwner(), resource -> handleSentMessage(resourceLiveData));
            binding.input.setText("");
            viewModel.setReplyToItem(null);
        });
        binding.send.setOnRecordLongClickListener(v -> {
            Log.d(TAG, "setOnRecordLongClickListener");
            return true;
        });
        binding.input.setOnFocusChangeListener((v, hasFocus) -> {
            if (!hasFocus) return;
            final Boolean emojiPickerVisibleValue = emojiPickerVisible.getValue();
            if (emojiPickerVisibleValue == null || !emojiPickerVisibleValue) return;
            inputHolderAnimationCallback.setShouldTranslate(false);
            chatsAnimationCallback.setShouldTranslate(false);
            emojiPickerAnimationCallback.setShouldTranslate(false);
        });
        setupInsetsCallback();
        setupEmojiPicker();
        binding.gallery.setOnClickListener(v -> {
            final MediaPickerBottomDialogFragment mediaPicker = MediaPickerBottomDialogFragment.newInstance();
            mediaPicker.setOnSelectListener(entry -> {
                mediaPicker.dismiss();
                if (!isAdded()) return;
                if (!entry.isVideo) {
                    navigateToImageEditFragment(entry.path);
                    return;
                }
                handleSentMessage(viewModel.sendUri(entry));
            });
            mediaPicker.show(getChildFragmentManager(), "MediaPicker");
        });
        binding.gif.setOnClickListener(v -> {
            final GifPickerBottomDialogFragment gifPicker = GifPickerBottomDialogFragment.newInstance();
            gifPicker.setOnSelectListener(giphyGif -> {
                gifPicker.dismiss();
                if (giphyGif == null) return;
                handleSentMessage(viewModel.sendAnimatedMedia(giphyGif));
            });
            gifPicker.show(getChildFragmentManager(), "GifPicker");
        });
        binding.camera.setOnClickListener(v -> {
            final Intent intent = new Intent(context, CameraActivity.class);
            startActivityForResult(intent, CAMERA_REQUEST_CODE);
        });
    }

    private void setupInsetsCallback() {
        inputHolderAnimationCallback = new TranslateDeferringInsetsAnimationCallback(
                binding.inputHolder,
                WindowInsetsCompat.Type.systemBars(),
                WindowInsetsCompat.Type.ime(),
                WindowInsetsAnimationCompat.Callback.DISPATCH_MODE_CONTINUE_ON_SUBTREE
        );
        ViewCompat.setWindowInsetsAnimationCallback(binding.inputHolder, inputHolderAnimationCallback);
        chatsAnimationCallback = new TranslateDeferringInsetsAnimationCallback(
                binding.chats,
                WindowInsetsCompat.Type.systemBars(),
                WindowInsetsCompat.Type.ime()
        );
        ViewCompat.setWindowInsetsAnimationCallback(binding.chats, chatsAnimationCallback);
        emojiPickerAnimationCallback = new EmojiPickerInsetsAnimationCallback(
                binding.emojiPicker,
                WindowInsetsCompat.Type.systemBars(),
                WindowInsetsCompat.Type.ime()
        );
        emojiPickerAnimationCallback.setKbVisibilityListener(this::onKbVisibilityChange);
        ViewCompat.setWindowInsetsAnimationCallback(binding.emojiPicker, emojiPickerAnimationCallback);
        ViewCompat.setWindowInsetsAnimationCallback(
                binding.input,
                new ControlFocusInsetsAnimationCallback(binding.input)
        );
        final SimpleImeAnimationController imeAnimController = root.getImeAnimController();
        if (imeAnimController != null) {
            imeAnimController.setAnimationControlListener(new WindowInsetsAnimationControlListenerCompat() {
                @Override
                public void onReady(@NonNull final WindowInsetsAnimationControllerCompat controller, final int types) {}

                @Override
                public void onFinished(@NonNull final WindowInsetsAnimationControllerCompat controller) {
                    checkKbVisibility();
                }

                @Override
                public void onCancelled(@Nullable final WindowInsetsAnimationControllerCompat controller) {
                    checkKbVisibility();
                }

                private void checkKbVisibility() {
                    final WindowInsetsCompat rootWindowInsets = ViewCompat.getRootWindowInsets(binding.getRoot());
                    final boolean visible = rootWindowInsets != null && rootWindowInsets.isVisible(WindowInsetsCompat.Type.ime());
                    onKbVisibilityChange(visible);
                }
            });
        }
    }

    private void onKbVisibilityChange(final boolean kbVisible) {
        this.kbVisible.postValue(kbVisible);
        if (wasToggled) {
            emojiPickerVisible.postValue(!kbVisible);
            wasToggled = false;
            return;
        }
        final Boolean emojiPickerVisibleValue = emojiPickerVisible.getValue();
        if (kbVisible && emojiPickerVisibleValue != null && emojiPickerVisibleValue) {
            emojiPickerVisible.postValue(false);
            return;
        }
        if (!kbVisible) {
            emojiPickerVisible.postValue(false);
        }
    }

    private void startIconAnimation() {
        final Drawable icon = binding.send.getIcon();
        if (icon instanceof Animatable) {
            final Animatable animatable = (Animatable) icon;
            if (animatable.isRunning()) {
                animatable.stop();
            }
            animatable.start();
        }
    }

    private void navigateToImageEditFragment(final String path) {
        navigateToImageEditFragment(Uri.fromFile(new File(path)));
    }

    private void navigateToImageEditFragment(final Uri uri) {
        final NavDirections navDirections = DirectMessageThreadFragmentDirections.actionThreadToImageEdit(uri);
        final NavController navController = NavHostFragment.findNavController(this);
        navController.navigate(navDirections);
    }

    private void handleSentMessage(final LiveData<Resource<Object>> resourceLiveData) {
        final Resource<Object> resource = resourceLiveData.getValue();
        if (resource == null) return;
        final Resource.Status status = resource.status;
        switch (status) {
            case SUCCESS:
                resourceLiveData.removeObservers(getViewLifecycleOwner());
                break;
            case LOADING:
                break;
            case ERROR:
                if (resource.message != null) {
                    Snackbar.make(binding.getRoot(), resource.message, Snackbar.LENGTH_LONG).show();
                }
                if (resource.resId != 0) {
                    Snackbar.make(binding.getRoot(), resource.resId, Snackbar.LENGTH_LONG).show();
                }
                resourceLiveData.removeObservers(getViewLifecycleOwner());
                break;
        }
    }

    private void setupEmojiPicker() {
        root.post(() -> binding.emojiPicker.init(
                root,
                (view, emoji) -> {
                    final KeyNotifyingEmojiEditText input = binding.input;
                    final int start = input.getSelectionStart();
                    final int end = input.getSelectionEnd();
                    if (start < 0) {
                        input.append(emoji.getUnicode());
                        return;
                    }
                    input.getText().replace(
                            Math.min(start, end),
                            Math.max(start, end),
                            emoji.getUnicode(),
                            0,
                            emoji.getUnicode().length()
                    );
                },
                () -> binding.input.dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_DEL))
        ));
        binding.emojiToggle.setOnClickListener(v -> {
            Boolean isEmojiPickerVisible = emojiPickerVisible.getValue();
            if (isEmojiPickerVisible == null) isEmojiPickerVisible = false;
            Boolean isKbVisible = kbVisible.getValue();
            if (isKbVisible == null) isKbVisible = false;
            wasToggled = isEmojiPickerVisible || isKbVisible;

            if (isEmojiPickerVisible) {
                if (hasKbOpenedOnce && binding.emojiPicker.getTranslationY() != 0) {
                    inputHolderAnimationCallback.setShouldTranslate(false);
                    chatsAnimationCallback.setShouldTranslate(false);
                    emojiPickerAnimationCallback.setShouldTranslate(false);
                }
                // trigger ime.
                // Since the kb visibility listener will toggle the emojiPickerVisible live data, we do not explicitly toggle it here
                showKeyboard();
                return;
            }

            if (isKbVisible) {
                // hide the keyboard, but don't translate the views
                // Since the kb visibility listener will toggle the emojiPickerVisible live data, we do not explicitly toggle it here
                inputHolderAnimationCallback.setShouldTranslate(false);
                chatsAnimationCallback.setShouldTranslate(false);
                emojiPickerAnimationCallback.setShouldTranslate(false);
                hideKeyboard();
            }
            emojiPickerVisible.postValue(true);
        });
        final LiveData<Pair<Boolean, Boolean>> emojiKbVisibilityLD = Utils.zipLiveData(emojiPickerVisible, kbVisible);
        emojiKbVisibilityLD.observe(getViewLifecycleOwner(), pair -> {
            Boolean isEmojiPickerVisible = pair.first;
            Boolean isKbVisible = pair.second;
            if (isEmojiPickerVisible == null) isEmojiPickerVisible = false;
            if (isKbVisible == null) isKbVisible = false;
            root.setScrollImeOffScreenWhenVisible(!isEmojiPickerVisible);
            root.setScrollImeOnScreenWhenNotVisible(!isEmojiPickerVisible);
            onEmojiPickerBackPressedCallback.setEnabled(isEmojiPickerVisible && !isKbVisible);
            if (isEmojiPickerVisible && !isKbVisible) {
                animatePan(binding.emojiPicker.getMeasuredHeight(), unused -> {
                    binding.emojiPicker.setAlpha(1);
                    binding.emojiToggle.setIconResource(R.drawable.ic_keyboard_24);
                    return null;
                }, null);
                return;
            }
            if (!isEmojiPickerVisible && !isKbVisible) {
                animatePan(0, null, unused -> {
                    binding.emojiPicker.setAlpha(0);
                    binding.emojiToggle.setIconResource(R.drawable.ic_face_24);
                    return null;
                });
                return;
            }
            // isKbVisible will always be true going forward
            hasKbOpenedOnce = true;
            if (!isEmojiPickerVisible) {
                binding.emojiToggle.setIconResource(R.drawable.ic_face_24);
                binding.emojiPicker.setAlpha(0);
                return;
            }
            binding.emojiPicker.setAlpha(1);
        });
    }

    public void showKeyboard() {
        final Context context = getContext();
        if (context == null) return;
        final InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm == null) return;
        if (!binding.input.isFocused()) {
            binding.input.requestFocus();
        }
        final boolean shown = imm.showSoftInput(binding.input, InputMethodManager.SHOW_IMPLICIT);
        if (!shown) {
            Log.e(TAG, "showKeyboard: System did not display the keyboard");
        }
    }

    public void hideKeyboard() {
        final Context context = getContext();
        if (context == null) return;
        final InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm == null) return;
        imm.hideSoftInputFromWindow(binding.input.getWindowToken(), InputMethodManager.RESULT_UNCHANGED_SHOWN);
    }

    private void setSendToMicIcon() {
        final Context context = getContext();
        if (context == null) return;
        final Drawable sendToMicDrawable = Utils.getAnimatableDrawable(context, R.drawable.avd_send_to_mic_anim);
        if (sendToMicDrawable instanceof Animatable) {
            AnimatedVectorDrawableCompat.registerAnimationCallback(sendToMicDrawable, sendToMicAnimationCallback);
        }
        binding.send.setIcon(sendToMicDrawable);
    }

    private void setMicToSendIcon() {
        final Context context = getContext();
        if (context == null) return;
        final Drawable micToSendDrawable = Utils.getAnimatableDrawable(context, R.drawable.avd_mic_to_send_anim);
        if (micToSendDrawable instanceof Animatable) {
            AnimatedVectorDrawableCompat.registerAnimationCallback(micToSendDrawable, micToSendAnimationCallback);
        }
        binding.send.setIcon(micToSendDrawable);
    }

    private void setTitle(final String title) {
        if (actionBar == null) return;
        if (prevTitleRunnable != null) {
            appExecutors.mainThread().cancel(prevTitleRunnable);
        }
        prevTitleRunnable = () -> actionBar.setTitle(title);
        // set title delayed to avoid title blink if fetch is fast
        appExecutors.mainThread().execute(prevTitleRunnable, 1000);
    }

    private void downloadItem(final DirectItem item) {
        final Context context = getContext();
        if (context == null) return;
        final DirectItemType itemType = item.getItemType();
        //noinspection SwitchStatementWithTooFewBranches
        switch (itemType) {
            case VOICE_MEDIA:
                downloadItem(context, item.getVoiceMedia() == null ? null : item.getVoiceMedia().getMedia());
                break;
        }
    }

    // currently ONLY for voice
    private void downloadItem(@NonNull final Context context, final Media media) {
        if (media == null) {
            Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
            return;
        }
        if (ContextCompat.checkSelfPermission(context, DownloadUtils.PERMS[0]) == PackageManager.PERMISSION_GRANTED) {
            DownloadUtils.download(context, media);
            Toast.makeText(context, R.string.downloader_downloading_media, Toast.LENGTH_SHORT).show();
            return;
        }
        tempMedia = media;
        requestPermissions(DownloadUtils.PERMS, STORAGE_PERM_REQUEST_CODE);
    }

    @Nullable
    private User getUser(final long userId) {
        for (final User user : users) {
            if (userId != user.getPk()) continue;
            return user;
        }
        return null;
    }

    // Sets the translationY of views to height with animation
    private void animatePan(final int height,
                            @Nullable final Function<Void, Void> onAnimationStart,
                            @Nullable final Function<Void, Void> onAnimationEnd) {
        if (animatorSet != null && animatorSet.isStarted()) {
            animatorSet.cancel();
        }
        final ImmutableList.Builder<Animator> builder = ImmutableList.builder();
        builder.add(
                ObjectAnimator.ofFloat(binding.chats, TRANSLATION_Y, -height),
                ObjectAnimator.ofFloat(binding.inputHolder, TRANSLATION_Y, -height),
                ObjectAnimator.ofFloat(binding.emojiPicker, TRANSLATION_Y, -height)
        );
        // if (headerItemDecoration != null && headerItemDecoration.getCurrentHeader() != null) {
        //     builder.add(ObjectAnimator.ofFloat(headerItemDecoration.getCurrentHeader(), TRANSLATION_Y, height));
        // }
        animatorSet = new AnimatorSet();
        animatorSet.playTogether(builder.build());
        animatorSet.setDuration(200);
        animatorSet.setInterpolator(CubicBezierInterpolator.EASE_IN);
        animatorSet.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationStart(final Animator animation) {
                super.onAnimationStart(animation);
                if (onAnimationStart != null) {
                    onAnimationStart.apply(null);
                }
            }

            @Override
            public void onAnimationEnd(final Animator animation) {
                super.onAnimationEnd(animation);
                animatorSet = null;
                if (onAnimationEnd != null) {
                    onAnimationEnd.apply(null);
                }
            }
        });
        animatorSet.start();
    }

    private void showReactionsDialog(final DirectItem item) {
        final LiveData<List<User>> users = viewModel.getUsers();
        final LiveData<List<User>> leftUsers = viewModel.getLeftUsers();
        final ArrayList<User> allUsers = new ArrayList<>();
        allUsers.add(viewModel.getCurrentUser());
        if (users != null && users.getValue() != null) {
            allUsers.addAll(users.getValue());
        }
        if (leftUsers != null && leftUsers.getValue() != null) {
            allUsers.addAll(leftUsers.getValue());
        }
        reactionDialogFragment = DirectItemReactionDialogFragment
                .newInstance(viewModel.getViewerId(),
                             allUsers,
                             item.getItemId(),
                             item.getReactions());
        reactionDialogFragment.show(getChildFragmentManager(), "reactions_dialog");
    }

    @Override
    public void onReactionClick(final String itemId, final DirectItemEmojiReaction reaction) {
        if (reactionDialogFragment != null) {
            reactionDialogFragment.dismiss();
        }
        if (reaction == null) return;
        if (reaction.getSenderId() == viewModel.getViewerId()) {
            final LiveData<Resource<Object>> resourceLiveData = viewModel.sendDeleteReaction(itemId);
            if (resourceLiveData != null) {
                resourceLiveData.observe(getViewLifecycleOwner(), directItemResource -> handleSentMessage(resourceLiveData));
            }
            return;
        }
        // navigate to user
        final User user = viewModel.getUser(reaction.getSenderId());
        if (user == null) return;
        navigateToUser(user.getUsername());
    }

    private void navigateToUser(@NonNull final String username) {
        final ProfileNavGraphDirections.ActionGlobalProfileFragment direction = ProfileNavGraphDirections
                .actionGlobalProfileFragment("@" + username);
        NavHostFragment.findNavController(DirectMessageThreadFragment.this).navigate(direction);
    }

    @Override
    public void onClick(final View view, final Emoji emoji) {
        if (addReactionItem == null) return;
        final LiveData<Resource<Object>> resourceLiveData = viewModel.sendReaction(addReactionItem, emoji);
        if (resourceLiveData != null) {
            resourceLiveData.observe(getViewLifecycleOwner(), directItemResource -> handleSentMessage(resourceLiveData));
        }
    }
}
