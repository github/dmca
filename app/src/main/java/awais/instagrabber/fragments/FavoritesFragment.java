package awais.instagrabber.fragments;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.FavoritesAdapter;
import awais.instagrabber.databinding.FragmentFavoritesBinding;
import awais.instagrabber.db.datasources.FavoriteDataSource;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.db.repositories.FavoriteRepository;
import awais.instagrabber.db.repositories.RepositoryCallback;
import awais.instagrabber.viewmodels.FavoritesViewModel;

public class FavoritesFragment extends Fragment {
    private static final String TAG = "FavoritesFragment";

    private boolean shouldRefresh = true;
    private FragmentFavoritesBinding binding;
    private RecyclerView root;
    private FavoritesViewModel favoritesViewModel;
    private FavoritesAdapter adapter;
    private FavoriteRepository favoriteRepository;

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final Context context = getContext();
        if (context == null) return;
        favoriteRepository = FavoriteRepository.getInstance(FavoriteDataSource.getInstance(context));
    }

    @NonNull
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentFavoritesBinding.inflate(getLayoutInflater());
        root = binding.getRoot();
        binding.favoriteList.setLayoutManager(new LinearLayoutManager(getContext()));
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (!shouldRefresh) return;
        init();
        shouldRefresh = false;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (favoritesViewModel == null || adapter == null) return;
        // refresh list every time in onViewStateRestored since it is cheaper than implementing pull down to refresh
        favoritesViewModel.getList().observe(getViewLifecycleOwner(), adapter::submitList);
        favoriteRepository.getAllFavorites(new RepositoryCallback<List<Favorite>>() {
            @Override
            public void onSuccess(final List<Favorite> favorites) {
                favoritesViewModel.getList().postValue(favorites);
            }

            @Override
            public void onDataNotAvailable() {}
        });
    }

    private void init() {
        favoritesViewModel = new ViewModelProvider(this).get(FavoritesViewModel.class);
        adapter = new FavoritesAdapter(model -> {
            // navigate
            switch (model.getType()) {
                case USER: {
                    final String username = model.getQuery();
                    // Log.d(TAG, "username: " + username);
                    final NavController navController = NavHostFragment.findNavController(this);
                    final Bundle bundle = new Bundle();
                    bundle.putString("username", "@" + username);
                    navController.navigate(R.id.action_global_profileFragment, bundle);
                    break;
                }
                case LOCATION: {
                    final String locationId = model.getQuery();
                    // Log.d(TAG, "locationId: " + locationId);
                    final NavController navController = NavHostFragment.findNavController(this);
                    final Bundle bundle = new Bundle();
                    try {
                        bundle.putLong("locationId", Long.parseLong(locationId));
                        navController.navigate(R.id.action_global_locationFragment, bundle);
                    } catch (Exception e) {
                        Log.e(TAG, "init: ", e);
                        return;
                    }
                    break;
                }
                case HASHTAG: {
                    final String hashtag = model.getQuery();
                    // Log.d(TAG, "hashtag: " + hashtag);
                    final NavController navController = NavHostFragment.findNavController(this);
                    final Bundle bundle = new Bundle();
                    bundle.putString("hashtag", "#" + hashtag);
                    navController.navigate(R.id.action_global_hashTagFragment, bundle);
                    break;
                }
                default:
                    // do nothing
            }
        }, model -> {
            // delete
            final Context context = getContext();
            if (context == null) return false;
            new MaterialAlertDialogBuilder(context)
                    .setMessage(getString(R.string.quick_access_confirm_delete, model.getQuery()))
                    .setPositiveButton(R.string.yes, (d, which) -> favoriteRepository
                            .deleteFavorite(model.getQuery(), model.getType(), new RepositoryCallback<Void>() {
                                @Override
                                public void onSuccess(final Void result) {
                                    d.dismiss();
                                    favoriteRepository.getAllFavorites(new RepositoryCallback<List<Favorite>>() {
                                        @Override
                                        public void onSuccess(final List<Favorite> result) {
                                            favoritesViewModel.getList().postValue(result);
                                        }

                                        @Override
                                        public void onDataNotAvailable() {}
                                    });
                                }

                                @Override
                                public void onDataNotAvailable() {}
                            }))
                    .setNegativeButton(R.string.no, null)
                    .show();
            return true;
        });
        binding.favoriteList.setAdapter(adapter);

    }
}
