/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.download;

import android.annotation.SuppressLint;
import android.app.DownloadManager;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import androidx.annotation.NonNull;
import android.util.LongSparseArray;

import com.huawei.cloudphone.MyApplication;

/**
 * 文件下载辅助类
 *
 * @author zWX809008
 * @since 2020/5/11
 */
public class DownloadHelper {
    private static final String TAG = DownloadHelper.class.getSimpleName();
    private static DownloadHelper instance;

    /**
     * 检测下载进度的间隔
     */
    private static final long DELAY = 200;

    // 下载器
    private DownloadManager mDownloadManager;

    // 下载任务队列
    private LongSparseArray<DownloadTask> mDownloadTasks = new LongSparseArray<>();

    private DownloadHelper() {
    }

    /**
     * 构造单例类
     *
     * @return DownloadHelper
     */
    public static DownloadHelper getInstance() {
        if (instance == null) {
            synchronized (DownloadHelper.class) {
                if (instance == null) {
                    instance = new DownloadHelper();
                }
            }
        }
        return instance;
    }

    @SuppressLint("HandlerLeak")
    private Handler mDownloadHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            if (msg.obj instanceof Long) {
                Long downloadId = (Long) msg.obj;
                DownloadTask task = mDownloadTasks.get(downloadId);
                if (task != null) {
                    queryDownloadStatus(task);
                }
            }
        }
    };

    /**
     * 添加下载任务
     *
     * @param url      下载路径
     * @param fileName 保存的文件名称
     * @param callback 下载进度回调
     */
    public void addDownloadTask(String url, String fileName, DownloadListener callback) {
        // 获取DownloadManager
        if (mDownloadManager == null) {
            Object obj = MyApplication.instance.getSystemService(Context.DOWNLOAD_SERVICE);
            if (obj instanceof DownloadManager) {
                mDownloadManager = (DownloadManager) obj;
            }
            if (mDownloadManager == null) {
                throw new IllegalStateException("get download service failed !");
            }
        }
        // 创建下载任务
        DownloadManager.Request request = new DownloadManager.Request(Uri.parse(url));
        request.setTitle(fileName);
        request.setDescription("新版本APK下载中...");
        // 设置生成的文件路径
        request.setDestinationInExternalFilesDir(MyApplication.instance, Environment.DIRECTORY_DOWNLOADS, fileName);
        // 将下载请求加入下载队列，加入下载队列后会给该任务返回一个long型的id，通过该id可以取消任务，重启任务、获取下载的文件等等
        long downloadId = mDownloadManager.enqueue(request);
        DownloadTask downloadTask = new DownloadTask(downloadId, callback);
        mDownloadTasks.put(downloadId, downloadTask);
        // 开启状态检测任务
        mDownloadHandler.sendMessageDelayed(buildMessage(downloadId), DELAY);
    }

    /**
     * 查询下载任务的状态
     *
     * @param downloadTask 下载任务
     */
    private void queryDownloadStatus(DownloadTask downloadTask) {
        // 通过下载id查找任务状态
        long downloadId = downloadTask.mDownloadId;
        DownloadManager.Query query = new DownloadManager.Query()
            .setFilterById(downloadId);
        Cursor cursor = mDownloadManager.query(query);
        if (cursor != null) {
            if (cursor.moveToFirst()) {
                // 已下载size
                long soFarSize = cursor.getLong(
                    cursor.getColumnIndex(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR));
                // 总size
                long totalSize = cursor.getLong(
                    cursor.getColumnIndex(DownloadManager.COLUMN_TOTAL_SIZE_BYTES));
                float progress = totalSize < 0 ? 0 : (soFarSize * 1.0f / totalSize);
                downloadTask.mCallback.onProgress(
                    mDownloadManager.getUriForDownloadedFile(downloadId), progress);
                // 未下载完成，继续检测下载进度
                if (progress < 1) {
                    mDownloadHandler.sendMessageDelayed(buildMessage(downloadId), DELAY);
                } else {
                    removeDownloadTask(downloadId);
                }
            }
            cursor.close();
        }
    }

    /**
     * 下载完成，移除下载任务
     *
     * @param downloadTaskId 下载任务ID
     */
    private void removeDownloadTask(long downloadTaskId) {
        mDownloadTasks.remove(downloadTaskId);
        if (mDownloadTasks.size() == 0) {
            mDownloadHandler.removeCallbacksAndMessages(null);
        }
    }

    private Message buildMessage(long downloadId) {
        Message message = new Message();
        message.obj = downloadId;
        return message;
    }

    /**
     * 下载任务
     */
    class DownloadTask {
        long mDownloadId;
        DownloadListener mCallback;

        DownloadTask(long downloadId, DownloadListener callback) {
            mDownloadId = downloadId;
            mCallback = callback;
        }
    }
}
