package awais.instagrabber.asyncs;

import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.customviews.helpers.PostFetcher;
import awais.instagrabber.interfaces.FetchListener;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.discover.TopicalExploreFeedResponse;
import awais.instagrabber.repositories.responses.WrappedMedia;
import awais.instagrabber.webservices.DiscoverService;
import awais.instagrabber.webservices.ServiceCallback;

public class DiscoverPostFetchService implements PostFetcher.PostFetchService {
    private static final String TAG = "DiscoverPostFetchService";
    private final DiscoverService discoverService;
    private final DiscoverService.TopicalExploreRequest topicalExploreRequest;
    private boolean moreAvailable = false;

    public DiscoverPostFetchService(final DiscoverService.TopicalExploreRequest topicalExploreRequest) {
        this.topicalExploreRequest = topicalExploreRequest;
        discoverService = DiscoverService.getInstance();
    }

    @Override
    public void fetch(final FetchListener<List<Media>> fetchListener) {
        discoverService.topicalExplore(topicalExploreRequest, new ServiceCallback<TopicalExploreFeedResponse>() {
            @Override
            public void onSuccess(final TopicalExploreFeedResponse result) {
                if (result == null) {
                    onFailure(new RuntimeException("result is null"));
                    return;
                }
                moreAvailable = result.isMoreAvailable();
                topicalExploreRequest.setMaxId(result.getNextMaxId());
                final List<WrappedMedia> items = result.getItems();
                final List<Media> posts;
                if (items == null) {
                    posts = Collections.emptyList();
                } else {
                    posts = items.stream()
                                 .map(WrappedMedia::getMedia)
                                 .filter(Objects::nonNull)
                                 .collect(Collectors.toList());
                }
                if (fetchListener != null) {
                    fetchListener.onResult(posts);
                }
            }

            @Override
            public void onFailure(final Throwable t) {
                if (fetchListener != null) {
                    fetchListener.onFailure(t);
                }
            }
        });
    }

    @Override
    public void reset() {
        topicalExploreRequest.setMaxId(null);
    }

    @Override
    public boolean hasNextPage() {
        return moreAvailable;
    }
}
