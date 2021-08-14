/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.utils;

import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * LogUtils
 *
 * @since 2018-07-05
 */
public class LogUtils {
    private static final String PATH = Environment.getExternalStorageDirectory() + "/log/";

    private static String[] running = new String[] {"logcat", "-s", "adb logcat *:V"};

    private static Process exec;

    private static ThreadPool threadPool = new ThreadPool(1, 20);

    /**
     * Log.i的输出为绿色，一般提示性的消息
     *
     * @param tag TAG
     * @param message MESSAGE
     */
    public static void info(String tag, String message) {
        Log.i(tag, message);
    }

    /**
     * Log.d的输出颜色为蓝色，仅输出debug调试
     *
     * @param tag TAG
     * @param message MESSAGE
     */
    public static void debug(String tag, String message) {
        Log.d(tag, message);
    }

    /**
     * Log.e的输出颜色为为红色，仅显示红色的错误信息
     *
     * @param tag TAG
     * @param message MESSAGE
     */
    public static void error(String tag, String message) {
        Log.e(tag, message);
    }

    /**
     * Log.w的输出颜色为橙色，可以看作为warning警告
     *
     * @param tag TAG
     * @param message MESSAGE
     */
    public static void warn(String tag, String message) {
        Log.w(tag, message);
    }

    /**
     * Log.v的调试颜色为黑色，任何消息都会输出
     *
     * @param tag TAG
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
    public static void startLogs(final String filename) throws IOException {
        exec = Runtime.getRuntime().exec(running);
        final InputStream is = exec.getInputStream();
        threadPool.submit(new Runnable() {
            @Override
            public void run() {
                File file1 = new File(PATH);
                if (!file1.exists()) {
                    file1.mkdirs();
                }
                File file = new File(PATH + filename);
                if (file.exists() && file.isFile()) {
                    file.delete();
                }
                FileOutputStream os = null;
                try {
                    // 新建一个路径信息
                    os = new FileOutputStream(PATH + filename);
                    int len = 0;
                    byte[] buf = new byte[1024];
                    len = is.read(buf);
                    while (len != -1) {
                        os.write(buf, 0, len);
                        os.flush();
                        len = is.read(buf);
                    }
                } catch (FileNotFoundException e) {
                    debug("writelog", "read logcat process failed. message: " + e.getMessage());
                } catch (IOException e) {
                    debug("writelog", "read logcat process failed. message: " + e.getMessage());
                } finally {
                    if (os != null) {
                        try {
                            os.close();
                        } catch (IOException e) {
                            // Do nothing
                            error("writelog", e.getMessage());
                        }
                    }
                }
            }
        });
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
     * getSystemLogLevel
     *
     * @return int int
     */
    public static int getSystemLogLevel() {
        try {
            Class systemPropertiesClass = Class.forName("android.os.SystemProperties");
            Method getIntMethod = systemPropertiesClass.getDeclaredMethod("getInt", String.class, int.class);
            getIntMethod.setAccessible(true);
            int inputLogLevel = (int) getIntMethod.invoke(systemPropertiesClass, "sys.cloud.game.log_level", 0);
            info("writelog", "input log level:" + inputLogLevel);
            if (inputLogLevel <= 0 || inputLogLevel > android.util.Log.ASSERT) {
                return android.util.Log.INFO;
            } else {
                return inputLogLevel;
            }
        } catch (InvocationTargetException | IllegalAccessException | NoSuchMethodException
            | ClassNotFoundException e) {
            error("writelog", e.getMessage());
            return android.util.Log.INFO;
        }
    }
}