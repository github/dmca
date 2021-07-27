/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package awais.instagrabber.utils;

import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;

import com.google.common.util.concurrent.ListeningExecutorService;
import com.google.common.util.concurrent.MoreExecutors;

import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

/**
 * Global executor pools for the whole application.
 * <p>
 * Grouping tasks like this avoids the effects of task starvation (e.g. disk reads don't wait behind
 * webservice requests).
 */
public class AppExecutors {

    private static final int THREAD_COUNT = 3;
    private static final Object LOCK = new Object();

    private static AppExecutors instance;

    private final Executor diskIO;
    private final Executor networkIO;
    private final MainThreadExecutor mainThread;
    private final ListeningExecutorService tasksThread;

    private AppExecutors(Executor diskIO,
                         Executor networkIO,
                         MainThreadExecutor mainThread,
                         ListeningExecutorService tasksThread) {
        this.diskIO = diskIO;
        this.networkIO = networkIO;
        this.mainThread = mainThread;
        this.tasksThread = tasksThread;
    }

    public static AppExecutors getInstance() {
        if (instance == null) {
            synchronized (LOCK) {
                if (instance == null) {
                    instance = new AppExecutors(Executors.newSingleThreadExecutor(),
                                                Executors.newFixedThreadPool(THREAD_COUNT),
                                                new MainThreadExecutor(),
                                                MoreExecutors.listeningDecorator(Executors.newFixedThreadPool(10)));
                }
            }
        }
        return instance;
    }


    public Executor diskIO() {
        return diskIO;
    }

    public Executor networkIO() {
        return networkIO;
    }


    public ListeningExecutorService tasksThread() {
        return tasksThread;
    }

    public MainThreadExecutor mainThread() {
        return mainThread;
    }

    public static class MainThreadExecutor implements Executor {
        private final Handler mainThreadHandler = new Handler(Looper.getMainLooper());

        @Override
        public void execute(@NonNull Runnable command) {
            mainThreadHandler.post(command);
        }

        public void execute(final Runnable command, final int delay) {
            mainThreadHandler.postDelayed(command, delay);
        }

        public void cancel(@NonNull Runnable command) {
            mainThreadHandler.removeCallbacks(command);
        }
    }
}
