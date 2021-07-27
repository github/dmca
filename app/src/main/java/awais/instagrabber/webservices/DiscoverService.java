package awais.instagrabber.webservices;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableMap;

import java.util.Objects;

import awais.instagrabber.repositories.DiscoverRepository;
import awais.instagrabber.repositories.responses.discover.TopicalExploreFeedResponse;
import awais.instagrabber.utils.TextUtils;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class DiscoverService extends BaseService {

    private static final String TAG = "DiscoverService";

    private final DiscoverRepository repository;

    private static DiscoverService instance;

    private DiscoverService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(DiscoverRepository.class);
    }

    public static DiscoverService getInstance() {
        if (instance == null) {
            instance = new DiscoverService();
        }
        return instance;
    }

    public void topicalExplore(@NonNull final TopicalExploreRequest request,
                               final ServiceCallback<TopicalExploreFeedResponse> callback) {
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.<String, String>builder()
                .put("module", "explore_popular");
        if (!TextUtils.isEmpty(request.getModule())) {
            builder.put("module", request.getModule());
        }
        if (!TextUtils.isEmpty(request.getClusterId())) {
            builder.put("cluster_id", request.getClusterId());
        }
        if (!TextUtils.isEmpty(request.getMaxId())) {
            builder.put("max_id", request.getMaxId());
        }
        final Call<TopicalExploreFeedResponse> req = repository.topicalExplore(builder.build());
        req.enqueue(new Callback<TopicalExploreFeedResponse>() {
            @Override
            public void onResponse(@NonNull final Call<TopicalExploreFeedResponse> call,
                                   @NonNull final Response<TopicalExploreFeedResponse> response) {
                if (callback == null) return;
                final TopicalExploreFeedResponse feedResponse = response.body();
                if (feedResponse == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(feedResponse);
                // try {
                // } catch (JSONException e) {
                //     callback.onFailure(e);
                //     // Log.e(TAG, "Error parsing topicalExplore response", e);
                // }
            }

            @Override
            public void onFailure(@NonNull final Call<TopicalExploreFeedResponse> call, @NonNull final Throwable t) {
                callback.onFailure(t);
            }
        });
    }

    // private TopicalExploreResponse parseTopicalExploreResponse(@NonNull final String body) throws JSONException {
    //     final JSONObject root = new JSONObject(body);
    //     final boolean moreAvailable = root.optBoolean("more_available");
    //     final int nextMaxId = root.optInt("next_max_id", -1);
    //     final int numResults = root.optInt("num_results");
    //     final String status = root.optString("status");
    //     final JSONArray clustersJson = root.optJSONArray("clusters");
    //     final List<TopicCluster> clusters = parseClusters(clustersJson);
    //     final JSONArray itemsJson = root.optJSONArray("items");
    //     final List<FeedModel> items = parseItems(itemsJson);
    //     return new TopicalExploreResponse(
    //             moreAvailable,
    //             nextMaxId,
    //             numResults,
    //             status,
    //             clusters,
    //             items
    //     );
    // }

    // private List<TopicCluster> parseClusters(final JSONArray clustersJson) throws JSONException {
    //     if (clustersJson == null) {
    //         return Collections.emptyList();
    //     }
    //     final List<TopicCluster> clusters = new ArrayList<>();
    //     for (int i = 0; i < clustersJson.length(); i++) {
    //         final JSONObject clusterJson = clustersJson.getJSONObject(i);
    //         final String id = clusterJson.optString("id");
    //         final String title = clusterJson.optString("title");
    //         if (TextUtils.isEmpty(id) || TextUtils.isEmpty(title)) {
    //             continue;
    //         }
    //         final String type = clusterJson.optString("type");
    //         final boolean canMute = clusterJson.optBoolean("can_mute");
    //         final boolean isMuted = clusterJson.optBoolean("is_muted");
    //         final JSONObject coverMediaJson = clusterJson.optJSONObject("cover_media");
    //         final int rankedPosition = clusterJson.optInt("ranked_position");
    //         final FeedModel feedModel = parseClusterCover(coverMediaJson);
    //         final TopicCluster topicCluster = new TopicCluster(
    //                 id,
    //                 title,
    //                 type,
    //                 canMute,
    //                 isMuted,
    //                 rankedPosition,
    //                 feedModel
    //         );
    //         clusters.add(topicCluster);
    //     }
    //     return clusters;
    // }

    // private FeedModel parseClusterCover(final JSONObject coverMediaJson) throws JSONException {
    //     if (coverMediaJson == null) {
    //         return null;
    //     }
    //     ProfileModel profileModel = null;
    //     if (coverMediaJson.has("user")) {
    //         final JSONObject user = coverMediaJson.getJSONObject("user");
    //         profileModel = new ProfileModel(
    //                 user.optBoolean("is_private"),
    //                 false,
    //                 user.optBoolean("is_verified"),
    //                 user.getString("pk"),
    //                 user.getString(Constants.EXTRAS_USERNAME),
    //                 user.optString("full_name"),
    //                 null,
    //                 null,
    //                 user.getString("profile_pic_url"),
    //                 null,
    //                 0,
    //                 0,
    //                 0,
    //                 false,
    //                 false,
    //                 false,
    //                 false,
    //                 false);
    //     }
    //     final String resourceUrl = ResponseBodyUtils.getHighQualityImage(coverMediaJson);
    //     final String thumbnailUrl = ResponseBodyUtils.getLowQualityImage(coverMediaJson);
    //     final int width = coverMediaJson.optInt("original_width");
    //     final int height = coverMediaJson.optInt("original_height");
    //     return new FeedModel.Builder()
    //             .setProfileModel(profileModel)
    //             .setItemType(MediaItemType.MEDIA_TYPE_IMAGE)
    //             .setViewCount(0)
    //             .setPostId(coverMediaJson.getString(Constants.EXTRAS_ID))
    //             .setDisplayUrl(resourceUrl)
    //             .setThumbnailUrl(thumbnailUrl)
    //             .setShortCode(coverMediaJson.getString("code"))
    //             .setPostCaption(null)
    //             .setCommentsCount(0)
    //             .setTimestamp(coverMediaJson.optLong("taken_at", -1))
    //             .setLiked(false)
    //             .setBookmarked(false)
    //             .setLikesCount(0)
    //             .setLocationName(null)
    //             .setLocationId(null)
    //             .setImageHeight(height)
    //             .setImageWidth(width)
    //             .build();
    // }

    // private List<FeedModel> parseItems(final JSONArray items) throws JSONException {
    //     if (items == null) {
    //         return Collections.emptyList();
    //     }
    //     final List<FeedModel> feedModels = new ArrayList<>();
    //     for (int i = 0; i < items.length(); i++) {
    //         final JSONObject itemJson = items.optJSONObject(i);
    //         if (itemJson == null) {
    //             continue;
    //         }
    //         final JSONObject mediaJson = itemJson.optJSONObject("media");
    //         final FeedModel feedModel = ResponseBodyUtils.parseItem(mediaJson);
    //         if (feedModel != null) {
    //             feedModels.add(feedModel);
    //         }
    //     }
    //     return feedModels;
    // }

    public static class TopicalExploreRequest {

        private String module;
        private String clusterId;
        private String maxId;

        public TopicalExploreRequest() {}

        public TopicalExploreRequest(final String module, final String clusterId, final String maxId) {
            this.module = module;
            this.clusterId = clusterId;
            this.maxId = maxId;
        }

        public String getModule() {
            return module;
        }

        public TopicalExploreRequest setModule(final String module) {
            this.module = module;
            return this;
        }

        public String getClusterId() {
            return clusterId;
        }

        public void setClusterId(final String clusterId) {
            this.clusterId = clusterId;
        }

        public String getMaxId() {
            return maxId;
        }

        public void setMaxId(final String maxId) {
            this.maxId = maxId;
        }

        @Override
        public boolean equals(final Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            final TopicalExploreRequest that = (TopicalExploreRequest) o;
            return maxId.equals(that.maxId) &&
                    Objects.equals(module, that.module) &&
                    Objects.equals(clusterId, that.clusterId);
        }

        @Override
        public int hashCode() {
            return Objects.hash(module, clusterId, maxId);
        }

        @NonNull
        @Override
        public String toString() {
            return "TopicalExploreRequest{" +
                    "module='" + module + '\'' +
                    ", clusterId='" + clusterId + '\'' +
                    ", maxId=" + maxId +
                    '}';
        }
    }

    // public static class TopicalExploreResponse {
    //
    //     private boolean moreAvailable;
    //     private int nextMaxId;
    //     private int numResults;
    //     private String status;
    //     private List<TopicCluster> clusters;
    //     private List<FeedModel> items;
    //
    //     public TopicalExploreResponse() {}
    //
    //     public TopicalExploreResponse(final boolean moreAvailable,
    //                                   final int nextMaxId,
    //                                   final int numResults,
    //                                   final String status,
    //                                   final List<TopicCluster> clusters, final List<FeedModel> items) {
    //         this.moreAvailable = moreAvailable;
    //         this.nextMaxId = nextMaxId;
    //         this.numResults = numResults;
    //         this.status = status;
    //         this.clusters = clusters;
    //         this.items = items;
    //     }
    //
    //     public boolean isMoreAvailable() {
    //         return moreAvailable;
    //     }
    //
    //     public TopicalExploreResponse setMoreAvailable(final boolean moreAvailable) {
    //         this.moreAvailable = moreAvailable;
    //         return this;
    //     }
    //
    //     public int getNextMaxId() {
    //         return nextMaxId;
    //     }
    //
    //     public TopicalExploreResponse setNextMaxId(final int nextMaxId) {
    //         this.nextMaxId = nextMaxId;
    //         return this;
    //     }
    //
    //     public int getNumResults() {
    //         return numResults;
    //     }
    //
    //     public TopicalExploreResponse setNumResults(final int numResults) {
    //         this.numResults = numResults;
    //         return this;
    //     }
    //
    //     public String getStatus() {
    //         return status;
    //     }
    //
    //     public TopicalExploreResponse setStatus(final String status) {
    //         this.status = status;
    //         return this;
    //     }
    //
    //     public List<TopicCluster> getClusters() {
    //         return clusters;
    //     }
    //
    //     public TopicalExploreResponse setClusters(final List<TopicCluster> clusters) {
    //         this.clusters = clusters;
    //         return this;
    //     }
    //
    //     public List<FeedModel> getItems() {
    //         return items;
    //     }
    //
    //     public TopicalExploreResponse setItems(final List<FeedModel> items) {
    //         this.items = items;
    //         return this;
    //     }
    //
    //     @Override
    //     public boolean equals(final Object o) {
    //         if (this == o) return true;
    //         if (o == null || getClass() != o.getClass()) return false;
    //         final TopicalExploreResponse that = (TopicalExploreResponse) o;
    //         return moreAvailable == that.moreAvailable &&
    //                 nextMaxId == that.nextMaxId &&
    //                 numResults == that.numResults &&
    //                 Objects.equals(status, that.status) &&
    //                 Objects.equals(clusters, that.clusters) &&
    //                 Objects.equals(items, that.items);
    //     }
    //
    //     @Override
    //     public int hashCode() {
    //         return Objects.hash(moreAvailable, nextMaxId, numResults, status, clusters, items);
    //     }
    //
    //     @Override
    //     public String toString() {
    //         return "TopicalExploreResponse{" +
    //                 "moreAvailable=" + moreAvailable +
    //                 ", nextMaxId=" + nextMaxId +
    //                 ", numResults=" + numResults +
    //                 ", status='" + status + '\'' +
    //                 ", clusters=" + clusters +
    //                 ", items=" + items +
    //                 '}';
    //     }
    // }
}
