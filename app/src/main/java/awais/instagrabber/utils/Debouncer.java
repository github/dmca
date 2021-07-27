package awais.instagrabber.utils;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class Debouncer<T> {
    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
    private final ConcurrentHashMap<T, TimerTask> delayedMap = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<T, ScheduledFuture<?>> futureMap = new ConcurrentHashMap<>();
    private final Callback<T> callback;
    private final int interval;

    public Debouncer(Callback<T> c, int interval) {
        this.callback = c;
        this.interval = interval;
    }

    public void call(T key) {
        TimerTask task = new TimerTask(key);

        TimerTask prev;
        do {
            prev = delayedMap.putIfAbsent(key, task);
            if (prev == null) {
                final ScheduledFuture<?> future = scheduler.schedule(task, interval, TimeUnit.MILLISECONDS);
                futureMap.put(key, future);
            }
        } while (prev != null && !prev.extend()); // Exit only if new task was added to map, or existing task was extended successfully
    }

    public void terminate() {
        scheduler.shutdownNow();
    }

    public void cancel(final T key) {
        delayedMap.remove(key);
        final ScheduledFuture<?> future = futureMap.get(key);
        if (future != null) {
            future.cancel(true);
        }
    }

    // The task that wakes up when the wait time elapses
    private class TimerTask implements Runnable {
        private final T key;
        private long dueTime;
        private final Object lock = new Object();

        public TimerTask(T key) {
            this.key = key;
            extend();
        }

        public boolean extend() {
            synchronized (lock) {
                if (dueTime < 0) // Task has been shutdown
                    return false;
                dueTime = System.currentTimeMillis() + interval;
                return true;
            }
        }

        public void run() {
            synchronized (lock) {
                long remaining = dueTime - System.currentTimeMillis();
                if (remaining > 0) { // Re-schedule task
                    scheduler.schedule(this, remaining, TimeUnit.MILLISECONDS);
                } else { // Mark as terminated and invoke callback
                    dueTime = -1;
                    try {
                        callback.call(key);
                    } catch (Exception e) {
                        callback.onError(e);
                    } finally {
                        delayedMap.remove(key);
                    }
                }
            }
        }
    }

    public interface Callback<T> {
        void call(T key);

        void onError(Throwable t);
    }
}
