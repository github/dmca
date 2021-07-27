package awais.instagrabber.viewmodels;

import android.app.Application;
import android.graphics.RectF;
import android.net.Uri;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;

import awais.instagrabber.fragments.imageedit.filters.FiltersHelper.FilterType;
import awais.instagrabber.fragments.imageedit.filters.filters.Filter;
import awais.instagrabber.fragments.imageedit.filters.properties.Property;
import awais.instagrabber.models.SavedImageEditState;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.DirectoryUtils;
import awais.instagrabber.utils.SerializablePair;
import jp.co.cyberagent.android.gpuimage.GPUImage;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageFilter;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageFilterGroup;

public class ImageEditViewModel extends AndroidViewModel {
    private static final String CROP = "crop";
    private static final String RESULT = "result";
    private static final String FILE_FORMAT = "yyyyMMddHHmmssSSS";
    private static final SimpleDateFormat SIMPLE_DATE_FORMAT = new SimpleDateFormat(FILE_FORMAT, Locale.US);

    private Uri originalUri;
    private SavedImageEditState savedImageEditState;

    private final String sessionId;
    private final Uri destinationUri;
    private final Uri cropDestinationUri;
    private final MutableLiveData<Boolean> loading = new MutableLiveData<>(false);
    private final MutableLiveData<Uri> resultUri = new MutableLiveData<>(null);
    private final MutableLiveData<Tab> currentTab = new MutableLiveData<>(Tab.RESULT);
    private final MutableLiveData<Boolean> isCropped = new MutableLiveData<>(false);
    private final MutableLiveData<Boolean> isTuned = new MutableLiveData<>(false);
    private final MutableLiveData<Boolean> isFiltered = new MutableLiveData<>(false);
    private final File outputDir;
    private List<Filter<? extends GPUImageFilter>> tuningFilters;
    private Filter<? extends GPUImageFilter> appliedFilter;
    private final File destinationFile;

    public ImageEditViewModel(final Application application) {
        super(application);
        sessionId = SIMPLE_DATE_FORMAT.format(new Date());
        outputDir = DirectoryUtils.getOutputMediaDirectory(application, "Edit", sessionId);
        destinationFile = new File(outputDir, RESULT + ".jpg");
        destinationUri = Uri.fromFile(destinationFile);
        cropDestinationUri = Uri.fromFile(new File(outputDir, CROP + ".jpg"));
    }

    public String getSessionId() {
        return sessionId;
    }

    public Uri getOriginalUri() {
        return originalUri;
    }

    public void setOriginalUri(final Uri originalUri) {
        if (originalUri == null) return;
        this.originalUri = originalUri;
        savedImageEditState = new SavedImageEditState(sessionId, originalUri.toString());
        if (resultUri.getValue() == null) {
            resultUri.postValue(originalUri);
        }
    }

    public Uri getDestinationUri() {
        return destinationUri;
    }

    public Uri getCropDestinationUri() {
        return cropDestinationUri;
    }

    public LiveData<Boolean> isLoading() {
        return loading;
    }

    public LiveData<Uri> getResultUri() {
        return resultUri;
    }

    public LiveData<Boolean> isCropped() {
        return isCropped;
    }

    public LiveData<Boolean> isTuned() {
        return isTuned;
    }

    public LiveData<Boolean> isFiltered() {
        return isFiltered;
    }

    public void setResultUri(final Uri uri) {
        if (uri == null) return;
        resultUri.postValue(uri);
    }

    public LiveData<Tab> getCurrentTab() {
        return currentTab;
    }

    public void setCurrentTab(final Tab tab) {
        if (tab == null) return;
        this.currentTab.postValue(tab);
    }

    public SavedImageEditState getSavedImageEditState() {
        return savedImageEditState;
    }

    public void setCropResult(final float[] imageMatrixValues, final RectF cropRect) {
        savedImageEditState.setCropImageMatrixValues(imageMatrixValues);
        savedImageEditState.setCropRect(cropRect);
        isCropped.postValue(true);
        applyFilters();
    }

    private void applyFilters() {
        final GPUImage gpuImage = new GPUImage(getApplication());
        if ((tuningFilters != null && !tuningFilters.isEmpty()) || appliedFilter != null) {
            AppExecutors.getInstance().tasksThread().submit(() -> {
                final List<GPUImageFilter> list = new ArrayList<>();
                if (tuningFilters != null) {
                    for (Filter<? extends GPUImageFilter> tuningFilter : tuningFilters) {
                        list.add(tuningFilter.getInstance());
                    }
                }
                if (appliedFilter != null) {
                    list.add(appliedFilter.getInstance());
                }
                gpuImage.setFilter(new GPUImageFilterGroup(list));
                final Uri uri = cropDestinationUri != null ? cropDestinationUri : originalUri;
                gpuImage.setImage(uri);
                gpuImage.saveToPictures(new File(destinationUri.toString()), false, uri1 -> setResultUri(destinationUri));
            });
            return;
        }
        setResultUri(cropDestinationUri);
    }

    public void cancel() {
        delete(outputDir);
    }

    private void delete(@NonNull final File file) {
        if (file.isDirectory()) {
            final File[] files = file.listFiles();
            if (files != null) {
                for (File f : files) {
                    delete(f);
                }
            }
        }
        //noinspection ResultOfMethodCallIgnored
        file.delete();
    }

    public void setAppliedFilters(final List<Filter<?>> tuningFilters, final Filter<?> filter) {
        this.tuningFilters = tuningFilters;
        this.appliedFilter = filter;
        if (savedImageEditState != null) {
            final HashMap<FilterType, Map<Integer, Object>> tuningFiltersMap = new HashMap<>();
            for (final Filter<?> tuningFilter : tuningFilters) {
                final SerializablePair<FilterType, Map<Integer, Object>> filterValuesMap = getFilterValuesMap(tuningFilter);
                tuningFiltersMap.put(filterValuesMap.first, filterValuesMap.second);
            }
            savedImageEditState.setAppliedTuningFilters(tuningFiltersMap);
            savedImageEditState.setAppliedFilter(getFilterValuesMap(filter));
        }
        isTuned.postValue(!tuningFilters.isEmpty());
        isFiltered.postValue(filter != null);
        setResultUri(destinationUri);
    }

    private SerializablePair<FilterType, Map<Integer, Object>> getFilterValuesMap(final Filter<?> filter) {
        if (filter == null) return null;
        final FilterType type = filter.getType();
        final Map<Integer, Property<?>> properties = filter.getProperties();
        final Map<Integer, Object> propertyValueMap = new HashMap<>();
        if (properties != null) {
            final Set<Map.Entry<Integer, Property<?>>> entries = properties.entrySet();
            for (final Map.Entry<Integer, Property<?>> entry : entries) {
                final Integer propId = entry.getKey();
                final Property<?> property = entry.getValue();
                final Object value = property.getValue();
                propertyValueMap.put(propId, value);
            }
        }
        return new SerializablePair<>(type, propertyValueMap);
    }

    public File getDestinationFile() {
        return destinationFile;
    }

    public enum Tab {
        RESULT,
        CROP,
        TUNE,
        FILTERS
    }
}
