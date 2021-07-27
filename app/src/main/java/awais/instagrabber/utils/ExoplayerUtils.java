package awais.instagrabber.utils;

import android.content.Context;

import androidx.annotation.NonNull;

import com.google.android.exoplayer2.database.ExoDatabaseProvider;
import com.google.android.exoplayer2.source.DefaultMediaSourceFactory;
import com.google.android.exoplayer2.upstream.DefaultHttpDataSourceFactory;
import com.google.android.exoplayer2.upstream.cache.CacheDataSource;
import com.google.android.exoplayer2.upstream.cache.LeastRecentlyUsedCacheEvictor;
import com.google.android.exoplayer2.upstream.cache.SimpleCache;

public final class ExoplayerUtils {
    private static final long MAX_CACHE_BYTES = 1048576;
    private static final LeastRecentlyUsedCacheEvictor cacheEvictor = new LeastRecentlyUsedCacheEvictor(MAX_CACHE_BYTES);

    public static DefaultMediaSourceFactory getCachedMediaSourceFactory(@NonNull final Context context) {
        final ExoDatabaseProvider exoDatabaseProvider = new ExoDatabaseProvider(context);
        final SimpleCache simpleCache = new SimpleCache(context.getCacheDir(), cacheEvictor, exoDatabaseProvider);
        final CacheDataSource.Factory cacheDataSourceFactory = new CacheDataSource.Factory()
                .setCache(simpleCache)
                .setUpstreamDataSourceFactory(new DefaultHttpDataSourceFactory());
        return new DefaultMediaSourceFactory(cacheDataSourceFactory);
    }
}
