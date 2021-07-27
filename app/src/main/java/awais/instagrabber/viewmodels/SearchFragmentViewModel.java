package awais.instagrabber.viewmodels;

import android.app.Application;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.google.common.collect.ImmutableList;
import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.SettableFuture;

import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.db.datasources.FavoriteDataSource;
import awais.instagrabber.db.datasources.RecentSearchDataSource;
import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.db.entities.RecentSearch;
import awais.instagrabber.db.repositories.FavoriteRepository;
import awais.instagrabber.db.repositories.RecentSearchRepository;
import awais.instagrabber.db.repositories.RepositoryCallback;
import awais.instagrabber.models.Resource;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.repositories.responses.search.SearchItem;
import awais.instagrabber.repositories.responses.search.SearchResponse;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.Debouncer;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.webservices.SearchService;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import static androidx.lifecycle.Transformations.distinctUntilChanged;
import static awais.instagrabber.utils.Utils.settingsHelper;

public class SearchFragmentViewModel extends AppStateViewModel {
    private static final String TAG = SearchFragmentViewModel.class.getSimpleName();
    private static final String QUERY = "query";

    private final MutableLiveData<String> query = new MutableLiveData<>();
    private final MutableLiveData<Resource<List<SearchItem>>> topResults = new MutableLiveData<>();
    private final MutableLiveData<Resource<List<SearchItem>>> userResults = new MutableLiveData<>();
    private final MutableLiveData<Resource<List<SearchItem>>> hashtagResults = new MutableLiveData<>();
    private final MutableLiveData<Resource<List<SearchItem>>> locationResults = new MutableLiveData<>();

    private final SearchService searchService;
    private final Debouncer<String> searchDebouncer;
    private final boolean isLoggedIn;
    private final LiveData<String> distinctQuery;
    private final RecentSearchRepository recentSearchRepository;
    private final FavoriteRepository favoriteRepository;

    private String tempQuery;

    public SearchFragmentViewModel(@NonNull final Application application) {
        super(application);
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) != 0;
        final Debouncer.Callback<String> searchCallback = new Debouncer.Callback<String>() {
            @Override
            public void call(final String key) {
                if (tempQuery == null) return;
                query.postValue(tempQuery);
            }

            @Override
            public void onError(final Throwable t) {
                Log.e(TAG, "onError: ", t);
            }
        };
        searchDebouncer = new Debouncer<>(searchCallback, 500);
        distinctQuery = distinctUntilChanged(query);
        searchService = SearchService.getInstance();
        recentSearchRepository = RecentSearchRepository.getInstance(RecentSearchDataSource.getInstance(application));
        favoriteRepository = FavoriteRepository.getInstance(FavoriteDataSource.getInstance(application));
    }

    public LiveData<String> getQuery() {
        return distinctQuery;
    }

    public LiveData<Resource<List<SearchItem>>> getTopResults() {
        return topResults;
    }

    public LiveData<Resource<List<SearchItem>>> getUserResults() {
        return userResults;
    }

    public LiveData<Resource<List<SearchItem>>> getHashtagResults() {
        return hashtagResults;
    }

    public LiveData<Resource<List<SearchItem>>> getLocationResults() {
        return locationResults;
    }

    public void submitQuery(@Nullable final String query) {
        String localQuery = query;
        if (query == null) {
            localQuery = "";
        }
        if (tempQuery != null && Objects.equals(localQuery.toLowerCase(), tempQuery.toLowerCase())) return;
        tempQuery = query;
        if (TextUtils.isEmpty(query)) {
            // If empty immediately post it
            searchDebouncer.cancel(QUERY);
            this.query.postValue("");
            return;
        }
        searchDebouncer.call(QUERY);
    }

    public void search(@NonNull final String query,
                       @NonNull final FavoriteType type) {
        final MutableLiveData<Resource<List<SearchItem>>> liveData = getLiveDataByType(type);
        if (liveData == null) return;
        if (TextUtils.isEmpty(query)) {
            if (type != FavoriteType.TOP) {
                liveData.postValue(Resource.success(Collections.emptyList()));
                return;
            }
            showRecentSearchesAndFavorites();
            return;
        }
        if (query.equals("@") || query.equals("#")) return;
        final String c;
        switch (type) {
            case TOP:
                c = "blended";
                break;
            case USER:
                c = "user";
                break;
            case HASHTAG:
                c = "hashtag";
                break;
            case LOCATION:
                c = "place";
                break;
            default:
                return;
        }
        liveData.postValue(Resource.loading(null));
        final Call<SearchResponse> request = searchService.search(isLoggedIn, query, c);
        request.enqueue(new Callback<SearchResponse>() {
            @Override
            public void onResponse(@NonNull final Call<SearchResponse> call,
                                   @NonNull final Response<SearchResponse> response) {
                if (!response.isSuccessful()) {
                    sendErrorResponse(type);
                    return;
                }
                final SearchResponse body = response.body();
                if (body == null) {
                    sendErrorResponse(type);
                    return;
                }
                parseResponse(body, type);
            }

            @Override
            public void onFailure(@NonNull final Call<SearchResponse> call,
                                  @NonNull final Throwable t) {
                Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    private void showRecentSearchesAndFavorites() {
        final SettableFuture<List<RecentSearch>> recentResultsFuture = SettableFuture.create();
        final SettableFuture<List<Favorite>> favoritesFuture = SettableFuture.create();
        recentSearchRepository.getAllRecentSearches(new RepositoryCallback<List<RecentSearch>>() {
            @Override
            public void onSuccess(final List<RecentSearch> result) {
                recentResultsFuture.set(result);
            }

            @Override
            public void onDataNotAvailable() {
                recentResultsFuture.set(Collections.emptyList());
            }
        });
        favoriteRepository.getAllFavorites(new RepositoryCallback<List<Favorite>>() {
            @Override
            public void onSuccess(final List<Favorite> result) {
                favoritesFuture.set(result);
            }

            @Override
            public void onDataNotAvailable() {
                favoritesFuture.set(Collections.emptyList());
            }
        });
        //noinspection UnstableApiUsage
        final ListenableFuture<List<List<?>>> listenableFuture = Futures.allAsList(recentResultsFuture, favoritesFuture);
        Futures.addCallback(listenableFuture, new FutureCallback<List<List<?>>>() {
            @Override
            public void onSuccess(@Nullable final List<List<?>> result) {
                if (!TextUtils.isEmpty(tempQuery)) return; // Make sure user has not entered anything before updating results
                if (result == null) {
                    topResults.postValue(Resource.success(Collections.emptyList()));
                    return;
                }
                try {
                    //noinspection unchecked
                    topResults.postValue(Resource.success(
                            ImmutableList.<SearchItem>builder()
                                    .addAll(SearchItem.fromRecentSearch((List<RecentSearch>) result.get(0)))
                                    .addAll(SearchItem.fromFavorite((List<Favorite>) result.get(1)))
                                    .build()
                    ));
                } catch (Exception e) {
                    Log.e(TAG, "onSuccess: ", e);
                    topResults.postValue(Resource.success(Collections.emptyList()));
                }
            }

            @Override
            public void onFailure(@NonNull final Throwable t) {
                if (!TextUtils.isEmpty(tempQuery)) return;
                topResults.postValue(Resource.success(Collections.emptyList()));
                Log.e(TAG, "onFailure: ", t);
            }
        }, AppExecutors.getInstance().mainThread());
    }

    private void sendErrorResponse(@NonNull final FavoriteType type) {
        final MutableLiveData<Resource<List<SearchItem>>> liveData = getLiveDataByType(type);
        if (liveData == null) return;
        liveData.postValue(Resource.error(null, Collections.emptyList()));
    }

    private MutableLiveData<Resource<List<SearchItem>>> getLiveDataByType(@NonNull final FavoriteType type) {
        final MutableLiveData<Resource<List<SearchItem>>> liveData;
        switch (type) {
            case TOP:
                liveData = topResults;
                break;
            case USER:
                liveData = userResults;
                break;
            case HASHTAG:
                liveData = hashtagResults;
                break;
            case LOCATION:
                liveData = locationResults;
                break;
            default:
                return null;
        }
        return liveData;
    }

    private void parseResponse(@NonNull final SearchResponse body,
                               @NonNull final FavoriteType type) {
        final MutableLiveData<Resource<List<SearchItem>>> liveData = getLiveDataByType(type);
        if (liveData == null) return;
        if (isLoggedIn) {
            if (body.getList() == null) {
                liveData.postValue(Resource.success(Collections.emptyList()));
                return;
            }
            if (type == FavoriteType.HASHTAG || type == FavoriteType.LOCATION) {
                liveData.postValue(Resource.success(body.getList()
                                                        .stream()
                                                        .filter(i -> i.getUser() == null)
                                                        .collect(Collectors.toList())));
                return;
            }
            liveData.postValue(Resource.success(body.getList()));
            return;
        }

        // anonymous
        final List<SearchItem> list;
        switch (type) {
            case TOP:
                list = ImmutableList
                        .<SearchItem>builder()
                        .addAll(body.getUsers())
                        .addAll(body.getHashtags())
                        .addAll(body.getPlaces())
                        .build();
                break;
            case USER:
                list = body.getUsers();
                break;
            case HASHTAG:
                list = body.getHashtags();
                break;
            case LOCATION:
                list = body.getPlaces();
                break;
            default:
                return;
        }
        liveData.postValue(Resource.success(list));
    }

    public void saveToRecentSearches(final SearchItem searchItem) {
        if (searchItem == null) return;
        try {
            final RecentSearch recentSearch = RecentSearch.fromSearchItem(searchItem);
            if (recentSearch == null) return;
            recentSearchRepository.insertOrUpdateRecentSearch(recentSearch, new RepositoryCallback<Void>() {
                @Override
                public void onSuccess(final Void result) {
                    // Log.d(TAG, "onSuccess: inserted recent: " + recentSearch);
                }

                @Override
                public void onDataNotAvailable() {}
            });
        } catch (Exception e) {
            Log.e(TAG, "saveToRecentSearches: ", e);
        }
    }

    @Nullable
    public LiveData<Resource<Object>> deleteRecentSearch(final SearchItem searchItem) {
        if (searchItem == null || !searchItem.isRecent()) return null;
        final RecentSearch recentSearch = RecentSearch.fromSearchItem(searchItem);
        if (recentSearch == null) return null;
        final MutableLiveData<Resource<Object>> data = new MutableLiveData<>();
        data.postValue(Resource.loading(null));
        recentSearchRepository.deleteRecentSearchByIgIdAndType(recentSearch.getIgId(), recentSearch.getType(), new RepositoryCallback<Void>() {
            @Override
            public void onSuccess(final Void result) {
                // Log.d(TAG, "onSuccess: deleted");
                data.postValue(Resource.success(new Object()));
            }

            @Override
            public void onDataNotAvailable() {
                // Log.e(TAG, "onDataNotAvailable: not deleted");
                data.postValue(Resource.error("Error deleting recent item", null));
            }
        });
        return data;
    }
}
