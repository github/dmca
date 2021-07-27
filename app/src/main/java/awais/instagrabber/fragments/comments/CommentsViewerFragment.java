package awais.instagrabber.fragments.comments;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.RelativeSizeSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.google.android.material.bottomsheet.BottomSheetBehavior;
import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.bottomsheet.BottomSheetDialogFragment;
import com.google.android.material.snackbar.Snackbar;

import java.util.Collections;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.CommentsAdapter;
import awais.instagrabber.customviews.helpers.RecyclerLazyLoader;
import awais.instagrabber.databinding.FragmentCommentsBinding;
import awais.instagrabber.models.Comment;
import awais.instagrabber.models.Resource;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.viewmodels.AppStateViewModel;
import awais.instagrabber.viewmodels.CommentsViewerViewModel;

public final class CommentsViewerFragment extends BottomSheetDialogFragment {
    private static final String TAG = CommentsViewerFragment.class.getSimpleName();

    private CommentsViewerViewModel viewModel;
    private CommentsAdapter commentsAdapter;
    private FragmentCommentsBinding binding;
    private ConstraintLayout root;
    private boolean shouldRefresh = true;
    private AppStateViewModel appStateViewModel;
    private boolean showingReplies;

    @Override
    public void onStart() {
        super.onStart();
        final Dialog dialog = getDialog();
        if (dialog == null) return;
        final BottomSheetDialog bottomSheetDialog = (BottomSheetDialog) dialog;
        final View bottomSheetInternal = bottomSheetDialog.findViewById(com.google.android.material.R.id.design_bottom_sheet);
        if (bottomSheetInternal == null) return;
        bottomSheetInternal.getLayoutParams().height = ViewGroup.LayoutParams.MATCH_PARENT;
        bottomSheetInternal.requestLayout();
        final BottomSheetBehavior<View> behavior = BottomSheetBehavior.from(bottomSheetInternal);
        behavior.setState(BottomSheetBehavior.STATE_EXPANDED);
        behavior.setSkipCollapsed(true);
    }

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final FragmentActivity activity = getActivity();
        if (activity == null) return;
        viewModel = new ViewModelProvider(this).get(CommentsViewerViewModel.class);
        appStateViewModel = new ViewModelProvider(activity).get(AppStateViewModel.class);
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(@Nullable final Bundle savedInstanceState) {
        return new BottomSheetDialog(getContext(), getTheme()) {
            @Override
            public void onBackPressed() {
                if (showingReplies) {
                    getChildFragmentManager().popBackStack();
                    showingReplies = false;
                    return;
                }
                super.onBackPressed();
            }
        };
    }

    @NonNull
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentCommentsBinding.inflate(getLayoutInflater());
        binding.swipeRefreshLayout.setEnabled(false);
        binding.swipeRefreshLayout.setNestedScrollingEnabled(false);
        root = binding.getRoot();
        appStateViewModel.getCurrentUserLiveData().observe(getViewLifecycleOwner(), user -> viewModel.setCurrentUser(user));
        if (getArguments() == null) return root;
        final CommentsViewerFragmentArgs args = CommentsViewerFragmentArgs.fromBundle(getArguments());
        viewModel.setPostDetails(args.getShortCode(), args.getPostId(), args.getPostUserId());
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (!shouldRefresh) return;
        shouldRefresh = false;
        init();
    }

    private void init() {
        setupToolbar();
        setupList();
        setupObservers();
    }

    private void setupObservers() {
        viewModel.getCurrentUserId().observe(getViewLifecycleOwner(), currentUserId -> {
            long userId = 0;
            if (currentUserId != null) {
                userId = currentUserId;
            }
            setupAdapter(userId);
            if (userId == 0) return;
            Helper.setupCommentInput(binding.commentField, binding.commentText, false, text -> {
                final LiveData<Resource<Object>> resourceLiveData = viewModel.comment(text, false);
                resourceLiveData.observe(getViewLifecycleOwner(), new Observer<Resource<Object>>() {
                    @Override
                    public void onChanged(final Resource<Object> objectResource) {
                        if (objectResource == null) return;
                        final Context context = getContext();
                        if (context == null) return;
                        Helper.handleCommentResource(
                                context,
                                objectResource.status,
                                objectResource.message,
                                resourceLiveData,
                                this,
                                binding.commentField,
                                binding.commentText,
                                binding.comments);
                    }
                });
                return null;
            });
        });
        viewModel.getRootList().observe(getViewLifecycleOwner(), listResource -> {
            if (listResource == null) return;
            switch (listResource.status) {
                case SUCCESS:
                    binding.swipeRefreshLayout.setRefreshing(false);
                    if (commentsAdapter != null) {
                        commentsAdapter.submitList(listResource.data);
                    }
                    break;
                case ERROR:
                    binding.swipeRefreshLayout.setRefreshing(false);
                    if (!TextUtils.isEmpty(listResource.message)) {
                        Snackbar.make(binding.getRoot(), listResource.message, Snackbar.LENGTH_LONG).show();
                    }
                    break;
                case LOADING:
                    binding.swipeRefreshLayout.setRefreshing(true);
                    break;
            }
        });
        viewModel.getRootCommentsCount().observe(getViewLifecycleOwner(), count -> {
            if (count == null || count == 0) {
                binding.toolbar.setTitle(R.string.title_comments);
                return;
            }
            final String titleComments = getString(R.string.title_comments);
            final String countString = String.valueOf(count);
            final SpannableString titleWithCount = new SpannableString(String.format("%s   %s", titleComments, countString));
            titleWithCount.setSpan(new RelativeSizeSpan(0.8f),
                                   titleWithCount.length() - countString.length(),
                                   titleWithCount.length(),
                                   Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            binding.toolbar.setTitle(titleWithCount);
        });
    }

    private void setupToolbar() {
        binding.toolbar.setTitle(R.string.title_comments);
    }

    private void setupAdapter(final long currentUserId) {
        final Context context = getContext();
        if (context == null) return;
        commentsAdapter = new CommentsAdapter(currentUserId, false, Helper.getCommentCallback(
                context,
                getViewLifecycleOwner(),
                getNavController(),
                viewModel,
                (comment, focusInput) -> {
                    if (comment == null) return null;
                    final RepliesFragment repliesFragment = RepliesFragment.newInstance(comment, focusInput == null ? false : focusInput);
                    getChildFragmentManager().beginTransaction()
                                             .setCustomAnimations(R.anim.slide_left, R.anim.slide_right, 0, R.anim.slide_right)
                                             .add(R.id.replies_container_view, repliesFragment)
                                             .addToBackStack(RepliesFragment.TAG)
                                             .commit();
                    showingReplies = true;
                    return null;
                }));
        final Resource<List<Comment>> listResource = viewModel.getRootList().getValue();
        binding.comments.setAdapter(commentsAdapter);
        commentsAdapter.submitList(listResource != null ? listResource.data : Collections.emptyList());
    }

    private void setupList() {
        final Context context = getContext();
        if (context == null) return;
        final LinearLayoutManager layoutManager = new LinearLayoutManager(context);
        final RecyclerLazyLoader lazyLoader = new RecyclerLazyLoader(layoutManager, (page, totalItemsCount) -> viewModel.fetchComments());
        Helper.setupList(context, binding.comments, layoutManager, lazyLoader);
    }

    @Nullable
    private NavController getNavController() {
        NavController navController = null;
        try {
            navController = NavHostFragment.findNavController(this);
        } catch (IllegalStateException e) {
            Log.e(TAG, "navigateToProfile", e);
        }
        return navController;
    }
}