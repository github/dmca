package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;

import awais.instagrabber.models.FollowModel;

public class FollowViewModel extends ViewModel {
    private MutableLiveData<List<FollowModel>> list;

    public MutableLiveData<List<FollowModel>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}
