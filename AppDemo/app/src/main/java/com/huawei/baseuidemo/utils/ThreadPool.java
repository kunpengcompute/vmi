package com.huawei.baseuidemo.utils;

import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

public class ThreadPool {
    private final ThreadPoolExecutor executor;

    /**
     * 创建带参数的线程池
     */
    public class NamedThreadFactory implements ThreadFactory {
        private final AtomicInteger count = new AtomicInteger();
        private final ThreadFactory defaultThreadFactory;
        private final String prefix;

        public NamedThreadFactory(final String baseName) {
            defaultThreadFactory = Executors.defaultThreadFactory();
            prefix = baseName + "-";
        }

        @Override
        public Thread newThread(Runnable runnable) {
            Thread thread = defaultThreadFactory.newThread(runnable);
            thread.setName(prefix + count.getAndIncrement());
            return thread;
        }
    }

    /**
     * 线程池初始化
     *
     * @param initSize 初始化线程池大小
     * @param maxSize 最大线程池大小
     */
    public ThreadPool(int initSize, int maxSize) {
        executor = new ThreadPoolExecutor(initSize, maxSize, 10, TimeUnit.SECONDS,
                new LinkedBlockingQueue<Runnable>(),
                new NamedThreadFactory("mythreadpool"));
    }

    /**
     * 创建线程池
     *
     * @param runnable
     */
    public void submit(Runnable runnable) {
        executor.execute(runnable);
    }

    /**
     * 删除线程池
     *
     * @param runnable
     */
    public void remove(Runnable runnable) {
        executor.remove(runnable);
    }
}
