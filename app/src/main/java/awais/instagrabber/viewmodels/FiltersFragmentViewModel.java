package awais.instagrabber.viewmodels;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

public class FiltersFragmentViewModel extends ViewModel {
    private final MutableLiveData<Boolean> loading = new MutableLiveData<>(false);
    private final MutableLiveData<ImageEditViewModel.Tab> currentTab = new MutableLiveData<>();

    public FiltersFragmentViewModel() {
    }

    public LiveData<Boolean> isLoading() {
        return loading;
    }

    public LiveData<ImageEditViewModel.Tab> getCurrentTab() {
        return currentTab;
    }

    public void setCurrentTab(final ImageEditViewModel.Tab tab) {
        if (tab == null) return;
        currentTab.postValue(tab);
    }
}
