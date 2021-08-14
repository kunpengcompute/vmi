/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.instructionstream.appui.engine.utils;

import android.os.Environment;
import android.util.Log;

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
            if (inputLogLevel <= 0 || inputLogLevel > Log.ASSERT) {
                return Log.INFO;
            } else {
                return inputLogLevel;
            }
        } catch (InvocationTargetException | IllegalAccessException | NoSuchMethodException
            | ClassNotFoundException e) {
            error("writelog", e.getMessage());
            return Log.INFO;
        }
    }
}
