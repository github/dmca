package awais.instagrabber.webservices;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.common.collect.ImmutableMap;

import org.json.JSONArray;

import java.io.UnsupportedEncodingException;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;
import java.util.stream.Collectors;

import awais.instagrabber.repositories.DirectMessagesRepository;
import awais.instagrabber.repositories.requests.directmessages.AnimatedMediaBroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.BroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.BroadcastOptions.ThreadIdOrUserIds;
import awais.instagrabber.repositories.requests.directmessages.LinkBroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.PhotoBroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.ReactionBroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.StoryReplyBroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.TextBroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.VideoBroadcastOptions;
import awais.instagrabber.repositories.requests.directmessages.VoiceBroadcastOptions;
import awais.instagrabber.repositories.responses.directmessages.DirectBadgeCount;
import awais.instagrabber.repositories.responses.directmessages.DirectInboxResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemSeenResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadBroadcastResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadDetailsChangeResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadFeedResponse;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadParticipantRequestsResponse;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipientsResponse;
import awais.instagrabber.repositories.responses.giphy.GiphyGif;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import retrofit2.Call;

public class DirectMessagesService extends BaseService {
    private static final String TAG = "DiscoverService";

    private static DirectMessagesService instance;

    private final DirectMessagesRepository repository;
    private final String csrfToken;
    private final long userId;
    private final String deviceUuid;

    private DirectMessagesService(@NonNull final String csrfToken,
                                  final long userId,
                                  @NonNull final String deviceUuid) {
        this.csrfToken = csrfToken;
        this.userId = userId;
        this.deviceUuid = deviceUuid;
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(DirectMessagesRepository.class);
    }

    public String getCsrfToken() {
        return csrfToken;
    }

    public long getUserId() {
        return userId;
    }

    public String getDeviceUuid() {
        return deviceUuid;
    }

    public static DirectMessagesService getInstance(@NonNull final String csrfToken,
                                                    final long userId,
                                                    @NonNull final String deviceUuid) {
        if (instance == null
                || !Objects.equals(instance.getCsrfToken(), csrfToken)
                || !Objects.equals(instance.getUserId(), userId)
                || !Objects.equals(instance.getDeviceUuid(), deviceUuid)) {
            instance = new DirectMessagesService(csrfToken, userId, deviceUuid);
        }
        return instance;
    }

    public Call<DirectInboxResponse> fetchInbox(final String cursor,
                                                final long seqId) {
        final ImmutableMap.Builder<String, Object> queryMapBuilder = ImmutableMap.<String, Object>builder()
                .put("visual_message_return_type", "unseen")
                .put("thread_message_limit", 10)
                .put("persistentBadging", true)
                .put("limit", 10);
        if (!TextUtils.isEmpty(cursor)) {
            queryMapBuilder.put("cursor", cursor);
            queryMapBuilder.put("direction", "older");
        }
        if (seqId != 0) {
            queryMapBuilder.put("seq_id", seqId);
        }
        return repository.fetchInbox(queryMapBuilder.build());
    }

    public Call<DirectThreadFeedResponse> fetchThread(final String threadId,
                                                      final String cursor) {
        final ImmutableMap.Builder<String, Object> queryMapBuilder = ImmutableMap.<String, Object>builder()
                .put("visual_message_return_type", "unseen")
                .put("limit", 20)
                .put("direction", "older");
        if (!TextUtils.isEmpty(cursor)) {
            queryMapBuilder.put("cursor", cursor);
        }
        return repository.fetchThread(threadId, queryMapBuilder.build());
    }

    public Call<DirectBadgeCount> fetchUnseenCount() {
        return repository.fetchUnseenCount();
    }

    public Call<DirectThreadBroadcastResponse> broadcastText(final String clientContext,
                                                             final ThreadIdOrUserIds threadIdOrUserIds,
                                                             final String text,
                                                             final String repliedToItemId,
                                                             final String repliedToClientContext) {
        final List<String> urls = TextUtils.extractUrls(text);
        if (!urls.isEmpty()) {
            return broadcastLink(clientContext, threadIdOrUserIds, text, urls, repliedToItemId, repliedToClientContext);
        }
        final TextBroadcastOptions broadcastOptions = new TextBroadcastOptions(clientContext, threadIdOrUserIds, text);
        broadcastOptions.setRepliedToItemId(repliedToItemId);
        broadcastOptions.setRepliedToClientContext(repliedToClientContext);
        return broadcast(broadcastOptions);
    }

    public Call<DirectThreadBroadcastResponse> broadcastLink(final String clientContext,
                                                             final ThreadIdOrUserIds threadIdOrUserIds,
                                                             final String linkText,
                                                             final List<String> urls,
                                                             final String repliedToItemId,
                                                             final String repliedToClientContext) {
        final LinkBroadcastOptions broadcastOptions = new LinkBroadcastOptions(clientContext, threadIdOrUserIds, linkText, urls);
        broadcastOptions.setRepliedToItemId(repliedToItemId);
        broadcastOptions.setRepliedToClientContext(repliedToClientContext);
        return broadcast(broadcastOptions);
    }

    public Call<DirectThreadBroadcastResponse> broadcastPhoto(final String clientContext,
                                                              final ThreadIdOrUserIds threadIdOrUserIds,
                                                              final String uploadId) {
        return broadcast(new PhotoBroadcastOptions(clientContext, threadIdOrUserIds, true, uploadId));
    }

    public Call<DirectThreadBroadcastResponse> broadcastVideo(final String clientContext,
                                                              final ThreadIdOrUserIds threadIdOrUserIds,
                                                              final String uploadId,
                                                              final String videoResult,
                                                              final boolean sampled) {
        return broadcast(new VideoBroadcastOptions(clientContext, threadIdOrUserIds, videoResult, uploadId, sampled));
    }

    public Call<DirectThreadBroadcastResponse> broadcastVoice(final String clientContext,
                                                              final ThreadIdOrUserIds threadIdOrUserIds,
                                                              final String uploadId,
                                                              final List<Float> waveform,
                                                              final int samplingFreq) {
        return broadcast(new VoiceBroadcastOptions(clientContext, threadIdOrUserIds, uploadId, waveform, samplingFreq));
    }

    public Call<DirectThreadBroadcastResponse> broadcastStoryReply(final ThreadIdOrUserIds threadIdOrUserIds,
                                                                   final String text,
                                                                   final String mediaId,
                                                                   final String reelId) throws UnsupportedEncodingException {
        return broadcast(new StoryReplyBroadcastOptions(UUID.randomUUID().toString(), threadIdOrUserIds, text, mediaId, reelId));
    }

    public Call<DirectThreadBroadcastResponse> broadcastReaction(final String clientContext,
                                                                 final ThreadIdOrUserIds threadIdOrUserIds,
                                                                 final String itemId,
                                                                 final String emoji,
                                                                 final boolean delete) {
        return broadcast(new ReactionBroadcastOptions(clientContext, threadIdOrUserIds, itemId, emoji, delete));
    }

    public Call<DirectThreadBroadcastResponse> broadcastAnimatedMedia(final String clientContext,
                                                                      final ThreadIdOrUserIds threadIdOrUserIds,
                                                                      final GiphyGif giphyGif) {
        return broadcast(new AnimatedMediaBroadcastOptions(clientContext, threadIdOrUserIds, giphyGif));
    }

    private Call<DirectThreadBroadcastResponse> broadcast(@NonNull final BroadcastOptions broadcastOptions) {
        if (TextUtils.isEmpty(broadcastOptions.getClientContext())) {
            throw new IllegalArgumentException("Broadcast requires a valid client context value");
        }
        final Map<String, Object> form = new HashMap<>();
        if (!TextUtils.isEmpty(broadcastOptions.getThreadId())) {
            form.put("thread_id", broadcastOptions.getThreadId());
        } else {
            form.put("recipient_users", new JSONArray(broadcastOptions.getUserIds()).toString());
        }
        form.put("_csrftoken", csrfToken);
        form.put("_uid", userId);
        form.put("__uuid", deviceUuid);
        form.put("client_context", broadcastOptions.getClientContext());
        form.put("mutation_token", broadcastOptions.getClientContext());
        if (!TextUtils.isEmpty(broadcastOptions.getRepliedToItemId()) && !TextUtils.isEmpty(broadcastOptions.getRepliedToClientContext())) {
            form.put("replied_to_item_id", broadcastOptions.getRepliedToItemId());
            form.put("replied_to_client_context", broadcastOptions.getRepliedToClientContext());
        }
        form.putAll(broadcastOptions.getFormMap());
        form.put("action", "send_item");
        final Map<String, String> signedForm = Utils.sign(form);
        return repository.broadcast(broadcastOptions.getItemType().getValue(), signedForm);
    }

    public Call<DirectThreadDetailsChangeResponse> addUsers(final String threadId,
                                                            final Collection<Long> userIds) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid,
                "user_ids", new JSONArray(userIds).toString()
        );
        return repository.addUsers(threadId, form);
    }

    public Call<String> removeUsers(final String threadId,
                                    final Collection<Long> userIds) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid,
                "user_ids", new JSONArray(userIds).toString()
        );
        return repository.removeUsers(threadId, form);
    }

    public Call<DirectThreadDetailsChangeResponse> updateTitle(final String threadId,
                                                               final String title) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid,
                "title", title
        );
        return repository.updateTitle(threadId, form);
    }

    public Call<String> addAdmins(final String threadId,
                                  final Collection<Long> userIds) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid,
                "user_ids", new JSONArray(userIds).toString()
        );
        return repository.addAdmins(threadId, form);
    }

    public Call<String> removeAdmins(final String threadId,
                                     final Collection<Long> userIds) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid,
                "user_ids", new JSONArray(userIds).toString()
        );
        return repository.removeAdmins(threadId, form);
    }

    public Call<String> deleteItem(final String threadId,
                                   final String itemId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.deleteItem(threadId, itemId, form);
    }

    public Call<RankedRecipientsResponse> rankedRecipients(@Nullable final String mode,
                                                           @Nullable final Boolean showThreads,
                                                           @Nullable final String query) {
        // String correctedMode = mode;
        // if (TextUtils.isEmpty(mode) || (!mode.equals("raven") && !mode.equals("reshare"))) {
        //     correctedMode = "raven";
        // }
        final ImmutableMap.Builder<String, String> builder = ImmutableMap.builder();
        if (mode != null) {
            builder.put("mode", mode);
        }
        if (query != null) {
            builder.put("query", query);
        }
        if (showThreads != null) {
            builder.put("showThreads", String.valueOf(showThreads));
        }
        return repository.rankedRecipients(builder.build());
    }

    public Call<DirectThreadBroadcastResponse> forward(@NonNull final String toThreadId,
                                                       @NonNull final String itemType,
                                                       @NonNull final String fromThreadId,
                                                       @NonNull final String itemId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "action", "forward_item",
                "thread_id", toThreadId,
                "item_type", itemType,
                "forwarded_from_thread_id", fromThreadId,
                "forwarded_from_thread_item_id", itemId
        );
        return repository.forward(form);
    }

    public Call<DirectThread> createThread(@NonNull final List<Long> userIds,
                                           @Nullable final String threadTitle) {
        final List<String> userIdStringList = userIds.stream()
                                                     .filter(Objects::nonNull)
                                                     .map(String::valueOf)
                                                     .collect(Collectors.toList());
        final ImmutableMap.Builder<String, Object> formBuilder = ImmutableMap.<String, Object>builder()
                .put("_csrftoken", csrfToken)
                .put("_uuid", deviceUuid)
                .put("_uid", userId)
                .put("recipient_users", new JSONArray(userIdStringList).toString());
        if (threadTitle != null) {
            formBuilder.put("thread_title", threadTitle);
        }
        final Map<String, String> signedForm = Utils.sign(formBuilder.build());
        return repository.createThread(signedForm);
    }

    public Call<String> mute(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.mute(threadId, form);
    }

    public Call<String> unmute(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.unmute(threadId, form);
    }

    public Call<String> muteMentions(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.muteMentions(threadId, form);
    }

    public Call<String> unmuteMentions(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.unmuteMentions(threadId, form);
    }

    public Call<DirectThreadParticipantRequestsResponse> participantRequests(@NonNull final String threadId,
                                                                             final int pageSize,
                                                                             @Nullable final String cursor) {
        return repository.participantRequests(threadId, pageSize, cursor);
    }

    public Call<DirectThreadDetailsChangeResponse> approveParticipantRequests(@NonNull final String threadId,
                                                                              @NonNull final List<Long> userIds) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid,
                "user_ids", new JSONArray(userIds).toString()
                // , "share_join_chat_story", String.valueOf(true)
        );
        return repository.approveParticipantRequests(threadId, form);
    }

    public Call<DirectThreadDetailsChangeResponse> declineParticipantRequests(@NonNull final String threadId,
                                                                              @NonNull final List<Long> userIds) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid,
                "user_ids", new JSONArray(userIds).toString()
        );
        return repository.declineParticipantRequests(threadId, form);
    }

    public Call<DirectThreadDetailsChangeResponse> approvalRequired(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.approvalRequired(threadId, form);
    }

    public Call<DirectThreadDetailsChangeResponse> approvalNotRequired(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.approvalNotRequired(threadId, form);
    }

    public Call<DirectThreadDetailsChangeResponse> leave(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.leave(threadId, form);
    }

    public Call<DirectThreadDetailsChangeResponse> end(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.end(threadId, form);
    }

    public Call<DirectInboxResponse> fetchPendingInbox(final String cursor, final long seqId) {
        final ImmutableMap.Builder<String, Object> queryMapBuilder = ImmutableMap.<String, Object>builder()
                .put("visual_message_return_type", "unseen")
                .put("thread_message_limit", 20)
                .put("persistentBadging", true)
                .put("limit", 10);
        if (!TextUtils.isEmpty(cursor)) {
            queryMapBuilder.put("cursor", cursor);
            queryMapBuilder.put("direction", "older");
        }
        if (seqId != 0) {
            queryMapBuilder.put("seq_id", seqId);
        }
        return repository.fetchPendingInbox(queryMapBuilder.build());
    }

    public Call<String> approveRequest(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.approveRequest(threadId, form);
    }

    public Call<String> declineRequest(@NonNull final String threadId) {
        final ImmutableMap<String, String> form = ImmutableMap.of(
                "_csrftoken", csrfToken,
                "_uuid", deviceUuid
        );
        return repository.declineRequest(threadId, form);
    }

    public Call<DirectItemSeenResponse> markAsSeen(@NonNull final String threadId,
                                                   @NonNull final DirectItem directItem) {
        final ImmutableMap<String, String> form = ImmutableMap.<String, String>builder()
                .put("_csrftoken", csrfToken)
                .put("_uuid", deviceUuid)
                .put("use_unified_inbox", "true")
                .put("action", "mark_seen")
                .put("thread_id", threadId)
                .put("item_id", directItem.getItemId())
                .build();
        return repository.markItemSeen(threadId, directItem.getItemId(), form);
    }
}
