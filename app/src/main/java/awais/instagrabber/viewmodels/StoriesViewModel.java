package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;

import awais.instagrabber.models.StoryModel;

public class StoriesViewModel extends ViewModel {
    private MutableLiveData<List<StoryModel>> list;

    public MutableLiveData<List<StoryModel>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}