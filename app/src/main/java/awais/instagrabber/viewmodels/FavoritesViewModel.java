package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;

import awais.instagrabber.db.entities.Favorite;

public class FavoritesViewModel extends ViewModel {
    private MutableLiveData<List<Favorite>> list;

    public MutableLiveData<List<Favorite>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}
