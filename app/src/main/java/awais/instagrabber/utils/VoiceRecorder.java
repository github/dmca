package awais.instagrabber.utils;

import android.media.MediaRecorder;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.webkit.MimeTypeMap;

import androidx.annotation.NonNull;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

public class VoiceRecorder {
    private static final String TAG = VoiceRecorder.class.getSimpleName();
    private static final String FILE_PREFIX = "recording";
    private static final String EXTENSION = "mp4";
    private static final String MIME_TYPE = MimeTypeMap.getSingleton().getMimeTypeFromExtension(EXTENSION);
    private static final int AUDIO_SAMPLE_RATE = 44100;
    private static final int AUDIO_BIT_DEPTH = 16;
    private static final int AUDIO_BIT_RATE = AUDIO_SAMPLE_RATE * AUDIO_BIT_DEPTH;
    private static final String FILE_FORMAT = "yyyy-MM-dd-HH-mm-ss-SSS";
    private static final SimpleDateFormat SIMPLE_DATE_FORMAT = new SimpleDateFormat(FILE_FORMAT, Locale.US);

    private final List<Float> waveform = new ArrayList<>();
    private final File recordingsDir;
    private final VoiceRecorderCallback callback;

    private MediaRecorder recorder;
    private File audioTempFile;
    private MaxAmpHandler maxAmpHandler;
    private boolean stopped;

    public VoiceRecorder(@NonNull final File recordingsDir, final VoiceRecorderCallback callback) {
        this.recordingsDir = recordingsDir;
        this.callback = callback;
    }

    public void startRecording() {
        stopped = false;
        try {
            recorder = new MediaRecorder();
            recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
            recorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
            deleteTempAudioFile();
            audioTempFile = getAudioRecordFile();
            recorder.setOutputFile(audioTempFile.getAbsolutePath());
            recorder.setAudioEncoder(MediaRecorder.AudioEncoder.HE_AAC);
            recorder.setAudioEncodingBitRate(AUDIO_BIT_RATE);
            recorder.setAudioSamplingRate(AUDIO_SAMPLE_RATE);
            recorder.prepare();
            waveform.clear();
            maxAmpHandler = new MaxAmpHandler(waveform);
            recorder.start();
            if (callback != null) {
                callback.onStart();
            }
            getMaxAmp();
        } catch (Exception e) {
            Log.e(TAG, "Audio recording failed", e);
            deleteTempAudioFile();
        }
    }

    public void stopRecording(final boolean cancelled) {
        stopped = true;
        if (maxAmpHandler != null) {
            maxAmpHandler.removeCallbacks(getMaxAmpRunnable);
        }
        if (recorder == null) {
            if (callback != null) {
                callback.onCancel();
            }
            return;
        }
        try {
            recorder.stop();
            recorder.release();
            recorder = null;
            // processWaveForm();
        } catch (Exception e) {
            Log.e(TAG, "stopRecording: error", e);
            deleteTempAudioFile();
        }
        if (cancelled) {
            deleteTempAudioFile();
            if (callback != null) {
                callback.onCancel();
            }
            return;
        }
        if (callback != null) {
            callback.onComplete(new VoiceRecordingResult(MIME_TYPE, audioTempFile, waveform));
        }
    }

    private static class MaxAmpHandler extends Handler {
        private final List<Float> waveform;

        public MaxAmpHandler(final List<Float> waveform) {
            this.waveform = waveform;
        }

        @Override
        public void handleMessage(@NonNull final Message msg) {
            if (waveform == null) return;
            waveform.add(msg.obj instanceof Float ? (Float) msg.obj : 0f);
        }
    }

    private final Runnable getMaxAmpRunnable = this::getMaxAmp;

    private void getMaxAmp() {
        if (stopped || recorder == null || maxAmpHandler == null) return;
        final float value = (float) Math.pow(2.0d, (Math.log10((double) recorder.getMaxAmplitude() / 2700.0d) * 20.0d) / 6.0d);
        maxAmpHandler.postDelayed(getMaxAmpRunnable, 100);
        Message msg = Message.obtain();
        msg.obj = value;
        maxAmpHandler.sendMessage(msg);
    }

    // private void processWaveForm() {
    //     // if (waveform == null || waveform.isEmpty()) return;
    //     final Optional<Float> maxAmplitudeOptional = waveform.stream().max(Float::compareTo);
    //     if (!maxAmplitudeOptional.isPresent()) return;
    //     final float maxAmp = maxAmplitudeOptional.get();
    //     final List<Float> normalised = waveform.stream()
    //                                            .map(amp -> amp / maxAmp)
    //                                            .map(amp -> amp < 0.01f ? 0f : amp)
    //                                            .collect(Collectors.toList());
    //     // final List<Float> normalised = waveform.stream()
    //     //                                        .map(amp -> amp * 1.0f / 32768)
    //     //                                        .collect(Collectors.toList());
    //     // Log.d(TAG, "processWaveForm: " + waveform);
    //     Log.d(TAG, "processWaveForm: " + normalised);
    // }

    @NonNull
    private File getAudioRecordFile() {
        final String name = String.format("%s-%s.%s", FILE_PREFIX, SIMPLE_DATE_FORMAT.format(new Date()), EXTENSION);
        return new File(recordingsDir, name);
    }

    private void deleteTempAudioFile() {
        if (audioTempFile == null) {
            //noinspection ResultOfMethodCallIgnored
            getAudioRecordFile().delete();
            return;
        }
        final boolean deleted = audioTempFile.delete();
        if (!deleted) {
            Log.w(TAG, "stopRecording: file not deleted");
        }
        audioTempFile = null;
    }

    public static class VoiceRecordingResult {
        private final String mimeType;
        private final File file;
        private final List<Float> waveform;
        private final int samplingFreq = 10;

        public VoiceRecordingResult(final String mimeType, final File file, final List<Float> waveform) {
            this.mimeType = mimeType;
            this.file = file;
            this.waveform = waveform;
        }

        public String getMimeType() {
            return mimeType;
        }

        public File getFile() {
            return file;
        }

        public List<Float> getWaveform() {
            return waveform;
        }

        public int getSamplingFreq() {
            return samplingFreq;
        }
    }

    public interface VoiceRecorderCallback {
        void onStart();

        void onComplete(final VoiceRecordingResult voiceRecordingResult);

        void onCancel();
    }
}
