package awais.instagrabber.asyncs;

import java.util.List;

import awais.instagrabber.customviews.helpers.PostFetcher;
import awais.instagrabber.interfaces.FetchListener;
import awais.instagrabber.models.enums.PostItemType;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.PostsFetchResponse;
import awais.instagrabber.webservices.GraphQLService;
import awais.instagrabber.webservices.ProfileService;
import awais.instagrabber.webservices.ServiceCallback;

public class SavedPostFetchService implements PostFetcher.PostFetchService {
    private final ProfileService profileService;
    private final GraphQLService graphQLService;
    private final long profileId;
    private final PostItemType type;
    private final boolean isLoggedIn;

    private String nextMaxId;
    private final String collectionId;
    private boolean moreAvailable;

    public SavedPostFetchService(final long profileId, final PostItemType type, final boolean isLoggedIn, final String collectionId) {
        this.profileId = profileId;
        this.type = type;
        this.isLoggedIn = isLoggedIn;
        this.collectionId = collectionId;
        graphQLService = isLoggedIn ? null : GraphQLService.getInstance();
        profileService = isLoggedIn ? ProfileService.getInstance() : null;
    }

    @Override
    public void fetch(final FetchListener<List<Media>> fetchListener) {
        final ServiceCallback<PostsFetchResponse> callback = new ServiceCallback<PostsFetchResponse>() {
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
        switch (type) {
            case LIKED:
                profileService.fetchLiked(nextMaxId, callback);
                break;
            case TAGGED:
                if (isLoggedIn) profileService.fetchTagged(profileId, nextMaxId, callback);
                else graphQLService.fetchTaggedPosts(profileId, 30, nextMaxId, callback);
                break;
            case COLLECTION:
            case SAVED:
                profileService.fetchSaved(nextMaxId, collectionId, callback);
                break;
            default:
                callback.onFailure(null);
        }
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
