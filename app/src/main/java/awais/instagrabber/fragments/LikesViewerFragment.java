package awais.instagrabber.fragments;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import com.google.android.material.bottomsheet.BottomSheetDialogFragment;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.LikesAdapter;
import awais.instagrabber.customviews.helpers.RecyclerLazyLoader;
import awais.instagrabber.databinding.FragmentLikesBinding;
import awais.instagrabber.repositories.responses.GraphQLUserListFetchResponse;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.webservices.GraphQLService;
import awais.instagrabber.webservices.MediaService;
import awais.instagrabber.webservices.ServiceCallback;

import static awais.instagrabber.utils.Utils.settingsHelper;

public final class LikesViewerFragment extends BottomSheetDialogFragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = LikesViewerFragment.class.getSimpleName();

    private FragmentLikesBinding binding;
    private RecyclerLazyLoader lazyLoader;
    private MediaService mediaService;
    private GraphQLService graphQLService;
    private boolean isLoggedIn;
    private String postId, endCursor;
    private boolean isComment;

    private final ServiceCallback<List<User>> cb = new ServiceCallback<List<User>>() {
        @Override
        public void onSuccess(final List<User> result) {
            final LikesAdapter likesAdapter = new LikesAdapter(result, v -> {
                final Object tag = v.getTag();
                if (tag instanceof User) {
                    User model = (User) tag;
                    final Bundle bundle = new Bundle();
                    bundle.putString("username", "@" + model.getUsername());
                    NavHostFragment.findNavController(LikesViewerFragment.this).navigate(R.id.action_global_profileFragment, bundle);
                }
            });
            binding.rvLikes.setAdapter(likesAdapter);
            binding.rvLikes.setLayoutManager(new LinearLayoutManager(getContext()));
            binding.rvLikes.addItemDecoration(new DividerItemDecoration(getContext(), DividerItemDecoration.VERTICAL));
            binding.swipeRefreshLayout.setRefreshing(false);
        }

        @Override
        public void onFailure(final Throwable t) {
            Log.e(TAG, "Error", t);
            try {
                final Context context = getContext();
                Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
            } catch (Exception ignored) {}
        }
    };

    private final ServiceCallback<GraphQLUserListFetchResponse> anonCb = new ServiceCallback<GraphQLUserListFetchResponse>() {
        @Override
        public void onSuccess(final GraphQLUserListFetchResponse result) {
            endCursor = result.getNextMaxId();
            final LikesAdapter likesAdapter = new LikesAdapter(result.getItems(), v -> {
                final Object tag = v.getTag();
                if (tag instanceof User) {
                    User model = (User) tag;
                    final Bundle bundle = new Bundle();
                    bundle.putString("username", "@" + model.getUsername());
                    NavHostFragment.findNavController(LikesViewerFragment.this).navigate(R.id.action_global_profileFragment, bundle);
                }
            });
            binding.rvLikes.setAdapter(likesAdapter);
            binding.swipeRefreshLayout.setRefreshing(false);
        }

        @Override
        public void onFailure(final Throwable t) {
            Log.e(TAG, "Error", t);
            try {
                final Context context = getContext();
                Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
            } catch (Exception ignored) {}
        }
    };

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) > 0;
        // final AppCompatActivity fragmentActivity = (AppCompatActivity) getActivity();
        mediaService = isLoggedIn ? MediaService.getInstance(null, null, 0) : null;
        graphQLService = isLoggedIn ? null : GraphQLService.getInstance();
        // setHasOptionsMenu(true);
    }

    @NonNull
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        binding = FragmentLikesBinding.inflate(getLayoutInflater());
        binding.swipeRefreshLayout.setEnabled(false);
        binding.swipeRefreshLayout.setNestedScrollingEnabled(false);
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        init();
    }

    @Override
    public void onRefresh() {
        if (isComment && !isLoggedIn) {
            lazyLoader.resetState();
            graphQLService.fetchCommentLikers(postId, null, anonCb);
        } else mediaService.fetchLikes(postId, isComment, cb);
    }

    private void init() {
        if (getArguments() == null) return;
        final LikesViewerFragmentArgs fragmentArgs = LikesViewerFragmentArgs.fromBundle(getArguments());
        postId = fragmentArgs.getPostId();
        isComment = fragmentArgs.getIsComment();
        binding.swipeRefreshLayout.setOnRefreshListener(this);
        binding.swipeRefreshLayout.setRefreshing(true);
        if (isComment && !isLoggedIn) {
            final LinearLayoutManager layoutManager = new LinearLayoutManager(getContext());
            binding.rvLikes.setLayoutManager(layoutManager);
            binding.rvLikes.addItemDecoration(new DividerItemDecoration(getContext(), DividerItemDecoration.HORIZONTAL));
            lazyLoader = new RecyclerLazyLoader(layoutManager, (page, totalItemsCount) -> {
                if (!TextUtils.isEmpty(endCursor))
                    graphQLService.fetchCommentLikers(postId, endCursor, anonCb);
                endCursor = null;
            });
            binding.rvLikes.addOnScrollListener(lazyLoader);
        }
        onRefresh();
    }
}