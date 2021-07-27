package awais.instagrabber.fragments.search;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import java.io.Serializable;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

import awais.instagrabber.adapters.SearchItemsAdapter;
import awais.instagrabber.models.Resource;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.repositories.responses.search.SearchItem;
import awais.instagrabber.viewmodels.SearchFragmentViewModel;

public class SearchCategoryFragment extends Fragment {
    private static final String TAG = SearchCategoryFragment.class.getSimpleName();
    private static final String ARG_TYPE = "type";


    @Nullable
    private SwipeRefreshLayout swipeRefreshLayout;
    @Nullable
    private RecyclerView list;
    private SearchFragmentViewModel viewModel;
    private FavoriteType type;
    private SearchItemsAdapter searchItemsAdapter;
    @Nullable
    private OnSearchItemClickListener onSearchItemClickListener;
    private boolean skipViewRefresh;
    private String prevQuery;

    @NonNull
    public static SearchCategoryFragment newInstance(@NonNull final FavoriteType type) {
        final SearchCategoryFragment fragment = new SearchCategoryFragment();
        final Bundle args = new Bundle();
        args.putSerializable(ARG_TYPE, type);
        fragment.setArguments(args);
        return fragment;
    }

    public SearchCategoryFragment() {}

    @Override
    public void onAttach(@NonNull final Context context) {
        super.onAttach(context);
        final Fragment parentFragment = getParentFragment();
        if (!(parentFragment instanceof OnSearchItemClickListener)) return;
        onSearchItemClickListener = (OnSearchItemClickListener) parentFragment;
    }

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final FragmentActivity fragmentActivity = getActivity();
        if (fragmentActivity == null) return;
        viewModel = new ViewModelProvider(fragmentActivity).get(SearchFragmentViewModel.class);
        final Bundle args = getArguments();
        if (args == null) {
            Log.e(TAG, "onCreate: arguments are null");
            return;
        }
        final Serializable typeSerializable = args.getSerializable(ARG_TYPE);
        if (!(typeSerializable instanceof FavoriteType)) {
            Log.e(TAG, "onCreate: type not a FavoriteType");
            return;
        }
        type = (FavoriteType) typeSerializable;
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        final Context context = getContext();
        if (context == null) return null;
        skipViewRefresh = false;
        if (swipeRefreshLayout != null) {
            skipViewRefresh = true;
            return swipeRefreshLayout;
        }
        swipeRefreshLayout = new SwipeRefreshLayout(context);
        swipeRefreshLayout.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        list = new RecyclerView(context);
        list.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        swipeRefreshLayout.addView(list);
        return swipeRefreshLayout;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (skipViewRefresh) return;
        setupList();
    }

    @Override
    public void onResume() {
        super.onResume();
        // Log.d(TAG, "onResume: type: " + type);
        setupObservers();
        final String currentQuery = viewModel.getQuery().getValue();
        if (prevQuery != null && currentQuery != null && !Objects.equals(prevQuery, currentQuery)) {
            viewModel.search(currentQuery, type);
        }
        prevQuery = null;
    }

    private void setupList() {
        if (list == null || swipeRefreshLayout == null) return;
        final Context context = getContext();
        if (context == null) return;
        list.setLayoutManager(new LinearLayoutManager(context));
        searchItemsAdapter = new SearchItemsAdapter(onSearchItemClickListener);
        list.setAdapter(searchItemsAdapter);
        swipeRefreshLayout.setOnRefreshListener(() -> {
            String currentQuery = viewModel.getQuery().getValue();
            if (currentQuery == null) currentQuery = "";
            viewModel.search(currentQuery, type);
        });
    }

    private void setupObservers() {
        viewModel.getQuery().observe(getViewLifecycleOwner(), q -> {
            if (!isVisible() || Objects.equals(prevQuery, q)) return;
            viewModel.search(q, type);
            prevQuery = q;
        });
        final LiveData<Resource<List<SearchItem>>> resultsLiveData = getResultsLiveData();
        if (resultsLiveData != null) {
            resultsLiveData.observe(getViewLifecycleOwner(), this::onResults);
        }
    }

    private void onResults(final Resource<List<SearchItem>> listResource) {
        if (listResource == null) return;
        switch (listResource.status) {
            case SUCCESS:
                if (searchItemsAdapter != null) {
                    searchItemsAdapter.submitList(listResource.data);
                }
                if (swipeRefreshLayout != null) {
                    swipeRefreshLayout.setRefreshing(false);
                }
                break;
            case ERROR:
                if (searchItemsAdapter != null) {
                    searchItemsAdapter.submitList(Collections.emptyList());
                }
                if (swipeRefreshLayout != null) {
                    swipeRefreshLayout.setRefreshing(false);
                }
                break;
            case LOADING:
                if (swipeRefreshLayout != null) {
                    swipeRefreshLayout.setRefreshing(true);
                }
                break;
            default:
                break;
        }
    }

    @Nullable
    private LiveData<Resource<List<SearchItem>>> getResultsLiveData() {
        switch (type) {
            case TOP:
                return viewModel.getTopResults();
            case USER:
                return viewModel.getUserResults();
            case HASHTAG:
                return viewModel.getHashtagResults();
            case LOCATION:
                return viewModel.getLocationResults();
        }
        return null;
    }

    public interface OnSearchItemClickListener {
        void onSearchItemClick(SearchItem searchItem);

        void onSearchItemDelete(SearchItem searchItem);
    }
}
