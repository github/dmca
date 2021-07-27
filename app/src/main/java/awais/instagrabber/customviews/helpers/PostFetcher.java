package awais.instagrabber.customviews.helpers;

import android.util.Log;

import java.util.List;

import awais.instagrabber.interfaces.FetchListener;
import awais.instagrabber.repositories.responses.Media;

public class PostFetcher {
    private static final String TAG = PostFetcher.class.getSimpleName();

    private final PostFetchService postFetchService;
    private final FetchListener<List<Media>> fetchListener;
    private boolean fetching;

    public PostFetcher(final PostFetchService postFetchService,
                       final FetchListener<List<Media>> fetchListener) {
        this.postFetchService = postFetchService;
        this.fetchListener = fetchListener;
    }

    public void fetch() {
        if (!fetching) {
            fetching = true;
            postFetchService.fetch(new FetchListener<List<Media>>() {
                @Override
                public void onResult(final List<Media> result) {
                    fetching = false;
                    fetchListener.onResult(result);
                }

                @Override
                public void onFailure(final Throwable t) {
                    Log.e(TAG, "onFailure: ", t);
                }
            });
        }
    }

    public void reset() {
        postFetchService.reset();
    }

    public boolean isFetching() {
        return fetching;
    }

    public boolean hasMore() {
        return postFetchService.hasNextPage();
    }

    public interface PostFetchService {
        void fetch(FetchListener<List<Media>> fetchListener);

        void reset();

        boolean hasNextPage();
    }
}
