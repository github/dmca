package awais.instagrabber.webservices;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.stream.Collectors;

import awais.instagrabber.repositories.NewsRepository;
import awais.instagrabber.repositories.responses.AymlResponse;
import awais.instagrabber.repositories.responses.AymlUser;
import awais.instagrabber.repositories.responses.NewsInboxResponse;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.UserSearchResponse;
import awais.instagrabber.repositories.responses.notification.Notification;
import awais.instagrabber.repositories.responses.notification.NotificationArgs;
import awais.instagrabber.repositories.responses.notification.NotificationCounts;
import awais.instagrabber.utils.Constants;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class NewsService extends BaseService {
    private static final String TAG = "NewsService";

    private final NewsRepository repository;

    private static NewsService instance;

    private NewsService() {
        repository = RetrofitFactory.getInstance()
                                    .getRetrofit()
                                    .create(NewsRepository.class);
    }

    public static NewsService getInstance() {
        if (instance == null) {
            instance = new NewsService();
        }
        return instance;
    }

    public void fetchAppInbox(final boolean markAsSeen,
                              final ServiceCallback<List<Notification>> callback) {
        final Call<NewsInboxResponse> request = repository.appInbox(markAsSeen, Constants.X_IG_APP_ID);
        request.enqueue(new Callback<NewsInboxResponse>() {
            @Override
            public void onResponse(@NonNull final Call<NewsInboxResponse> call, @NonNull final Response<NewsInboxResponse> response) {
                final NewsInboxResponse body = response.body();
                if (body == null) {
                    callback.onSuccess(null);
                    return;
                }
                final List<Notification> result = new ArrayList<Notification>();
                final List<Notification> newStories = body.getNewStories();
                if (newStories != null) result.addAll(newStories);
                final List<Notification> oldStories = body.getOldStories();
                if (oldStories != null) result.addAll(oldStories);
                callback.onSuccess(result);
            }

            @Override
            public void onFailure(@NonNull final Call<NewsInboxResponse> call, @NonNull final Throwable t) {
                callback.onFailure(t);
                // Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    public void fetchActivityCounts(final ServiceCallback<NotificationCounts> callback) {
        final Call<NewsInboxResponse> request = repository.appInbox(false, null);
        request.enqueue(new Callback<NewsInboxResponse>() {
            @Override
            public void onResponse(@NonNull final Call<NewsInboxResponse> call, @NonNull final Response<NewsInboxResponse> response) {
                final NewsInboxResponse body = response.body();
                if (body == null) {
                    callback.onSuccess(null);
                    return;
                }
                callback.onSuccess(body.getCounts());
            }

            @Override
            public void onFailure(@NonNull final Call<NewsInboxResponse> call, @NonNull final Throwable t) {
                callback.onFailure(t);
                // Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    public void fetchSuggestions(final String csrfToken,
                                 final String deviceUuid,
                                 final ServiceCallback<List<Notification>> callback) {
        final Map<String, String> form = new HashMap<>();
        form.put("_uuid", UUID.randomUUID().toString());
        form.put("_csrftoken", csrfToken);
        form.put("phone_id", UUID.randomUUID().toString());
        form.put("device_id", UUID.randomUUID().toString());
        form.put("module", "discover_people");
        form.put("paginate", "false");
        final Call<AymlResponse> request = repository.getAyml(form);
        request.enqueue(new Callback<AymlResponse>() {
            @Override
            public void onResponse(@NonNull final Call<AymlResponse> call, @NonNull final Response<AymlResponse> response) {
                final AymlResponse body = response.body();
                if (body == null) {
                    callback.onSuccess(null);
                    return;
                }
                final List<AymlUser> aymlUsers = new ArrayList<AymlUser>();
                final List<AymlUser> newSuggestions = body.getNewSuggestedUsers().getSuggestions();
                if (newSuggestions != null) {
                    aymlUsers.addAll(newSuggestions);
                }
                final List<AymlUser> oldSuggestions = body.getSuggestedUsers().getSuggestions();
                if (oldSuggestions != null) {
                    aymlUsers.addAll(oldSuggestions);
                }

                final List<Notification> newsItems = aymlUsers
                        .stream()
                        .map(i -> {
                            final User u = i.getUser();
                            return new Notification(
                                    new NotificationArgs(
                                            i.getSocialContext(),
                                            i.getAlgorithm(),
                                            u.getPk(),
                                            u.getProfilePicUrl(),
                                            null,
                                            0L,
                                            u.getUsername(),
                                            u.getFullName(),
                                            u.isVerified()
                                    ),
                                    9999,
                                    i.getUuid()
                            );
                        })
                        .collect(Collectors.toList());
                callback.onSuccess(newsItems);
            }

            @Override
            public void onFailure(@NonNull final Call<AymlResponse> call, @NonNull final Throwable t) {
                callback.onFailure(t);
                // Log.e(TAG, "onFailure: ", t);
            }
        });
    }

    public void fetchChaining(final long targetId, final ServiceCallback<List<Notification>> callback) {
        final Call<UserSearchResponse> request = repository.getChaining(targetId);
        request.enqueue(new Callback<UserSearchResponse>() {
            @Override
            public void onResponse(@NonNull final Call<UserSearchResponse> call, @NonNull final Response<UserSearchResponse> response) {
                final UserSearchResponse body = response.body();
                if (body == null) {
                    callback.onSuccess(null);
                    return;
                }

                final List<Notification> newsItems = body
                        .getUsers()
                        .stream()
                        .map(u -> {
                            return new Notification(
                                    new NotificationArgs(
                                            u.getSocialContext(),
                                            null,
                                            u.getPk(),
                                            u.getProfilePicUrl(),
                                            null,
                                            0L,
                                            u.getUsername(),
                                            u.getFullName(),
                                            u.isVerified()
                                    ),
                                    9999,
                                    u.getProfilePicId() // placeholder
                            );
                        })
                        .collect(Collectors.toList());
                callback.onSuccess(newsItems);
            }

            @Override
            public void onFailure(@NonNull final Call<UserSearchResponse> call, @NonNull final Throwable t) {
                callback.onFailure(t);
                // Log.e(TAG, "onFailure: ", t);
            }
        });
    }
}
