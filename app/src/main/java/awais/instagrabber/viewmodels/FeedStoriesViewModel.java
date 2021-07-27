package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.models.FeedStoryModel;

public class FeedStoriesViewModel extends ViewModel {
    private MutableLiveData<List<FeedStoryModel>> list;

    public MutableLiveData<List<FeedStoryModel>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}