package awais.instagrabber.repositories;

import java.util.Map;

import awais.instagrabber.repositories.responses.directmessages.DirectBadgeCount;
import awais.instagrabber.repositories.responses.directmessages.DirectInboxResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectItemSeenResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadBroadcastResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadDetailsChangeResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadFeedResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadParticipantRequestsResponse;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipientsResponse;
import retrofit2.Call;
import retrofit2.http.FieldMap;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Path;
import retrofit2.http.Query;
import retrofit2.http.QueryMap;

public interface DirectMessagesRepository {

    @GET("/api/v1/direct_v2/inbox/")
    Call<DirectInboxResponse> fetchInbox(@QueryMap Map<String, Object> queryMap);

    @GET("/api/v1/direct_v2/pending_inbox/")
    Call<DirectInboxResponse> fetchPendingInbox(@QueryMap Map<String, Object> queryMap);

    @GET("/api/v1/direct_v2/threads/{threadId}/")
    Call<DirectThreadFeedResponse> fetchThread(@Path("threadId") String threadId,
                                               @QueryMap Map<String, Object> queryMap);

    @GET("/api/v1/direct_v2/get_badge_count/?no_raven=1")
    Call<DirectBadgeCount> fetchUnseenCount();

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/broadcast/{item}/")
    Call<DirectThreadBroadcastResponse> broadcast(@Path("item") String item,
                                                  @FieldMap final Map<String, String> signedForm);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/add_user/")
    Call<DirectThreadDetailsChangeResponse> addUsers(@Path("threadId") String threadId,
                                                     @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/remove_users/")
    Call<String> removeUsers(@Path("threadId") String threadId,
                             @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/update_title/")
    Call<DirectThreadDetailsChangeResponse> updateTitle(@Path("threadId") String threadId,
                                                        @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/add_admins/")
    Call<String> addAdmins(@Path("threadId") String threadId,
                           @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/remove_admins/")
    Call<String> removeAdmins(@Path("threadId") String threadId,
                              @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/items/{itemId}/delete/")
    Call<String> deleteItem(@Path("threadId") String threadId,
                            @Path("itemId") String itemId,
                            @FieldMap final Map<String, String> form);

    @GET("/api/v1/direct_v2/ranked_recipients/")
    Call<RankedRecipientsResponse> rankedRecipients(@QueryMap Map<String, String> queryMap);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/broadcast/forward/")
    Call<DirectThreadBroadcastResponse> forward(@FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/create_group_thread/")
    Call<DirectThread> createThread(@FieldMap final Map<String, String> signedForm);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/mute/")
    Call<String> mute(@Path("threadId") String threadId,
                      @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/unmute/")
    Call<String> unmute(@Path("threadId") String threadId,
                        @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/mute_mentions/")
    Call<String> muteMentions(@Path("threadId") String threadId,
                              @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/unmute_mentions/")
    Call<String> unmuteMentions(@Path("threadId") String threadId,
                                @FieldMap final Map<String, String> form);

    @GET("/api/v1/direct_v2/threads/{threadId}/participant_requests/")
    Call<DirectThreadParticipantRequestsResponse> participantRequests(@Path("threadId") String threadId,
                                                                      @Query("page_size") int pageSize,
                                                                      @Query("cursor") String cursor);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/approve_participant_requests/")
    Call<DirectThreadDetailsChangeResponse> approveParticipantRequests(@Path("threadId") String threadId,
                                                                       @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/deny_participant_requests/")
    Call<DirectThreadDetailsChangeResponse> declineParticipantRequests(@Path("threadId") String threadId,
                                                                       @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/approval_required_for_new_members/")
    Call<DirectThreadDetailsChangeResponse> approvalRequired(@Path("threadId") String threadId,
                                                             @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/approval_not_required_for_new_members/")
    Call<DirectThreadDetailsChangeResponse> approvalNotRequired(@Path("threadId") String threadId,
                                                                @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/leave/")
    Call<DirectThreadDetailsChangeResponse> leave(@Path("threadId") String threadId,
                                                  @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/remove_all_users/")
    Call<DirectThreadDetailsChangeResponse> end(@Path("threadId") String threadId,
                                                @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/approve/")
    Call<String> approveRequest(@Path("threadId") String threadId,
                                @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/decline/")
    Call<String> declineRequest(@Path("threadId") String threadId,
                                @FieldMap final Map<String, String> form);

    @FormUrlEncoded
    @POST("/api/v1/direct_v2/threads/{threadId}/items/{itemId}/seen/")
    Call<DirectItemSeenResponse> markItemSeen(@Path("threadId") String threadId,
                                              @Path("itemId") String itemId,
                                              @FieldMap final Map<String, String> form);
}
