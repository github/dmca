package awais.instagrabber.models;

import android.graphics.RectF;

import java.util.HashMap;
import java.util.Map;

import awais.instagrabber.fragments.imageedit.filters.FiltersHelper;
import awais.instagrabber.utils.SerializablePair;

public class SavedImageEditState {

    private final String sessionId;
    private final String originalPath;

    private float[] cropImageMatrixValues; // 9 values of matrix
    private RectF cropRect;
    private HashMap<FiltersHelper.FilterType, Map<Integer, Object>> appliedTuningFilters;
    private SerializablePair<FiltersHelper.FilterType, Map<Integer, Object>> appliedFilter;

    public SavedImageEditState(final String sessionId, String originalPath) {
        this.sessionId = sessionId;
        this.originalPath = originalPath;
    }

    public String getSessionId() {
        return sessionId;
    }

    public String getOriginalPath() {
        return originalPath;
    }

    public float[] getCropImageMatrixValues() {
        return cropImageMatrixValues;
    }

    public void setCropImageMatrixValues(float[] cropImageMatrixValues) {
        this.cropImageMatrixValues = cropImageMatrixValues;
    }

    public RectF getCropRect() {
        return cropRect;
    }

    public void setCropRect(RectF cropRect) {
        this.cropRect = cropRect;
    }

    public HashMap<FiltersHelper.FilterType, Map<Integer, Object>> getAppliedTuningFilters() {
        return appliedTuningFilters;
    }

    public void setAppliedTuningFilters(final HashMap<FiltersHelper.FilterType, Map<Integer, Object>> appliedTuningFilters) {
        this.appliedTuningFilters = appliedTuningFilters;
    }

    public SerializablePair<FiltersHelper.FilterType, Map<Integer, Object>> getAppliedFilter() {
        return appliedFilter;
    }

    public void setAppliedFilter(final SerializablePair<FiltersHelper.FilterType, Map<Integer, Object>> appliedFilter) {
        this.appliedFilter = appliedFilter;
    }
}

