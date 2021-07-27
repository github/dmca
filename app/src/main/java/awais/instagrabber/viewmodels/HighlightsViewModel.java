package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;

import awais.instagrabber.models.HighlightModel;

public class HighlightsViewModel extends ViewModel {
    private MutableLiveData<List<HighlightModel>> list;

    public MutableLiveData<List<HighlightModel>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}