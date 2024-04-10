/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.helper;

import android.os.Environment;

import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.model.Response;
import com.huawei.cloudphone.net.MyObserver;
import com.huawei.cloudphone.net.RetrofitHelper;
import com.huawei.cloudphone.ui.activities.BaseActivity;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphonesdk.utils.LogUtil;
import com.huawei.cloudphonesdk.utils.ThreadPool;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Calendar;
import java.util.Locale;

import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;

/**
 * 日志上传
 *
 * @author gj
 * @since 2020/8/07
 */
public class UploadLogHelper {
    private static final String TAG = UploadLogHelper.class.getSimpleName();

    // 日志文件名称
    private String filename = "cloud_phone_client.log";

    // 抓取日志执行的命令
    private String[] running = new String[]{"logcat", "-s", "adb logcat *:V"};

    private Process process;

    private ThreadPool mThreadPool = new ThreadPool(1, 20);

    // 日志文件路径
    private String PATH;

    private BaseActivity mActivity;
    private long mStartTime;
    private long mStopTime;

    /**
     * 构造方法
     *
     * @param activity BaseActivity
     */
    public UploadLogHelper(BaseActivity activity) {
        mActivity = activity;
        PATH = mActivity.getExternalFilesDir(Environment.DIRECTORY_DOCUMENTS).getAbsolutePath();
    }

    /**
     * 开始抓取日志
     */
    public void startLogs() {
        mStartTime = Calendar.getInstance(Locale.ENGLISH).getTime().getTime();
        // 判断日志文件是否存在，存在清空里面内容，不存在，创建
        createSDCardDir();
        mThreadPool.submit(new Runnable() {
            @Override
            public void run() {
                InputStream inputStream = null;
                FileOutputStream outputStream = null;
                try {
                    process = Runtime.getRuntime().exec(running);
                    inputStream = process.getInputStream();
                    // 新建一个路径信息
                    outputStream = new FileOutputStream(PATH + File.separator + filename);
                    int len;
                    byte[] buf = new byte[1024];
                    len = inputStream.read(buf);
                    while (len != -1) {
                        outputStream.write(buf, 0, len);
                        outputStream.flush();
                        len = inputStream.read(buf);
                    }
                } catch (FileNotFoundException e) {
                    LogUtil.error(TAG, "file is not exist. message: " + e.getMessage());
                } catch (IOException e) {
                    LogUtil.error(TAG, "get logcat failed. message: " + e.getMessage());
                } finally {
                    try {
                        if (outputStream != null) {
                            outputStream.close();
                        }
                        if (inputStream != null) {
                            inputStream.close();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }

    /**
     * 停止抓取日志
     */
    public void stopLog() {
        if (process != null) {
            process.destroy();
        }
        mStopTime = Calendar.getInstance(Locale.ENGLISH).getTime().getTime();
    }

    /**
     * 创建日志文件
     */
    private void createSDCardDir() {
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
            // 得到一个路径，内容是sdcard的文件夹路径和名字
            String filePath = PATH + File.separator + filename;
            File logFile = new File(filePath);
            if (logFile.exists() && logFile.isFile()) {
                // 如果文件已存在，则先删除
                logFile.delete();
            }
            // 创建新log文件
            try {
                logFile.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
