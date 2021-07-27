package awais.instagrabber.fragments.imageedit;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.RectF;
import android.net.Uri;
import android.os.Bundle;
import android.os.Parcelable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.activity.OnBackPressedCallback;
import androidx.activity.OnBackPressedDispatcher;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.lifecycle.SavedStateHandle;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavBackStackEntry;
import androidx.navigation.NavController;
import androidx.navigation.fragment.NavHostFragment;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.imagepipeline.request.ImageRequestBuilder;
import com.yalantis.ucrop.UCrop;
import com.yalantis.ucrop.UCropActivity;
import com.yalantis.ucrop.UCropFragment;
import com.yalantis.ucrop.UCropFragmentCallback;

import java.io.File;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.databinding.FragmentImageEditBinding;
import awais.instagrabber.fragments.imageedit.filters.filters.Filter;
import awais.instagrabber.models.SavedImageEditState;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.ImageEditViewModel;

public class ImageEditFragment extends Fragment {
    private static final String TAG = ImageEditFragment.class.getSimpleName();
    private static final String ARGS_URI = "uri";
    private static final String FILTERS_FRAGMENT_TAG = "Filters";

    private FragmentImageEditBinding binding;
    private ImageEditViewModel viewModel;
    private ImageEditViewModel.Tab previousTab;
    private FiltersFragment filtersFragment;

    private final OnBackPressedCallback onBackPressedCallback = new OnBackPressedCallback(false) {
        @Override
        public void handleOnBackPressed() {
            setEnabled(false);
            remove();
            if (previousTab != ImageEditViewModel.Tab.CROP
                    && previousTab != ImageEditViewModel.Tab.TUNE
                    && previousTab != ImageEditViewModel.Tab.FILTERS) {
                return;
            }
            final FragmentManager fragmentManager = getChildFragmentManager();
            final FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
            fragmentTransaction.setReorderingAllowed(true)
                               .remove(previousTab == ImageEditViewModel.Tab.CROP ? uCropFragment : filtersFragment)
                               .commit();
            viewModel.setCurrentTab(ImageEditViewModel.Tab.RESULT);
        }
    };
    private FragmentActivity fragmentActivity;
    private UCropFragment uCropFragment;

    public static ImageEditFragment newInstance(final Uri uri) {
        final Bundle args = new Bundle();
        args.putParcelable(ARGS_URI, uri);
        final ImageEditFragment fragment = new ImageEditFragment();
        fragment.setArguments(args);
        return fragment;
    }

    public ImageEditFragment() {}

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = getActivity();
        viewModel = new ViewModelProvider(this).get(ImageEditViewModel.class);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        binding = FragmentImageEditBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        init();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
    }

    private void init() {
        setupObservers();
        final Bundle arguments = getArguments();
        if (arguments == null) return;
        final Parcelable parcelable = arguments.getParcelable(ARGS_URI);
        Uri originalUri = null;
        if (parcelable instanceof Uri) {
            originalUri = (Uri) parcelable;
        }
        if (originalUri == null) return;
        viewModel.setOriginalUri(originalUri);
        viewModel.setCurrentTab(ImageEditViewModel.Tab.RESULT);
    }

    private void setupObservers() {
        viewModel.isLoading().observe(getViewLifecycleOwner(), loading -> {});
        viewModel.getCurrentTab().observe(getViewLifecycleOwner(), tab -> {
            if (tab == null) return;
            switch (tab) {
                case RESULT:
                    setupResult();
                    break;
                case CROP:
                    setupCropFragment();
                    break;
                case TUNE:
                case FILTERS:
                    setupFilterFragment();
                    break;
            }
            previousTab = tab;
        });
        viewModel.isCropped().observe(getViewLifecycleOwner(), isCropped -> binding.crop.setSelected(isCropped));
        viewModel.isTuned().observe(getViewLifecycleOwner(), isTuned -> binding.tune.setSelected(isTuned));
        viewModel.isFiltered().observe(getViewLifecycleOwner(), isFiltered -> binding.filters.setSelected(isFiltered));
        viewModel.getResultUri().observe(getViewLifecycleOwner(), uri -> {
            if (uri == null) {
                binding.preview.setController(null);
                return;
            }
            binding.preview.setController(Fresco.newDraweeControllerBuilder()
                                                .setImageRequest(ImageRequestBuilder.newBuilderWithSource(uri)
                                                                                    .disableDiskCache()
                                                                                    .disableMemoryCache()
                                                                                    .build())
                                                .build());
        });
    }

    private void setupResult() {
        binding.fragmentContainerView.setVisibility(View.GONE);
        binding.cropBottomControls.setVisibility(View.GONE);
        binding.preview.setVisibility(View.VISIBLE);
        binding.resultBottomControls.setVisibility(View.VISIBLE);
        binding.crop.setOnClickListener(v -> viewModel.setCurrentTab(ImageEditViewModel.Tab.CROP));
        binding.tune.setOnClickListener(v -> viewModel.setCurrentTab(ImageEditViewModel.Tab.TUNE));
        binding.filters.setOnClickListener(v -> viewModel.setCurrentTab(ImageEditViewModel.Tab.FILTERS));
        binding.cancel.setOnClickListener(v -> {
            viewModel.cancel();
            final NavController navController = NavHostFragment.findNavController(this);
            setNavControllerResult(navController, null);
            navController.navigateUp();
        });
        binding.done.setOnClickListener(v -> {
            final Context context = getContext();
            if (context == null) return;
            final Uri resultUri = viewModel.getResultUri().getValue();
            if (resultUri == null) return;
            Utils.mediaScanFile(context, new File(resultUri.toString()), (path, uri) -> AppExecutors.getInstance().mainThread().execute(() -> {
                final NavController navController = NavHostFragment.findNavController(this);
                setNavControllerResult(navController, resultUri);
                navController.navigateUp();
            }));
        });
    }

    private void setNavControllerResult(@NonNull final NavController navController, final Uri resultUri) {
        final NavBackStackEntry navBackStackEntry = navController.getPreviousBackStackEntry();
        if (navBackStackEntry == null) return;
        final SavedStateHandle savedStateHandle = navBackStackEntry.getSavedStateHandle();
        savedStateHandle.set("result", resultUri);
    }

    private void setupCropFragment() {
        final Context context = getContext();
        if (context == null) return;
        binding.preview.setVisibility(View.GONE);
        binding.resultBottomControls.setVisibility(View.GONE);
        binding.fragmentContainerView.setVisibility(View.VISIBLE);
        binding.cropBottomControls.setVisibility(View.VISIBLE);
        final UCrop.Options options = new UCrop.Options();
        options.setCompressionFormat(Bitmap.CompressFormat.JPEG);
        options.setFreeStyleCropEnabled(true);
        options.setAllowedGestures(UCropActivity.SCALE, UCropActivity.ROTATE, UCropActivity.ALL);
        final UCrop uCrop = UCrop.of(viewModel.getOriginalUri(), viewModel.getCropDestinationUri()).withOptions(options);
        final SavedImageEditState savedState = viewModel.getSavedImageEditState();
        if (savedState != null && savedState.getCropImageMatrixValues() != null && savedState.getCropRect() != null) {
            uCrop.withSavedState(savedState.getCropImageMatrixValues(), savedState.getCropRect());
        }
        uCropFragment = uCrop.getFragment(uCrop.getIntent(context).getExtras());
        final FragmentManager fragmentManager = getChildFragmentManager();
        uCropFragment.setCallback(new UCropFragmentCallback() {
            @Override
            public void loadingProgress(final boolean showLoader) {
                Log.d(TAG, "loadingProgress: " + showLoader);
            }

            @Override
            public void onCropFinish(final UCropFragment.UCropResult result) {
                Log.d(TAG, "onCropFinish: " + result.mResultCode);
                if (result.mResultCode == AppCompatActivity.RESULT_OK) {
                    final Intent resultData = result.mResultData;
                    final Bundle extras = resultData.getExtras();
                    if (extras == null) return;
                    final Object uri = extras.get(UCrop.EXTRA_OUTPUT_URI);
                    final Object imageMatrixValues = extras.get(UCrop.EXTRA_IMAGE_MATRIX_VALUES);
                    final Object cropRect = extras.get(UCrop.EXTRA_CROP_RECT);
                    if (uri instanceof Uri && imageMatrixValues instanceof float[] && cropRect instanceof RectF) {
                        Log.d(TAG, "onCropFinish: result uri: " + uri);
                        viewModel.setCropResult((float[]) imageMatrixValues, (RectF) cropRect);
                        viewModel.setCurrentTab(ImageEditViewModel.Tab.RESULT);
                    }
                }
            }
        });
        final FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.setReorderingAllowed(true)
                           .replace(R.id.fragment_container_view, uCropFragment, UCropFragment.TAG)
                           .commit();
        if (!onBackPressedCallback.isEnabled()) {
            final OnBackPressedDispatcher onBackPressedDispatcher = fragmentActivity.getOnBackPressedDispatcher();
            onBackPressedCallback.setEnabled(true);
            onBackPressedDispatcher.addCallback(getViewLifecycleOwner(), onBackPressedCallback);
        }
        binding.cropCancel.setOnClickListener(v -> onBackPressedCallback.handleOnBackPressed());
        binding.cropReset.setOnClickListener(v -> uCropFragment.reset());
        binding.cropDone.setOnClickListener(v -> uCropFragment.cropAndSaveImage());
    }

    private void setupFilterFragment() {
        binding.resultBottomControls.setVisibility(View.GONE);
        binding.preview.setVisibility(View.GONE);
        binding.cropBottomControls.setVisibility(View.GONE);
        binding.fragmentContainerView.setVisibility(View.VISIBLE);
        final Boolean isCropped = viewModel.isCropped().getValue();
        final Uri uri = isCropped != null && isCropped ? viewModel.getCropDestinationUri() : viewModel.getOriginalUri();
        final ImageEditViewModel.Tab value = viewModel.getCurrentTab().getValue();
        final SavedImageEditState savedImageEditState = viewModel.getSavedImageEditState();
        filtersFragment = FiltersFragment.newInstance(
                uri,
                viewModel.getDestinationUri(),
                savedImageEditState.getAppliedTuningFilters(),
                savedImageEditState.getAppliedFilter(),
                value == null ? ImageEditViewModel.Tab.TUNE : value
        );
        final FragmentManager fragmentManager = getChildFragmentManager();
        final FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.setReorderingAllowed(true)
                           .replace(R.id.fragment_container_view, filtersFragment, FILTERS_FRAGMENT_TAG)
                           .commit();
        if (!onBackPressedCallback.isEnabled()) {
            final OnBackPressedDispatcher onBackPressedDispatcher = fragmentActivity.getOnBackPressedDispatcher();
            onBackPressedCallback.setEnabled(true);
            onBackPressedDispatcher.addCallback(getViewLifecycleOwner(), onBackPressedCallback);
        }
        filtersFragment.setCallback(new FiltersFragment.FilterCallback() {
            @Override
            public void onApply(final Uri uri, final List<Filter<?>> tuningFilters, final Filter<?> filter) {
                viewModel.setAppliedFilters(tuningFilters, filter);
                viewModel.setCurrentTab(ImageEditViewModel.Tab.RESULT);
            }

            @Override
            public void onCancel() {
                onBackPressedCallback.handleOnBackPressed();
            }
        });
    }
}
