package awais.instagrabber.fragments.directmessages;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import com.google.android.material.snackbar.Snackbar;

import java.util.Collections;
import java.util.List;

import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.DirectMessageInboxAdapter;
import awais.instagrabber.customviews.helpers.RecyclerLazyLoaderAtEdge;
import awais.instagrabber.databinding.FragmentDirectPendingInboxBinding;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.viewmodels.DirectPendingInboxViewModel;

public class DirectPendingInboxFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = DirectPendingInboxFragment.class.getSimpleName();

    private CoordinatorLayout root;
    private RecyclerLazyLoaderAtEdge lazyLoader;
    private DirectPendingInboxViewModel viewModel;
    private boolean shouldRefresh = true;
    private FragmentDirectPendingInboxBinding binding;
    private DirectMessageInboxAdapter inboxAdapter;
    private MainActivity fragmentActivity;
    private boolean scrollToTop = false;
    private boolean navigating;
    private Observer<List<DirectThread>> threadsObserver;

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = (MainActivity) getActivity();
        if (fragmentActivity != null) {
            viewModel = new ViewModelProvider(fragmentActivity).get(DirectPendingInboxViewModel.class);
        }
    }

    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             final ViewGroup container,
                             final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentDirectPendingInboxBinding.inflate(inflater, container, false);
        root = binding.getRoot();
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (!shouldRefresh) return;
        init();
    }

    @Override
    public void onRefresh() {
        lazyLoader.resetState();
        scrollToTop = true;
        if (viewModel != null) {
            viewModel.refresh();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        setupObservers();
    }

    @Override
    public void onConfigurationChanged(@NonNull final Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        init();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        removeViewModelObservers();
        viewModel.onDestroy();
    }

    private void setupObservers() {
        removeViewModelObservers();
        threadsObserver = list -> {
            if (inboxAdapter == null) return;
            if (binding.swipeRefreshLayout.getVisibility() == View.GONE) {
                binding.swipeRefreshLayout.setVisibility(View.VISIBLE);
                binding.empty.setVisibility(View.GONE);
            }
            inboxAdapter.submitList(list == null ? Collections.emptyList() : list, () -> {
                if (!scrollToTop) return;
                binding.pendingInboxList.smoothScrollToPosition(0);
                scrollToTop = false;
            });
            if (list == null || list.isEmpty()) {
                binding.swipeRefreshLayout.setVisibility(View.GONE);
                binding.empty.setVisibility(View.VISIBLE);
            }
        };
        viewModel.getThreads().observe(fragmentActivity, threadsObserver);
        viewModel.getInbox().observe(getViewLifecycleOwner(), inboxResource -> {
            if (inboxResource == null) return;
            switch (inboxResource.status) {
                case SUCCESS:
                    binding.swipeRefreshLayout.setRefreshing(false);
                    break;
                case ERROR:
                    if (inboxResource.message != null) {
                        Snackbar.make(binding.getRoot(), inboxResource.message, Snackbar.LENGTH_LONG).show();
                    }
                    binding.swipeRefreshLayout.setRefreshing(false);
                    break;
                case LOADING:
                    binding.swipeRefreshLayout.setRefreshing(true);
                    break;
            }
        });
    }

    private void removeViewModelObservers() {
        if (viewModel == null) return;
        if (threadsObserver != null) {
            viewModel.getThreads().removeObserver(threadsObserver);
        }
    }

    private void init() {
        final Context context = getContext();
        if (context == null) return;
        setupObservers();
        binding.swipeRefreshLayout.setOnRefreshListener(this);
        binding.pendingInboxList.setHasFixedSize(true);
        binding.pendingInboxList.setItemViewCacheSize(20);
        final LinearLayoutManager layoutManager = new LinearLayoutManager(context);
        binding.pendingInboxList.setLayoutManager(layoutManager);
        inboxAdapter = new DirectMessageInboxAdapter(thread -> {
            if (navigating) return;
            navigating = true;
            if (isAdded()) {
                final DirectPendingInboxFragmentDirections.ActionPendingInboxToThread directions = DirectPendingInboxFragmentDirections
                        .actionPendingInboxToThread(thread.getThreadId(), thread.getThreadTitle());
                directions.setPending(true);
                NavHostFragment.findNavController(this).navigate(directions);
            }
            navigating = false;
        });
        inboxAdapter.setHasStableIds(true);
        binding.pendingInboxList.setAdapter(inboxAdapter);
        lazyLoader = new RecyclerLazyLoaderAtEdge(layoutManager, page -> {
            if (viewModel == null) return;
            viewModel.fetchInbox();
        });
        binding.pendingInboxList.addOnScrollListener(lazyLoader);
    }
}
