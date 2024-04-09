/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.utils;

import android.os.Environment;
import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;

/**
 * Log工具类
 *
 * @author zhh
 * @since 2020-07-25
 */
public class LogUtil {
    public static final String PATH = Environment.getExternalStorageDirectory() + "/log/";

    public static final String CLIENT_LOG = "client.log";

    public static final String EXIT_INFO = "=============exit cloud phone===============";

    public static final String ENTER_INFO = "============enter cloud phone==============";

    private static final String TAG = "LogUtil";

    private static final String[] running = new String[]{"logcat"};

    private static final String[] clear = new String[]{"logcat", "-c", "-b", "main", "-b", "events", "-b", "radio", "-b", "system"};

    private static Process exec;

    private static final ThreadPool threadPool = new ThreadPool(1, 20);

    /**
     * Log.i的输出为绿色，一般提示性的消息
     *
     * @param tag     TAG
     * @param message MESSAGE
     */
    public static void info(String tag, String message) {
        Log.i(tag, message);
    }

    /**
     * Log.d的输出颜色为蓝色，仅输出debug调试
     *
     * @param tag     TAG
     * @param message MESSAGE
     */
    public static void debug(String tag, String message) {
        Log.d(tag, message);
    }

    /**
     * Log.e的输出颜色为为红色，仅显示红色的错误信息
     *
     * @param tag     TAG
     * @param message MESSAGE
     */
    public static void error(String tag, String message) {
        Log.e(tag, message);
    }

    /**
     * Log.w的输出颜色为橙色，可以看作为warning警告
     *
     * @param tag     TAG
     * @param message MESSAGE
     */
    public static void warn(String tag, String message) {
        Log.w(tag, message);
    }

    /**
     * Log.v的调试颜色为黑色，任何消息都会输出
     *
     * @param tag     TAG
     * @param message MESSAGE
     */
    public static void verbose(String tag, String message) {
        Log.v(tag, message);
    }

    /**
     * startLogs
     *
     * @param filename filename
     * @throws IOException IOException
     */
    public static void startLogs(final String filename) {
        stopLog();
        try {
            Runtime.getRuntime().exec(clear);
            createFile(filename);
            exec = Runtime.getRuntime().exec(running);
        } catch (IOException e) {
            LogUtil.error(TAG, e.getMessage());
        }
        threadPool.submit(new Runnable() {
            @Override
            public void run() {
                final InputStream is = exec.getInputStream();
                FileOutputStream os = null;
                try {
                    // 新建一个路径信息
                    os = new FileOutputStream(PATH + filename, true);
                    int len = 0;
                    byte[] buf = new byte[1024];
                    len = is.read(buf);
                    while (len != -1) {
                        os.write(buf, 0, len);
                        os.flush();
                        len = is.read(buf);
                    }
                } catch (FileNotFoundException e) {
                    debug(TAG, "read logcat process failed. message: " + e.getMessage());
                } catch (IOException e) {
                    debug(TAG, "read logcat process failed. message: " + e.getMessage());
                } finally {
                    if (os != null) {
                        try {
                            os.close();
                        } catch (IOException e) {
                            // Do nothing
                            error(TAG, e.getMessage());
                        }
                    }
                    if (is != null) {
                        try {
                            is.close();
                        } catch (IOException e) {
                            // Do nothing
                            error(TAG, e.getMessage());
                        }
                    }
                }
            }
        });
    }

    private static void createFile(String filename) {
        File logDir = new File(PATH);
        if (!logDir.exists()) {
            boolean mkdirs = logDir.mkdirs();
            LogUtil.debug(TAG, "创建日志文件夹：" + mkdirs);
        }
        File logFile = new File(PATH + filename);
        if (!logFile.exists()) {
            try {
                boolean newFile = logFile.createNewFile();
                LogUtil.debug(TAG, "创建日志文件：" + newFile);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * stopLog
     */
    public static void stopLog() {
        if (exec != null) {
            exec.destroy();
        }
    }

    /**
     * 在日志文件末尾写入退出云手机标记
     *
     * @param fileDirName 日志文件所在目录
     * @param fileName    日志文件名
     */
    public static void writeLog(String fileDirName, String fileName, String info) {
        BufferedWriter bw = null;
        info = info + System.getProperty("line.separator");
        try {
            bw = new BufferedWriter(new OutputStreamWriter(
                    new FileOutputStream(fileDirName + fileName, true)));
            bw.write(info);
        } catch (Exception e) {
            error(TAG, e.getMessage());
        } finally {
            if (bw != null) {
                try {
                    bw.close();
                } catch (IOException e) {
                    error(TAG, e.getMessage());
                }
            }
        }
    }
}
