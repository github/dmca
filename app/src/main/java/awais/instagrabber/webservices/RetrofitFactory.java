package awais.instagrabber.webservices;

import com.google.gson.FieldNamingPolicy;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import java.io.File;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.repositories.responses.Caption;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.webservices.interceptors.AddCookiesInterceptor;
import awais.instagrabber.webservices.interceptors.IgErrorsInterceptor;
import okhttp3.Cache;
import okhttp3.OkHttpClient;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;
import retrofit2.converter.scalars.ScalarsConverterFactory;

public final class RetrofitFactory {
    private static final Object LOCK = new Object();

    private static RetrofitFactory instance;

    private final int cacheSize = 10 * 1024 * 1024; // 10 MB
    private final Cache cache = new Cache(new File(Utils.cacheDir), cacheSize);

    private IgErrorsInterceptor igErrorsInterceptor;
    private Retrofit.Builder builder;
    private Retrofit retrofit;
    private Retrofit retrofitWeb;

    public static RetrofitFactory getInstance() {
        if (instance == null) {
            synchronized (LOCK) {
                if (instance == null) {
                    instance = new RetrofitFactory();
                }
            }
        }
        return instance;
    }

    private Retrofit.Builder getRetrofitBuilder() {
        if (builder == null) {
            igErrorsInterceptor = new IgErrorsInterceptor();
            final OkHttpClient.Builder clientBuilder = new OkHttpClient.Builder()
                    .followRedirects(false)
                    .followSslRedirects(false)
                    .cache(cache);
            if (BuildConfig.DEBUG) {
                // clientBuilder.addInterceptor(new LoggingInterceptor());
            }
            clientBuilder.addInterceptor(new AddCookiesInterceptor())
                         .addInterceptor(igErrorsInterceptor);
            final Gson gson = new GsonBuilder()
                    .setFieldNamingPolicy(FieldNamingPolicy.LOWER_CASE_WITH_UNDERSCORES)
                    .registerTypeAdapter(Caption.class, new Caption.CaptionDeserializer())
                    .setLenient()
                    .create();
            builder = new Retrofit.Builder()
                    .addConverterFactory(ScalarsConverterFactory.create())
                    .addConverterFactory(GsonConverterFactory.create(gson))
                    .client(clientBuilder.build());
        }
        return builder;
    }

    public Retrofit getRetrofit() {
        if (retrofit == null) {
            retrofit = getRetrofitBuilder()
                    .baseUrl("https://i.instagram.com")
                    .build();
        }
        return retrofit;
    }

    public Retrofit getRetrofitWeb() {
        if (retrofitWeb == null) {
            retrofitWeb = getRetrofitBuilder()
                    .baseUrl("https://www.instagram.com")
                    .build();
        }
        return retrofitWeb;
    }

    public void destroy() {
        if (igErrorsInterceptor != null) {
            igErrorsInterceptor.destroy();
        }
        igErrorsInterceptor = null;
        retrofit = null;
        retrofitWeb = null;
        builder = null;
        instance = null;
    }
}
