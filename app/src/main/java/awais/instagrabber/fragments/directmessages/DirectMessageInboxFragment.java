package awais.instagrabber.fragments.directmessages;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.view.menu.ActionMenuItemView;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavDirections;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import com.google.android.material.badge.BadgeDrawable;
import com.google.android.material.badge.BadgeUtils;
import com.google.android.material.internal.ToolbarUtils;
import com.google.android.material.snackbar.Snackbar;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.DirectMessageInboxAdapter;
import awais.instagrabber.broadcasts.DMRefreshBroadcastReceiver;
import awais.instagrabber.customviews.helpers.RecyclerLazyLoaderAtEdge;
import awais.instagrabber.databinding.FragmentDirectMessagesInboxBinding;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.viewmodels.DirectInboxViewModel;

public class DirectMessageInboxFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = "DirectMessagesInboxFrag";

    private CoordinatorLayout root;
    private RecyclerLazyLoaderAtEdge lazyLoader;
    private DirectInboxViewModel viewModel;
    private boolean shouldRefresh = true;
    private FragmentDirectMessagesInboxBinding binding;
    private DMRefreshBroadcastReceiver receiver;
    private DirectMessageInboxAdapter inboxAdapter;
    private MainActivity fragmentActivity;
    private boolean scrollToTop = false;
    private boolean navigating;
    private Observer<List<DirectThread>> threadsObserver;
    private MenuItem pendingRequestsMenuItem;
    private BadgeDrawable pendingRequestTotalBadgeDrawable;
    private boolean isPendingRequestTotalBadgeAttached;

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = (MainActivity) getActivity();
        if (fragmentActivity != null) {
            viewModel = new ViewModelProvider(fragmentActivity).get(DirectInboxViewModel.class);
        }
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
        binding = FragmentDirectMessagesInboxBinding.inflate(inflater, container, false);
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

    @SuppressLint({"UnsafeExperimentalUsageError", "UnsafeOptInUsageError"})
    @Override
    public void onPause() {
        super.onPause();
        unregisterReceiver();
        isPendingRequestTotalBadgeAttached = false;
        @SuppressLint("RestrictedApi") final ActionMenuItemView menuItemView = ToolbarUtils
                .getActionMenuItemView(fragmentActivity.getToolbar(), pendingRequestsMenuItem.getItemId());
        if (pendingRequestTotalBadgeDrawable != null && menuItemView != null) {
            BadgeUtils.detachBadgeDrawable(pendingRequestTotalBadgeDrawable, fragmentActivity.getToolbar(), pendingRequestsMenuItem.getItemId());
            pendingRequestTotalBadgeDrawable = null;
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        setupObservers();
        final Context context = getContext();
        if (context == null) return;
        receiver = new DMRefreshBroadcastReceiver(() -> {
            Log.d(TAG, "onResume: broadcast received");
            // refreshInbox = true;
        });
        context.registerReceiver(receiver, new IntentFilter(DMRefreshBroadcastReceiver.ACTION_REFRESH_DM));
    }

    @SuppressLint("UnsafeExperimentalUsageError")
    @Override
    public void onDestroyView() {
        super.onDestroyView();
        unregisterReceiver();
    }

    @Override
    public void onCreateOptionsMenu(@NonNull final Menu menu, @NonNull final MenuInflater inflater) {
        inflater.inflate(R.menu.dm_inbox_menu, menu);
        pendingRequestsMenuItem = menu.findItem(R.id.pending_requests);
        pendingRequestsMenuItem.setVisible(isPendingRequestTotalBadgeAttached);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        if (item.getItemId() == R.id.pending_requests) {
            final NavDirections directions = DirectMessageInboxFragmentDirections.actionInboxToPendingInbox();
            try {
                NavHostFragment.findNavController(this).navigate(directions);
            } catch (Exception e) {
                Log.e(TAG, "onOptionsItemSelected: ", e);
            }
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void unregisterReceiver() {
        if (receiver == null) return;
        final Context context = getContext();
        if (context == null) return;
        context.unregisterReceiver(receiver);
        receiver = null;
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
            inboxAdapter.submitList(list, () -> {
                if (!scrollToTop) return;
                binding.inboxList.post(() -> binding.inboxList.smoothScrollToPosition(0));
                scrollToTop = false;
            });
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
                    if (inboxResource.resId != 0) {
                        Snackbar.make(binding.getRoot(), inboxResource.resId, Snackbar.LENGTH_LONG).show();
                    }
                    binding.swipeRefreshLayout.setRefreshing(false);
                    break;
                case LOADING:
                    binding.swipeRefreshLayout.setRefreshing(true);
                    break;
            }
        });
        viewModel.getPendingRequestsTotal().observe(getViewLifecycleOwner(), this::attachPendingRequestsBadge);
    }

    @SuppressLint({"UnsafeExperimentalUsageError", "UnsafeOptInUsageError"})
    private void attachPendingRequestsBadge(@Nullable final Integer count) {
        if (pendingRequestsMenuItem == null) {
            final Handler handler = new Handler();
            handler.postDelayed(() -> attachPendingRequestsBadge(count), 500);
            return;
        }
        if (pendingRequestTotalBadgeDrawable == null) {
            final Context context = getContext();
            if (context == null) return;
            pendingRequestTotalBadgeDrawable = BadgeDrawable.create(context);
        }
        if (count == null || count == 0) {
            @SuppressLint("RestrictedApi") final ActionMenuItemView menuItemView = ToolbarUtils
                    .getActionMenuItemView(fragmentActivity.getToolbar(), pendingRequestsMenuItem.getItemId());
            if (menuItemView != null) {
                BadgeUtils.detachBadgeDrawable(pendingRequestTotalBadgeDrawable, fragmentActivity.getToolbar(), pendingRequestsMenuItem.getItemId());
            }
            isPendingRequestTotalBadgeAttached = false;
            pendingRequestTotalBadgeDrawable.setNumber(0);
            pendingRequestsMenuItem.setVisible(false);
            return;
        }
        pendingRequestsMenuItem.setVisible(true);
        if (pendingRequestTotalBadgeDrawable.getNumber() == count) return;
        pendingRequestTotalBadgeDrawable.setNumber(count);
        if (!isPendingRequestTotalBadgeAttached) {
            BadgeUtils.attachBadgeDrawable(pendingRequestTotalBadgeDrawable, fragmentActivity.getToolbar(), pendingRequestsMenuItem.getItemId());
            isPendingRequestTotalBadgeAttached = true;
        }
    }

    private void removeViewModelObservers() {
        if (viewModel == null) return;
        if (threadsObserver != null) {
            viewModel.getThreads().removeObserver(threadsObserver);
        }
        // no need to explicitly remove observers whose lifecycle owner is getViewLifecycleOwner
    }

    private void init() {
        final Context context = getContext();
        if (context == null) return;
        setupObservers();
        binding.swipeRefreshLayout.setOnRefreshListener(this);
        binding.inboxList.setHasFixedSize(true);
        binding.inboxList.setItemViewCacheSize(20);
        final LinearLayoutManager layoutManager = new LinearLayoutManager(context);
        binding.inboxList.setLayoutManager(layoutManager);
        inboxAdapter = new DirectMessageInboxAdapter(thread -> {
            if (navigating) return;
            navigating = true;
            if (isAdded()) {
                final DirectMessageInboxFragmentDirections.ActionInboxToThread directions = DirectMessageInboxFragmentDirections
                        .actionInboxToThread(thread.getThreadId(), thread.getThreadTitle());
                try {
                    NavHostFragment.findNavController(this).navigate(directions);
                } catch (Exception e) {
                    Log.e(TAG, "init: ", e);
                }
            }
            navigating = false;
        });
        inboxAdapter.setHasStableIds(true);
        binding.inboxList.setAdapter(inboxAdapter);
        lazyLoader = new RecyclerLazyLoaderAtEdge(layoutManager, page -> {
            if (viewModel == null) return;
            viewModel.fetchInbox();
        });
        binding.inboxList.addOnScrollListener(lazyLoader);
    }
}
