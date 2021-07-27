package awais.instagrabber.asyncs;

import java.util.List;

import awais.instagrabber.customviews.helpers.PostFetcher;
import awais.instagrabber.interfaces.FetchListener;
import awais.instagrabber.repositories.responses.Hashtag;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.webservices.GraphQLService;
import awais.instagrabber.webservices.ServiceCallback;
import awais.instagrabber.webservices.TagsService;

public class HashtagPostFetchService implements PostFetcher.PostFetchService {
    private final TagsService tagsService;
    private final GraphQLService graphQLService;
    private final Hashtag hashtagModel;
    private String nextMaxId;
    private boolean moreAvailable;
    private final boolean isLoggedIn;

    public HashtagPostFetchService(final Hashtag hashtagModel, final boolean isLoggedIn) {
        this.hashtagModel = hashtagModel;
        this.isLoggedIn = isLoggedIn;
        tagsService = isLoggedIn ? TagsService.getInstance() : null;
        graphQLService = isLoggedIn ? null : GraphQLService.getInstance();
    }

    @Override
    public void fetch(final FetchListener<List<Media>> fetchListener) {
        final ServiceCallback<PostsFetchResponse> cb = new ServiceCallback<PostsFetchResponse>() {
            @Override
            public void onSuccess(final PostsFetchResponse result) {
                if (result == null) return;
                nextMaxId = result.getNextCursor();
                moreAvailable = result.hasNextPage();
                if (fetchListener != null) {
                    fetchListener.onResult(result.getFeedModels());
                }
            }

            @Override
            public void onFailure(final Throwable t) {
                // Log.e(TAG, "onFailure: ", t);
                if (fetchListener != null) {
                    fetchListener.onFailure(t);
                }
            }
        };
        if (isLoggedIn) tagsService.fetchPosts(hashtagModel.getName().toLowerCase(), nextMaxId, cb);
        else graphQLService.fetchHashtagPosts(hashtagModel.getName().toLowerCase(), nextMaxId, cb);
    }

    @Override
    public void reset() {
        nextMaxId = null;
    }

    @Override
    public boolean hasNextPage() {
        return moreAvailable;
    }
}
