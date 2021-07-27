package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;

import awais.instagrabber.repositories.responses.Media;

public class MediaViewModel extends ViewModel {
    private MutableLiveData<List<Media>> list;

    public MutableLiveData<List<Media>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}