package awais.instagrabber.repositories.responses.search;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.db.entities.Favorite;
import awais.instagrabber.db.entities.RecentSearch;
import awais.instagrabber.models.enums.FavoriteType;
import awais.instagrabber.repositories.responses.Hashtag;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.Place;
import awais.instagrabber.repositories.responses.User;

public class SearchItem {
    private static final String TAG = SearchItem.class.getSimpleName();

    private final User user;
    private final Place place;
    private final Hashtag hashtag;
    private final int position;

    private boolean isRecent = false;
    private boolean isFavorite = false;

    public SearchItem(final User user,
                      final Place place,
                      final Hashtag hashtag,
                      final int position) {
        this.user = user;
        this.place = place;
        this.hashtag = hashtag;
        this.position = position;
    }

    public User getUser() {
        return user;
    }

    public Place getPlace() {
        return place;
    }

    public Hashtag getHashtag() {
        return hashtag;
    }

    public int getPosition() {
        return position;
    }

    public boolean isRecent() {
        return isRecent;
    }

    public void setRecent(final boolean recent) {
        isRecent = recent;
    }

    public boolean isFavorite() {
        return isFavorite;
    }

    public void setFavorite(final boolean favorite) {
        isFavorite = favorite;
    }

    @Nullable
    public FavoriteType getType() {
        if (user != null) {
            return FavoriteType.USER;
        }
        if (hashtag != null) {
            return FavoriteType.HASHTAG;
        }
        if (place != null) {
            return FavoriteType.LOCATION;
        }
        return null;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final SearchItem that = (SearchItem) o;
        return Objects.equals(user, that.user) &&
                Objects.equals(place, that.place) &&
                Objects.equals(hashtag, that.hashtag);
    }

    @Override
    public int hashCode() {
        return Objects.hash(user, place, hashtag);
    }

    @NonNull
    @Override
    public String toString() {
        return "SearchItem{" +
                "user=" + user +
                ", place=" + place +
                ", hashtag=" + hashtag +
                ", position=" + position +
                ", isRecent=" + isRecent +
                '}';
    }

    @NonNull
    public static List<SearchItem> fromRecentSearch(final List<RecentSearch> recentSearches) {
        if (recentSearches == null) return Collections.emptyList();
        return recentSearches.stream()
                             .map(SearchItem::fromRecentSearch)
                             .filter(Objects::nonNull)
                             .collect(Collectors.toList());
    }

    @Nullable
    private static SearchItem fromRecentSearch(final RecentSearch recentSearch) {
        if (recentSearch == null) return null;
        try {
            final FavoriteType type = recentSearch.getType();
            final SearchItem searchItem;
            switch (type) {
                case USER:
                    searchItem = new SearchItem(getUser(recentSearch), null, null, 0);
                    break;
                case HASHTAG:
                    searchItem = new SearchItem(null, null, getHashtag(recentSearch), 0);
                    break;
                case LOCATION:
                    searchItem = new SearchItem(null, getPlace(recentSearch), null, 0);
                    break;
                default:
                    return null;
            }
            searchItem.setRecent(true);
            return searchItem;
        } catch (Exception e) {
            Log.e(TAG, "fromRecentSearch: ", e);
        }
        return null;
    }

    public static List<SearchItem> fromFavorite(final List<Favorite> favorites) {
        if (favorites == null) {
            return Collections.emptyList();
        }
        return favorites.stream()
                        .map(SearchItem::fromFavorite)
                        .filter(Objects::nonNull)
                        .collect(Collectors.toList());
    }

    @Nullable
    private static SearchItem fromFavorite(final Favorite favorite) {
        if (favorite == null) return null;
        final FavoriteType type = favorite.getType();
        if (type == null) return null;
        final SearchItem searchItem;
        switch (type) {
            case USER:
                searchItem = new SearchItem(getUser(favorite), null, null, 0);
                break;
            case HASHTAG:
                searchItem = new SearchItem(null, null, getHashtag(favorite), 0);
                break;
            case LOCATION:
                final Place place = getPlace(favorite);
                if (place == null) return null;
                searchItem = new SearchItem(null, place, null, 0);
                break;
            default:
                return null;
        }
        searchItem.setFavorite(true);
        return searchItem;
    }

    @NonNull
    private static User getUser(@NonNull final RecentSearch recentSearch) {
        return new User(
                Long.parseLong(recentSearch.getIgId()),
                recentSearch.getUsername(),
                recentSearch.getName(),
                false,
                recentSearch.getPicUrl(),
                false
        );
    }

    @NonNull
    private static User getUser(@NonNull final Favorite favorite) {
        return new User(
                0,
                favorite.getQuery(),
                favorite.getDisplayName(),
                false,
                favorite.getPicUrl(),
                false
        );
    }

    @NonNull
    private static Hashtag getHashtag(@NonNull final RecentSearch recentSearch) {
        return new Hashtag(
                recentSearch.getIgId(),
                recentSearch.getName(),
                0,
                null,
                null
        );
    }

    @NonNull
    private static Hashtag getHashtag(@NonNull final Favorite favorite) {
        return new Hashtag(
                "0",
                favorite.getQuery(),
                0,
                null,
                null
        );
    }

    @NonNull
    private static Place getPlace(@NonNull final RecentSearch recentSearch) {
        final Location location = new Location(
                Long.parseLong(recentSearch.getIgId()),
                recentSearch.getName(),
                recentSearch.getName(),
                null, null, 0, 0
        );
        return new Place(
                location,
                recentSearch.getName(),
                null,
                null,
                null
        );
    }

    @Nullable
    private static Place getPlace(@NonNull final Favorite favorite) {
        try {
            final Location location = new Location(
                    Long.parseLong(favorite.getQuery()),
                    favorite.getDisplayName(),
                    favorite.getDisplayName(),
                    null, null, 0, 0
            );
            return new Place(
                    location,
                    favorite.getDisplayName(),
                    null,
                    null,
                    null
            );
        } catch (Exception e) {
            Log.e(TAG, "getPlace: ", e);
            return null;
        }
    }
}
