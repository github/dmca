package awais.instagrabber.customviews.masoudss_waveform;

import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.nfc.FormatException;
import android.os.Build;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

final class SoundParser {
    private ProgressListener progressListener;
    int[] frameGains;
    //////////////////
    private static String[] supportedExtensions = {"mp3", "wav", "3gpp", "3gp", "amr", "aac", "m4a", "ogg"};
    private static ArrayList<String> additionalExtensions = new ArrayList<>();

    static void addCustomExtension(final String extension) {
        additionalExtensions.add(extension);
    }

    static void removeCustomExtension(final String extension) {
        additionalExtensions.remove(extension);
    }

    static void addCustomExtensions(final List<String> extensions) {
        additionalExtensions.addAll(extensions);
    }

    static void removeCustomExtensions(final List<String> extensions) {
        additionalExtensions.removeAll(extensions);
    }

    private static boolean isFilenameSupported(final String filename) {
        for (final String supportedExtension : supportedExtensions)
            if (filename.endsWith('.' + supportedExtension)) return true;
        for (final String additionalExtension : additionalExtensions)
            if (filename.endsWith('.' + additionalExtension)) return true;
        return false;
    }

    @NonNull
    public static SoundParser create(final String fileName, final boolean ignoreExtension) throws IOException, FormatException {
        if (!ignoreExtension && !isFilenameSupported(fileName))
            throw new FormatException("Not supported file extension.");

        final File f = new File(fileName);
        if (!f.exists()) throw new FileNotFoundException(fileName);

        final SoundParser soundFile = new SoundParser();
        soundFile.readFile(f);

        return soundFile;
    }

    public void setProgressListener(final ProgressListener progressListener) {
        this.progressListener = progressListener;
    }

    @SuppressWarnings("deprecation")
    private void readFile(@NonNull final File inputFile) throws IOException, FormatException {
        final MediaExtractor extractor = new MediaExtractor();
        MediaFormat format = null;

        final int fileSizeBytes = (int) inputFile.length();
        extractor.setDataSource(inputFile.getPath());

        final int numTracks = extractor.getTrackCount();

        int i = 0;
        while (i < numTracks) {
            format = extractor.getTrackFormat(i);
            if (Objects.requireNonNull(format.getString(MediaFormat.KEY_MIME)).startsWith("audio/")) {
                extractor.selectTrack(i);
                break;
            }
            i++;
        }

        if (i == numTracks) throw new FormatException("No audio track found in " + inputFile);
        assert format != null;

        final int channels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
        final int sampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);

        final int expectedNumSamples = (int) (format.getLong(MediaFormat.KEY_DURATION) / 1000000f * sampleRate + 0.5f);

        final MediaCodec codec = MediaCodec.createDecoderByType(Objects.requireNonNull(format.getString(MediaFormat.KEY_MIME)));
        codec.configure(format, null, null, 0);
        codec.start();

        final MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
        final ByteBuffer[] inputBuffers = codec.getInputBuffers();

        boolean firstSampleData = true, doneReading = false;
        long presentationTime;
        int sampleSize, decodedSamplesSize = 0, totSizeRead = 0;
        byte[] decodedSamples = null;
        ByteBuffer mDecodedBytes = ByteBuffer.allocate(1 << 20);
        ByteBuffer[] outputBuffers = codec.getOutputBuffers();

        while (true) {
            final int inputBufferIndex = codec.dequeueInputBuffer(100);

            if (!doneReading && inputBufferIndex >= 0) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
                    sampleSize = extractor.readSampleData(Objects.requireNonNull(codec.getInputBuffer(inputBufferIndex)), 0);
                else
                    sampleSize = extractor.readSampleData(inputBuffers[inputBufferIndex], 0);

                if (firstSampleData && sampleSize == 2 && "audio/mp4a-latm".equals(format.getString(MediaFormat.KEY_MIME))) {
                    extractor.advance();
                    totSizeRead += sampleSize;
                } else if (sampleSize < 0) {
                    codec.queueInputBuffer(inputBufferIndex, 0, 0, -1, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                    doneReading = true;
                } else {
                    presentationTime = extractor.getSampleTime();
                    codec.queueInputBuffer(inputBufferIndex, 0, sampleSize, presentationTime, 0);
                    extractor.advance();
                    totSizeRead += sampleSize;

                    if (progressListener != null && !progressListener.reportProgress((double) totSizeRead / fileSizeBytes)) {
                        // We are asked to stop reading the file. Returning immediately.
                        // The SoundFile object is invalid and should NOT be used afterward!
                        extractor.release();
                        codec.stop();
                        codec.release();
                        return;
                    }
                }

                firstSampleData = false;
            }

            // Get decoded stream from the decoder output buffers.
            final int outputBufferIndex = codec.dequeueOutputBuffer(info, 100);
            if (outputBufferIndex >= 0 && info.size > 0) {
                if (decodedSamplesSize < info.size) {
                    decodedSamplesSize = info.size;
                    decodedSamples = new byte[decodedSamplesSize];
                }

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    final ByteBuffer outputBuffer = codec.getOutputBuffer(outputBufferIndex);
                    assert outputBuffer != null;
                    outputBuffer.get(decodedSamples, 0, info.size);
                    outputBuffer.clear();
                } else {
                    outputBuffers[outputBufferIndex].get(decodedSamples, 0, info.size);
                    outputBuffers[outputBufferIndex].clear();
                }

                // Check if buffer is big enough. Resize it if it's too small.
                if (mDecodedBytes.remaining() < info.size) {
                    // Getting a rough estimate of the total size, allocate 20% more, and
                    // make sure to allocate at least 5MB more than the initial size.
                    final int position = mDecodedBytes.position();

                    int newSize = (int) (position * (1.0 * fileSizeBytes / totSizeRead) * 1.2);
                    final int infoSize = info.size + 5 * (1 << 20);
                    if (newSize - position < infoSize)
                        newSize = position + infoSize;

                    ByteBuffer newDecodedBytes = null;

                    // Try to allocate memory. If we are OOM, try to run the garbage collector.
                    int retry = 10;
                    while (retry > 0) {
                        try {
                            newDecodedBytes = ByteBuffer.allocate(newSize);
                            break;
                        } catch (final OutOfMemoryError e) {
                            retry--;
                        }
                    }
                    if (retry == 0) break;
                    mDecodedBytes.rewind();
                    assert newDecodedBytes != null;
                    newDecodedBytes.put(mDecodedBytes);
                    mDecodedBytes = newDecodedBytes;
                    mDecodedBytes.position(position);
                }

                mDecodedBytes.put(decodedSamples, 0, info.size);
                codec.releaseOutputBuffer(outputBufferIndex, false);
            } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED)
                    outputBuffers = codec.getOutputBuffers();
            }

            if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0 || mDecodedBytes.position() / (2 * channels) >= expectedNumSamples)
                break;
        }

        final int numSamples = mDecodedBytes.position() / (channels * 2);  // One sample = 2 bytes.
        mDecodedBytes.rewind();
        mDecodedBytes.order(ByteOrder.LITTLE_ENDIAN);
        final ShortBuffer mDecodedSamples = mDecodedBytes.asShortBuffer();
        // final int avgBitrateKbps = (int) (fileSizeBytes * 8F * ((float) sampleRate / numSamples) / 1000F);

        extractor.release();
        codec.stop();
        codec.release();

        final int samplesPerFrame = 1024;
        int numFrames = numSamples / samplesPerFrame;
        if (numSamples % samplesPerFrame != 0) numFrames++;
        frameGains = new int[numFrames];
        // final int[] mFrameLens = new int[numFrames];
        // final int[] mFrameOffsets = new int[numFrames];
        // final int frameLens = (int) (1000F * avgBitrateKbps / 8F * ((float) samplesPerFrame / sampleRate));
        int j, gain, value;

        i = 0;
        while (i < numFrames) {
            gain = -1;
            j = 0;

            while (j < samplesPerFrame) {
                value = 0;
                for (int k = 0; k < channels; ++k)
                    if (mDecodedSamples.remaining() > 0)
                        value += Math.abs(mDecodedSamples.get());
                value /= channels;
                if (gain < value) gain = value;
                j++;
            }

            frameGains[i] = (int) Math.sqrt(gain);
            // mFrameLens[i] = frameLens;
            // mFrameOffsets[i] = (int) ((float) i * (1000F * avgBitrateKbps / 8F) * ((float) samplesPerFrame / sampleRate));
            i++;
        }

        mDecodedSamples.rewind();
    }

    private interface ProgressListener {
        boolean reportProgress(final double fractionComplete);
    }
}