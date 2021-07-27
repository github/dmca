package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.io.File;
import java.util.List;

public class FileListViewModel extends ViewModel {
    private MutableLiveData<List<File>> list;

    public MutableLiveData<List<File>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}
