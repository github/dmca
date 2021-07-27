package awais.instagrabber.activities;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.hardware.display.DisplayManager;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.webkit.MimeTypeMap;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.camera.core.CameraInfoUnavailableException;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.ImageCapture;
import androidx.camera.core.ImageCaptureException;
import androidx.camera.core.Preview;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.core.content.ContextCompat;

import com.google.common.io.Files;
import com.google.common.util.concurrent.ListenableFuture;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Locale;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import awais.instagrabber.databinding.ActivityCameraBinding;
import awais.instagrabber.utils.DirectoryUtils;
import awais.instagrabber.utils.PermissionUtils;
import awais.instagrabber.utils.Utils;

public class CameraActivity extends BaseLanguageActivity {
    private static final String TAG = CameraActivity.class.getSimpleName();
    private static final int CAMERA_REQUEST_CODE = 100;
    private static final String FILE_FORMAT = "yyyy-MM-dd-HH-mm-ss-SSS";
    private static final SimpleDateFormat SIMPLE_DATE_FORMAT = new SimpleDateFormat(FILE_FORMAT, Locale.US);

    private ActivityCameraBinding binding;
    private ImageCapture imageCapture;
    private File outputDirectory;
    private ExecutorService cameraExecutor;
    private int displayId = -1;

    private final DisplayManager.DisplayListener displayListener = new DisplayManager.DisplayListener() {
        @Override
        public void onDisplayAdded(final int displayId) {}

        @Override
        public void onDisplayRemoved(final int displayId) {}

        @Override
        public void onDisplayChanged(final int displayId) {
            if (displayId == CameraActivity.this.displayId) {
                imageCapture.setTargetRotation(binding.getRoot().getDisplay().getRotation());
            }
        }
    };
    private DisplayManager displayManager;
    private ProcessCameraProvider cameraProvider;
    private int lensFacing;

    @Override
    protected void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityCameraBinding.inflate(LayoutInflater.from(getBaseContext()));
        setContentView(binding.getRoot());
        Utils.transparentStatusBar(this, true, false);
        displayManager = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);
        outputDirectory = DirectoryUtils.getOutputMediaDirectory(this, "Camera");
        cameraExecutor = Executors.newSingleThreadExecutor();
        displayManager.registerDisplayListener(displayListener, null);
        binding.viewFinder.post(() -> {
            displayId = binding.viewFinder.getDisplay().getDisplayId();
            updateUi();
            checkPermissionsAndSetupCamera();
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        // Make sure that all permissions are still present, since the
        // user could have removed them while the app was in paused state.
        if (!PermissionUtils.hasCameraPerms(this)) {
            PermissionUtils.requestCameraPerms(this, CAMERA_REQUEST_CODE);
        }
    }

    @Override
    public void onConfigurationChanged(@NonNull final Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        // Redraw the camera UI controls
        updateUi();

        // Enable or disable switching between cameras
        updateCameraSwitchButton();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Utils.transparentStatusBar(this, false, false);
        cameraExecutor.shutdown();
        displayManager.unregisterDisplayListener(displayListener);
    }

    private void updateUi() {
        binding.cameraCaptureButton.setOnClickListener(v -> takePhoto());
        // Disable the button until the camera is set up
        binding.switchCamera.setEnabled(false);
        // Listener for button used to switch cameras. Only called if the button is enabled
        binding.switchCamera.setOnClickListener(v -> {
            lensFacing = CameraSelector.LENS_FACING_FRONT == lensFacing ? CameraSelector.LENS_FACING_BACK
                                                                        : CameraSelector.LENS_FACING_FRONT;
            // Re-bind use cases to update selected camera
            bindCameraUseCases();
        });
        binding.close.setOnClickListener(v -> {
            setResult(Activity.RESULT_CANCELED);
            finish();
        });
    }

    private void checkPermissionsAndSetupCamera() {
        if (PermissionUtils.hasCameraPerms(this)) {
            setupCamera();
            return;
        }
        PermissionUtils.requestCameraPerms(this, CAMERA_REQUEST_CODE);
    }

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        if (requestCode == CAMERA_REQUEST_CODE) {
            if (PermissionUtils.hasCameraPerms(this)) {
                setupCamera();
            }
        }
    }

    private void setupCamera() {
        final ListenableFuture<ProcessCameraProvider> cameraProviderFuture = ProcessCameraProvider.getInstance(this);
        cameraProviderFuture.addListener(() -> {
            try {
                cameraProvider = cameraProviderFuture.get();
                // Select lensFacing depending on the available cameras
                lensFacing = -1;
                if (hasBackCamera()) {
                    lensFacing = CameraSelector.LENS_FACING_BACK;
                } else if (hasFrontCamera()) {
                    lensFacing = CameraSelector.LENS_FACING_FRONT;
                }
                if (lensFacing == -1) {
                    throw new IllegalStateException("Back and front camera are unavailable");
                }
                // Enable or disable switching between cameras
                updateCameraSwitchButton();
                // Build and bind the camera use cases
                bindCameraUseCases();
            } catch (ExecutionException | InterruptedException | CameraInfoUnavailableException e) {
                Log.e(TAG, "setupCamera: ", e);
            }

        }, ContextCompat.getMainExecutor(this));
    }

    private void bindCameraUseCases() {
        final int rotation = binding.viewFinder.getDisplay().getRotation();

        // CameraSelector
        final CameraSelector cameraSelector = new CameraSelector.Builder()
                .requireLensFacing(lensFacing)
                .build();

        // Preview
        final Preview preview = new Preview.Builder()
                // Set initial target rotation
                .setTargetRotation(rotation)
                .build();

        // ImageCapture
        imageCapture = new ImageCapture.Builder()
                .setCaptureMode(ImageCapture.CAPTURE_MODE_MINIMIZE_LATENCY)
                // Set initial target rotation, we will have to call this again if rotation changes
                // during the lifecycle of this use case
                .setTargetRotation(rotation)
                .build();

        cameraProvider.unbindAll();
        cameraProvider.bindToLifecycle(this, cameraSelector, preview, imageCapture);

        preview.setSurfaceProvider(binding.viewFinder.getSurfaceProvider());
    }

    private void takePhoto() {
        if (imageCapture == null) return;
        final File photoFile = new File(outputDirectory, SIMPLE_DATE_FORMAT.format(System.currentTimeMillis()) + ".jpg");
        final ImageCapture.OutputFileOptions outputFileOptions = new ImageCapture.OutputFileOptions.Builder(photoFile).build();
        imageCapture.takePicture(
                outputFileOptions,
                cameraExecutor,
                new ImageCapture.OnImageSavedCallback() {
                    @Override
                    public void onImageSaved(@NonNull final ImageCapture.OutputFileResults outputFileResults) {
                        final Uri uri = Uri.fromFile(photoFile);
                        //noinspection UnstableApiUsage
                        final String mimeType = MimeTypeMap.getSingleton()
                                                           .getMimeTypeFromExtension(Files.getFileExtension(photoFile.getName()));
                        MediaScannerConnection.scanFile(
                                CameraActivity.this,
                                new String[]{photoFile.getAbsolutePath()},
                                new String[]{mimeType},
                                (path, uri1) -> {
                                    Log.d(TAG, "onImageSaved: scan complete");
                                    final Intent intent = new Intent();
                                    intent.setData(uri1);
                                    setResult(Activity.RESULT_OK, intent);
                                    finish();
                                });
                        Log.d(TAG, "onImageSaved: " + uri);
                    }

                    @Override
                    public void onError(@NonNull final ImageCaptureException exception) {
                        Log.e(TAG, "onError: ", exception);
                    }
                }
        );
        // We can only change the foreground Drawable using API level 23+ API
        // if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        //     // Display flash animation to indicate that photo was captured
        //     final ConstraintLayout container = binding.getRoot();
        //     container.postDelayed(() -> {
        //         container.setForeground(new ColorDrawable(Color.WHITE));
        //         container.postDelayed(() -> container.setForeground(null), 50);
        //     }, 100);
        // }
    }

    /**
     * Enabled or disabled a button to switch cameras depending on the available cameras
     */
    private void updateCameraSwitchButton() {
        try {
            binding.switchCamera.setEnabled(hasBackCamera() && hasFrontCamera());
        } catch (CameraInfoUnavailableException e) {
            binding.switchCamera.setEnabled(false);
        }
    }

    /**
     * Returns true if the device has an available back camera. False otherwise
     */
    private boolean hasBackCamera() throws CameraInfoUnavailableException {
        if (cameraProvider == null) return false;
        return cameraProvider.hasCamera(CameraSelector.DEFAULT_BACK_CAMERA);
    }

    /**
     * Returns true if the device has an available front camera. False otherwise
     */
    private boolean hasFrontCamera() throws CameraInfoUnavailableException {
        if (cameraProvider == null) {
            return false;
        }
        return cameraProvider.hasCamera(CameraSelector.DEFAULT_FRONT_CAMERA);
    }
}
