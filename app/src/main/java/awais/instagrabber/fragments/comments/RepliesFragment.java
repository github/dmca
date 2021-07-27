package awais.instagrabber.fragments.comments;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.google.android.material.snackbar.Snackbar;

import java.io.Serializable;
import java.util.Collections;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.CommentsAdapter;
import awais.instagrabber.customviews.helpers.RecyclerLazyLoader;
import awais.instagrabber.databinding.FragmentCommentsBinding;
import awais.instagrabber.models.Comment;
import awais.instagrabber.models.Resource;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.CommentsViewerViewModel;

public class RepliesFragment extends Fragment {
    public static final String TAG = RepliesFragment.class.getSimpleName();
    private static final String ARG_PARENT = "parent";
    private static final String ARG_FOCUS_INPUT = "focus";

    private FragmentCommentsBinding binding;
    private CommentsViewerViewModel viewModel;
    private CommentsAdapter commentsAdapter;

    @NonNull
    public static RepliesFragment newInstance(@NonNull final Comment parent,
                                              final boolean focusInput) {
        final Bundle args = new Bundle();
        args.putSerializable(ARG_PARENT, parent);
        args.putBoolean(ARG_FOCUS_INPUT, focusInput);
        final RepliesFragment fragment = new RepliesFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        viewModel = new ViewModelProvider(getParentFragment()).get(CommentsViewerViewModel.class);
        final Bundle bundle = getArguments();
        if (bundle == null) return;
        final Serializable serializable = bundle.getSerializable(ARG_PARENT);
        if (!(serializable instanceof Comment)) return;
        viewModel.showReplies((Comment) serializable);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        binding = FragmentCommentsBinding.inflate(inflater, container, false);
        binding.swipeRefreshLayout.setEnabled(false);
        binding.swipeRefreshLayout.setNestedScrollingEnabled(false);
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        setupToolbar();
    }

    @Override
    public Animation onCreateAnimation(int transit, boolean enter, int nextAnim) {
        if (!enter || nextAnim == 0) {
            return super.onCreateAnimation(transit, enter, nextAnim);
        }
        final Animation animation = AnimationUtils.loadAnimation(getContext(), nextAnim);
        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {}

            @Override
            public void onAnimationEnd(Animation animation) {
                setupList();
                setupObservers();
            }

            @Override
            public void onAnimationRepeat(Animation animation) {}
        });
        return animation;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        viewModel.clearReplies();
    }

    private void setupObservers() {
        viewModel.getCurrentUserId().observe(getViewLifecycleOwner(), currentUserId -> {
            long userId = 0;
            if (currentUserId != null) {
                userId = currentUserId;
            }
            setupAdapter(userId);
            if (userId == 0) return;
            Helper.setupCommentInput(binding.commentField, binding.commentText, true, text -> {
                final LiveData<Resource<Object>> resourceLiveData = viewModel.comment(text, true);
                resourceLiveData.observe(getViewLifecycleOwner(), new Observer<Resource<Object>>() {
                    @Override
                    public void onChanged(final Resource<Object> objectResource) {
                        if (objectResource == null) return;
                        final Context context = getContext();
                        if (context == null) return;
                        Helper.handleCommentResource(context,
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
            final Bundle bundle = getArguments();
            if (bundle == null) return;
            final boolean focusInput = bundle.getBoolean(ARG_FOCUS_INPUT);
            if (focusInput && viewModel.getRepliesParent() != null && viewModel.getRepliesParent().getUser() != null) {
                binding.commentText.setText(String.format("@%s ", viewModel.getRepliesParent().getUser().getUsername()));
                Utils.showKeyboard(binding.commentText);
            }
        });
        viewModel.getReplyList().observe(getViewLifecycleOwner(), listResource -> {
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
    }

    private void setupToolbar() {
        binding.toolbar.setTitle("Replies");
        binding.toolbar.setNavigationIcon(R.drawable.ic_round_arrow_back_24);
        binding.toolbar.setNavigationOnClickListener(v -> {
            final FragmentManager fragmentManager = getParentFragmentManager();
            fragmentManager.popBackStack();
        });
    }

    private void setupAdapter(final long currentUserId) {
        final Context context = getContext();
        if (context == null) return;
        commentsAdapter = new CommentsAdapter(currentUserId,
                                              true,
                                              Helper.getCommentCallback(context, getViewLifecycleOwner(), getNavController(), viewModel, null));
        binding.comments.setAdapter(commentsAdapter);
        final Resource<List<Comment>> listResource = viewModel.getReplyList().getValue();
        commentsAdapter.submitList(listResource != null ? listResource.data : Collections.emptyList());
    }

    private void setupList() {
        final Context context = getContext();
        if (context == null) return;
        final LinearLayoutManager layoutManager = new LinearLayoutManager(context);
        final RecyclerLazyLoader lazyLoader = new RecyclerLazyLoader(layoutManager, (page, totalItemsCount) -> viewModel.fetchReplies());
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
