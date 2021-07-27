package awais.instagrabber.viewmodels;

import android.content.Context;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.Collections;
import java.util.List;

import awais.instagrabber.utils.MediaController;
import awais.instagrabber.utils.MediaController.AlbumEntry;

public class MediaPickerViewModel extends ViewModel implements MediaController.OnLoadListener {
    private final MutableLiveData<List<AlbumEntry>> allAlbums = new MutableLiveData<>(Collections.emptyList());

    private MediaController mediaController;

    public MediaPickerViewModel() {

    }

    public void loadMedia(final Context context) {
        mediaController = new MediaController(context, this);
        mediaController.load();
    }

    @Override
    public void onLoad() {
        if (mediaController == null) {
            return;
        }
        final List<AlbumEntry> allPhotoAlbums = mediaController.getAllMediaAlbums();
        this.allAlbums.postValue(allPhotoAlbums);
    }

    public LiveData<List<AlbumEntry>> getAllAlbums() {
        return allAlbums;
    }
}
