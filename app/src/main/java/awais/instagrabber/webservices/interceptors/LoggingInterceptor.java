package awais.instagrabber.webservices.interceptors;

import android.util.Log;

import androidx.annotation.NonNull;

import java.io.IOException;

import okhttp3.Interceptor;
import okhttp3.MediaType;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;

public class LoggingInterceptor implements Interceptor {
    private static final String TAG = "LoggingInterceptor";

    @NonNull
    @Override
    public Response intercept(Interceptor.Chain chain) throws IOException {
        final Request request = chain.request();
        long t1 = System.nanoTime();
        Log.i(TAG, String.format("Sending request %s on %s%n%s",
                                 request.url(), chain.connection(), request.headers()));
        final Response response = chain.proceed(request);
        long t2 = System.nanoTime();
        Log.i(TAG, String.format("Received response for %s in %.1fms%n%s", response.request().url(), (t2 - t1) / 1e6d, response.headers()));
        final ResponseBody body = response.body();
        MediaType contentType = null;
        String content = "";
        if (body != null) {
            contentType = body.contentType();
            try {
                content = body.string();
            } catch (Exception e) {
                Log.e(TAG, "intercept: ", e);
            }
            Log.d(TAG, content);
        }
        final ResponseBody wrappedBody = ResponseBody.create(contentType, content);
        return response.newBuilder()
                       .body(wrappedBody)
                       .build();
    }
}
