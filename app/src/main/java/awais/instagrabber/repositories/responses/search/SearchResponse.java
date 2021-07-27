package awais.instagrabber.repositories.responses.search;

import java.util.List;

public class SearchResponse {
    // app
    private final List<SearchItem> list;
    // browser
    private final List<SearchItem> users;
    private final List<SearchItem> places;
    private final List<SearchItem> hashtags;
    // universal
    private final String status;

    public SearchResponse(final List<SearchItem> list,
                          final List<SearchItem> users,
                          final List<SearchItem> places,
                          final List<SearchItem> hashtags,
                          final String status) {
        this.list = list;
        this.users = users;
        this.places = places;
        this.hashtags = hashtags;
        this.status = status;
    }

    public List<SearchItem> getList() {
        return list;
    }

    public List<SearchItem> getUsers() {
        return users;
    }

    public List<SearchItem> getPlaces() {
        return places;
    }

    public List<SearchItem> getHashtags() {
        return hashtags;
    }

    public String getStatus() {
        return status;
    }
}
