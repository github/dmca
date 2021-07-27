package awais.instagrabber.fragments.search;

import android.content.Context;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.LinearLayoutCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;

import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.tabs.TabLayoutMediator;

import java.util.Arrays;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.SearchCategoryAdapter;
import awais.instagrabber.customviews.helpers.TextWatcherAdapter;
import awais.instagrabber.databinding.FragmentSearchBinding;
import awais.instagrabber.models.Resource;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.repositories.responses.search.SearchItem;
import awais.instagrabber.viewmodels.SearchFragmentViewModel;

import static awais.instagrabber.fragments.settings.PreferenceKeys.PREF_SEARCH_FOCUS_KEYBOARD;
import static awais.instagrabber.utils.Utils.settingsHelper;

public class SearchFragment extends Fragment implements SearchCategoryFragment.OnSearchItemClickListener {
    private static final String TAG = SearchFragment.class.getSimpleName();
    private static final String QUERY = "query";

    private FragmentSearchBinding binding;
    private LinearLayoutCompat root;
    private boolean shouldRefresh = true;
    @Nullable
    private EditText searchInput;
    @Nullable
    private MainActivity mainActivity;
    private SearchFragmentViewModel viewModel;

    private final TextWatcherAdapter textWatcher = new TextWatcherAdapter() {
        @Override
        public void afterTextChanged(final Editable s) {
            if (s == null) return;
            viewModel.submitQuery(s.toString().trim());
        }
    };

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final FragmentActivity fragmentActivity = getActivity();
        if (!(fragmentActivity instanceof MainActivity)) return;
        mainActivity = (MainActivity) fragmentActivity;
        viewModel = new ViewModelProvider(mainActivity).get(SearchFragmentViewModel.class);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentSearchBinding.inflate(inflater, container, false);
        root = binding.getRoot();
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (!shouldRefresh) return;
        init(savedInstanceState);
        shouldRefresh = false;
    }

    @Override
    public void onSaveInstanceState(@NonNull final Bundle outState) {
        super.onSaveInstanceState(outState);
        final String current = viewModel.getQuery().getValue();
        if (TextUtils.isEmpty(current)) return;
        outState.putString(QUERY, current);
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mainActivity != null) {
            mainActivity.hideSearchView();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mainActivity != null) {
            mainActivity.hideSearchView();
        }
        if (searchInput != null) {
            searchInput.removeTextChangedListener(textWatcher);
            searchInput.setText("");
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mainActivity != null) {
            mainActivity.showSearchView();
        }
        if (settingsHelper.getBoolean(PREF_SEARCH_FOCUS_KEYBOARD)) {
            searchInput.requestFocus();
            final InputMethodManager imm = (InputMethodManager) requireContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) imm.showSoftInput(searchInput, InputMethodManager.SHOW_IMPLICIT);
        }
    }

    private void init(@Nullable final Bundle savedInstanceState) {
        if (mainActivity == null) return;
        searchInput = mainActivity.showSearchView().getEditText();
        setupObservers();
        setupViewPager();
        setupSearchInput(savedInstanceState);
    }

    private void setupObservers() {
        viewModel.getQuery().observe(getViewLifecycleOwner(), q -> {}); // need to observe, so that getQuery returns proper value
    }

    private void setupSearchInput(@Nullable final Bundle savedInstanceState) {
        if (searchInput == null) return;
        searchInput.removeTextChangedListener(textWatcher); // make sure we add only 1 instance of textWatcher
        searchInput.addTextChangedListener(textWatcher);
        boolean triggerEmptyQuery = true;
        if (savedInstanceState != null) {
            final String savedQuery = savedInstanceState.getString(QUERY);
            if (TextUtils.isEmpty(savedQuery)) return;
            searchInput.setText(savedQuery);
            triggerEmptyQuery = false;
        }
        if (settingsHelper.getBoolean(PREF_SEARCH_FOCUS_KEYBOARD)) {
            searchInput.requestFocus();
            final InputMethodManager imm = (InputMethodManager) requireContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) imm.showSoftInput(searchInput, InputMethodManager.SHOW_IMPLICIT);
        }
        if (triggerEmptyQuery) {
            viewModel.submitQuery("");
        }
    }

    private void setupViewPager() {
        binding.pager.setSaveEnabled(false);
        final List<FavoriteType> categories = Arrays.asList(FavoriteType.values());
        binding.pager.setAdapter(new SearchCategoryAdapter(this, categories));
        final TabLayoutMediator mediator = new TabLayoutMediator(binding.tabLayout, binding.pager, (tab, position) -> {
            try {
                final FavoriteType type = categories.get(position);
                final int resId;
                switch (type) {
                    case TOP:
                        resId = R.string.top;
                        break;
                    case USER:
                        resId = R.string.accounts;
                        break;
                    case HASHTAG:
                        resId = R.string.hashtags;
                        break;
                    case LOCATION:
                        resId = R.string.locations;
                        break;
                    default:
                        throw new IllegalStateException("Unexpected value: " + type);
                }
                tab.setText(resId);
            } catch (Exception e) {
                Log.e(TAG, "setupViewPager: ", e);
            }
        });
        mediator.attach();
    }

    @Override
    public void onSearchItemClick(final SearchItem searchItem) {
        if (searchItem == null) return;
        final FavoriteType type = searchItem.getType();
        if (type == null) return;
        try {
            if (!searchItem.isFavorite()) {
                viewModel.saveToRecentSearches(searchItem); // insert or update recent
            }
            final NavController navController = NavHostFragment.findNavController(this);
            final Bundle bundle = new Bundle();
            switch (type) {
                case USER:
                    bundle.putString("username", searchItem.getUser().getUsername());
                    navController.navigate(R.id.action_global_profileFragment, bundle);
                    break;
                case HASHTAG:
                    bundle.putString("hashtag", searchItem.getHashtag().getName());
                    navController.navigate(R.id.action_global_hashTagFragment, bundle);
                    break;
                case LOCATION:
                    bundle.putLong("locationId", searchItem.getPlace().getLocation().getPk());
                    navController.navigate(R.id.action_global_locationFragment, bundle);
                    break;
                default:
                    break;
            }
        } catch (Exception e) {
            Log.e(TAG, "onSearchItemClick: ", e);
        }
    }

    @Override
    public void onSearchItemDelete(final SearchItem searchItem) {
        final LiveData<Resource<Object>> liveData = viewModel.deleteRecentSearch(searchItem);
        if (liveData == null) return;
        liveData.observe(getViewLifecycleOwner(), new Observer<Resource<Object>>() {
            @Override
            public void onChanged(final Resource<Object> resource) {
                if (resource == null) return;
                switch (resource.status) {
                    case SUCCESS:
                        viewModel.search("", FavoriteType.TOP);
                        liveData.removeObserver(this);
                        break;
                    case ERROR:
                        Snackbar.make(binding.getRoot(), R.string.error, Snackbar.LENGTH_SHORT).show();
                        liveData.removeObserver(this);
                        break;
                    case LOADING:
                    default:
                        break;
                }
            }
        });
    }
}