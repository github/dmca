package awais.instagrabber.asyncs;

import java.util.List;

import awais.instagrabber.customviews.helpers.PostFetcher;
import awais.instagrabber.interfaces.FetchListener;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.webservices.GraphQLService;
import awais.instagrabber.webservices.ProfileService;
import awais.instagrabber.webservices.ServiceCallback;

public class ProfilePostFetchService implements PostFetcher.PostFetchService {
    private static final String TAG = "ProfilePostFetchService";
    private final ProfileService profileService;
    private final GraphQLService graphQLService;
    private final User profileModel;
    private final boolean isLoggedIn;
    private String nextMaxId;
    private boolean moreAvailable;

    public ProfilePostFetchService(final User profileModel, final boolean isLoggedIn) {
        this.profileModel = profileModel;
        this.isLoggedIn = isLoggedIn;
        graphQLService = isLoggedIn ? null : GraphQLService.getInstance();
        profileService = isLoggedIn ? ProfileService.getInstance() : null;
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
        if (isLoggedIn) profileService.fetchPosts(profileModel.getPk(), nextMaxId, cb);
        else graphQLService.fetchProfilePosts(profileModel.getPk(), 30, nextMaxId, profileModel, cb);
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
