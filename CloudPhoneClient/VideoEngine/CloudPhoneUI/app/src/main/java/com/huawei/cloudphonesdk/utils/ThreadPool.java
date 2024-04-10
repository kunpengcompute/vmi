/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.utils;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;

import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * ThreadPool
 *
 * @since 2018-07-05
 */
@Keep
public class ThreadPool {
    // threadpool
    private static final int CORE_POOL_SIZE = 4;
    private static final int MAX_POOL_SIZE = 8;
    private static final int KEEP_ALIVE_TIME = 10; // 10 seconds
    private final ThreadPoolExecutor executor;

    /**
     * A thread factory that creates threads with a suffix.
     */
    public static class NamedThreadFactory implements ThreadFactory {
        private final AtomicInteger count = new AtomicInteger(0);
        private final ThreadFactory defaultThreadFactory;
        private final String prefix;

        public NamedThreadFactory(final String baseName) {
            defaultThreadFactory = Executors.defaultThreadFactory();
            prefix = baseName + "-";
        }

        @Override
        public Thread newThread(@NonNull final Runnable runnable) {
            final Thread thread = defaultThreadFactory.newThread(runnable);
            thread.setName(prefix + count.getAndIncrement());
            return thread;
        }
    }

    /**
     * create threadpool.
     *
     * @param initPoolSize poolsize
     * @param maxPoolSize  max poolsize
     */
    public ThreadPool(int initPoolSize, int maxPoolSize) {
        executor = new ThreadPoolExecutor(
            initPoolSize, maxPoolSize, KEEP_ALIVE_TIME,
            TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(),
            new NamedThreadFactory("mythreadpool"));
    }

    /**
     * create threadpool.
     */
    public ThreadPool() {
        this(CORE_POOL_SIZE, MAX_POOL_SIZE);
    }

    /**
     * create threadpool.
     *
     * @param run Runnable
     */
    public void submit(Runnable run) {
        executor.execute(run);
    }

    /**
     * destroy executor
     */
    public void destroy() {
        executor.shutdownNow();
    }
}
