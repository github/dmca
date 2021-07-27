package awais.instagrabber.viewmodels;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.google.common.collect.ImmutableList;

import java.io.IOException;
import java.util.Collections;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.models.Resource;
import awais.instagrabber.repositories.responses.AnimatedMediaFixedHeight;
import awais.instagrabber.repositories.responses.giphy.GiphyGif;
import awais.instagrabber.repositories.responses.giphy.GiphyGifImages;
import awais.instagrabber.repositories.responses.giphy.GiphyGifResponse;
import awais.instagrabber.repositories.responses.giphy.GiphyGifResults;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.webservices.GifService;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class GifPickerViewModel extends ViewModel {
    private static final String TAG = GifPickerViewModel.class.getSimpleName();

    private final MutableLiveData<Resource<List<GiphyGif>>> images = new MutableLiveData<>(Resource.success(Collections.emptyList()));
    private final GifService gifService;

    private Call<GiphyGifResponse> searchRequest;

    public GifPickerViewModel() {
        gifService = GifService.getInstance();
        search(null);
    }

    public LiveData<Resource<List<GiphyGif>>> getImages() {
        return images;
    }

    public void search(final String query) {
        final Resource<List<GiphyGif>> currentValue = images.getValue();
        if (currentValue != null && currentValue.status == Resource.Status.LOADING) {
            cancelSearchRequest();
        }
        images.postValue(Resource.loading(getCurrentImages()));
        searchRequest = gifService.searchGiphyGifs(query, query != null);
        searchRequest.enqueue(new Callback<GiphyGifResponse>() {
            @Override
            public void onResponse(@NonNull final Call<GiphyGifResponse> call,
                                   @NonNull final Response<GiphyGifResponse> response) {
                if (response.isSuccessful()) {
                    parseResponse(response);
                    return;
                }
                if (response.errorBody() != null) {
                    try {
                        final String string = response.errorBody().string();
                        final String msg = String.format(Locale.US,
                                                         "onResponse: url: %s, responseCode: %d, errorBody: %s",
                                                         call.request().url().toString(),
                                                         response.code(),
                                                         string);
                        images.postValue(Resource.error(msg, getCurrentImages()));
                        Log.e(TAG, msg);
                    } catch (IOException e) {
                        images.postValue(Resource.error(e.getMessage(), getCurrentImages()));
                        Log.e(TAG, "onResponse: ", e);
                    }
                }
                images.postValue(Resource.error(R.string.generic_failed_request, getCurrentImages()));
            }

            @Override
            public void onFailure(@NonNull final Call<GiphyGifResponse> call,
                                  @NonNull final Throwable t) {
                images.postValue(Resource.error(t.getMessage(), getCurrentImages()));
                Log.e(TAG, "enqueueRequest: onFailure: ", t);
            }
        });
    }

    private void parseResponse(final Response<GiphyGifResponse> response) {
        final GiphyGifResponse giphyGifResponse = response.body();
        if (giphyGifResponse == null) {
            images.postValue(Resource.error(R.string.generic_null_response, getCurrentImages()));
            return;
        }
        final GiphyGifResults results = giphyGifResponse.getResults();
        images.postValue(Resource.success(
                ImmutableList.<GiphyGif>builder()
                        .addAll(results.getGiphy() == null ? Collections.emptyList() : filterInvalid(results.getGiphy()))
                        .addAll(results.getGiphyGifs() == null ? Collections.emptyList() : filterInvalid(results.getGiphyGifs()))
                        .build()
        ));
    }

    private List<GiphyGif> filterInvalid(@NonNull final List<GiphyGif> giphyGifs) {
        return giphyGifs.stream()
                        .filter(Objects::nonNull)
                        .filter(giphyGif -> {
                            final GiphyGifImages images = giphyGif.getImages();
                            if (images == null) return false;
                            final AnimatedMediaFixedHeight fixedHeight = images.getFixedHeight();
                            if (fixedHeight == null) return false;
                            return !TextUtils.isEmpty(fixedHeight.getWebp());
                        })
                        .collect(Collectors.toList());
    }

    // @NonNull
    // private List<GiphyGifImage> getGiphyGifImages(@NonNull final List<GiphyGif> giphy) {
    //     return giphy.stream()
    //                 .map(giphyGif -> {
    //                     final GiphyGifImages images = giphyGif.getImages();
    //                     if (images == null) return null;
    //                     return images.getOriginal();
    //                 })
    //                 .filter(Objects::nonNull)
    //                 .collect(Collectors.toList());
    // }

    private List<GiphyGif> getCurrentImages() {
        final Resource<List<GiphyGif>> value = images.getValue();
        return value == null ? Collections.emptyList() : value.data;
    }

    public void cancelSearchRequest() {
        if (searchRequest == null) return;
        searchRequest.cancel();
    }
}
