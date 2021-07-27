package awais.instagrabber.fragments;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.SavedStateHandle;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavBackStackEntry;
import androidx.navigation.NavController;
import androidx.navigation.fragment.FragmentNavigator;
import androidx.navigation.fragment.NavHostFragment;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.SavedCollectionsAdapter;
import awais.instagrabber.customviews.helpers.GridSpacingItemDecoration;
import awais.instagrabber.databinding.FragmentSavedCollectionsBinding;
import awais.instagrabber.repositories.responses.saved.CollectionsListResponse;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.SavedCollectionsViewModel;
import awais.instagrabber.webservices.ProfileService;
import awais.instagrabber.webservices.ServiceCallback;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class SavedCollectionsFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = SavedCollectionsFragment.class.getSimpleName();
    public static boolean pleaseRefresh = false;

    private MainActivity fragmentActivity;
    private CoordinatorLayout root;
    private FragmentSavedCollectionsBinding binding;
    private SavedCollectionsViewModel savedCollectionsViewModel;
    private boolean shouldRefresh = true;
    private boolean isSaving;
    private ProfileService profileService;

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = (MainActivity) requireActivity();
        profileService = ProfileService.getInstance();
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
        binding = FragmentSavedCollectionsBinding.inflate(inflater, container, false);
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
        inflater.inflate(R.menu.saved_collection_menu, menu);
    }

    @Override
    public void onResume() {
        super.onResume();
        if (pleaseRefresh) onRefresh();
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        if (item.getItemId() == R.id.add) {
            final Context context = getContext();
            final EditText input = new EditText(context);
            new AlertDialog.Builder(context)
                    .setTitle(R.string.saved_create_collection)
                    .setView(input)
                    .setPositiveButton(R.string.confirm, (d, w) -> {
                        final String cookie = settingsHelper.getString(Constants.COOKIE);
                        profileService.createCollection(
                                input.getText().toString(),
                                settingsHelper.getString(Constants.DEVICE_UUID),
                                CookieUtils.getUserIdFromCookie(cookie),
                                CookieUtils.getCsrfTokenFromCookie(cookie),
                                new ServiceCallback<String>() {
                                    @Override
                                    public void onSuccess(final String result) {
                                        onRefresh();
                                    }

                                    @Override
                                    public void onFailure(final Throwable t) {
                                        Log.e(TAG, "Error creating collection", t);
                                        Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                                    }
                                });
                    })
                    .setNegativeButton(R.string.cancel, null)
                    .show();
            return true;
        }
        return false;
    }

    private void init() {
        setupTopics();
        fetchTopics(null);
        final SavedCollectionsFragmentArgs fragmentArgs = SavedCollectionsFragmentArgs.fromBundle(getArguments());
        isSaving = fragmentArgs.getIsSaving();
    }

    @Override
    public void onRefresh() {
        fetchTopics(null);
    }

    public void setupTopics() {
        savedCollectionsViewModel = new ViewModelProvider(fragmentActivity).get(SavedCollectionsViewModel.class);
        binding.topicsRecyclerView.addItemDecoration(new GridSpacingItemDecoration(Utils.convertDpToPx(2)));
        final SavedCollectionsAdapter adapter = new SavedCollectionsAdapter((topicCluster, root, cover, title, titleColor, backgroundColor) -> {
            final NavController navController = NavHostFragment.findNavController(this);
            if (isSaving) {
                setNavControllerResult(navController, topicCluster.getId());
                navController.navigateUp();
            } else {
                try {
                    final FragmentNavigator.Extras.Builder builder = new FragmentNavigator.Extras.Builder()
                            .addSharedElement(cover, "collection-" + topicCluster.getId());
                    final SavedCollectionsFragmentDirections.ActionSavedCollectionsFragmentToCollectionPostsFragment action = SavedCollectionsFragmentDirections
                            .actionSavedCollectionsFragmentToCollectionPostsFragment(topicCluster, titleColor, backgroundColor);
                    navController.navigate(action, builder.build());
                } catch (Exception e) {
                    Log.e(TAG, "setupTopics: ", e);
                }
            }
        });
        binding.topicsRecyclerView.setAdapter(adapter);
        savedCollectionsViewModel.getList().observe(getViewLifecycleOwner(), adapter::submitList);
    }

    private void fetchTopics(final String maxId) {
        binding.swipeRefreshLayout.setRefreshing(true);
        profileService.fetchCollections(maxId, new ServiceCallback<CollectionsListResponse>() {
            @Override
            public void onSuccess(final CollectionsListResponse result) {
                if (result == null) return;
                savedCollectionsViewModel.getList().postValue(result.getItems());
                binding.swipeRefreshLayout.setRefreshing(false);
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "onFailure", t);
                binding.swipeRefreshLayout.setRefreshing(false);
            }
        });
    }

    private void setNavControllerResult(@NonNull final NavController navController, final String result) {
        final NavBackStackEntry navBackStackEntry = navController.getPreviousBackStackEntry();
        if (navBackStackEntry == null) return;
        final SavedStateHandle savedStateHandle = navBackStackEntry.getSavedStateHandle();
        savedStateHandle.set("collection", result);
    }
}
