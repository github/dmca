package awais.instagrabber.viewmodels;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;

import awais.instagrabber.repositories.responses.notification.Notification;

public class NotificationViewModel extends ViewModel {
    private MutableLiveData<List<Notification>> list;

    public MutableLiveData<List<Notification>> getList() {
        if (list == null) {
            list = new MutableLiveData<>();
        }
        return list;
    }
}
