/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.utils;

import android.content.Context;
import android.content.SharedPreferences;

import java.util.regex.Pattern;

/**
 * 工具类
 *
 * @since 2018-10-22
 */
public class Utils {
    /**
     * sp name
     */
    public static final String SP_NAME = "cloud_game_platform";

    private static final int MIN_CLICK_DELAY_TIME = 1000;
    public static final int MIN_PORT = 1;
    public static final int MAX_PORT = 65535;
    private static long lastClickTime;
    private static final String TAG = "CGP-Utils";

    /**
     * save value into shared preference.
     *
     * @param context context
     * @param preferenceName preferenceName
     * @param key key
     * @param object object
     *
     */
    public static void saveKeyValue(Context context, String preferenceName, String key, Object object) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(preferenceName, context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        if (object instanceof String) {
            editor.putString(key, (String) object);
        } else if (object instanceof Integer) {
            editor.putInt(key, (Integer) object);
        } else if (object instanceof Boolean) {
            editor.putBoolean(key, (Boolean) object);
        } else if (object instanceof Float) {
            editor.putFloat(key, (Float) object);
        } else if (object instanceof Long) {
            editor.putLong(key, (Long) object);
        } else {
            editor.putString(key, object.toString());
        }
        editor.commit();
    }

    /**
     * get value from shared preference.
     *
     * @param context context
     * @param preferenceName preferenceName
     * @param key key
     * @param object object
     * @return Object Object
     */
    public static Object getValueByKey(Context context, String preferenceName, String key, Object object) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(preferenceName, context.MODE_PRIVATE);
        if (object instanceof String) {
            return sharedPreferences.getString(key, (String) object);
        } else if (object instanceof Integer) {
            return sharedPreferences.getInt(key, (Integer) object);
        } else if (object instanceof Boolean) {
            return sharedPreferences.getBoolean(key, (Boolean) object);
        } else if (object instanceof Float) {
            return sharedPreferences.getFloat(key, (Float) object);
        } else if (object instanceof Long) {
            return sharedPreferences.getLong(key, (Long) object);
        } else {
            return sharedPreferences.getString(key, object.toString());
        }
    }

    /**
     * test if user is clicking too fast.
     *
     * @return true if use is clicking too fast
     */
    public static boolean isFastClick() {
        boolean flag = false;
        long curClickTime = System.currentTimeMillis();
        if ((curClickTime - lastClickTime) >= MIN_CLICK_DELAY_TIME) {
            flag = true;
        }
        lastClickTime = curClickTime;
        return flag;
    }

    /**
     * 校验IP是否合法.
     *
     * @param text String, ip 地址
     * @return boolean
     */
    public static boolean checkIp(String text) {
        if (text != null && !text.isEmpty()) {
            String regex = "^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$";
            if (text.matches(regex)) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    /**
     * 检验ip是否合法.
     *
     * @param port 端口字符串
     * @return boolean,true 合法,false 不合法.
     */
    public static boolean checkPort(String port) {
        Pattern pattern = Pattern.compile("[1-9][0-9]{0,5}");
        if (pattern.matcher(port).matches()) {
            int num = Integer.parseInt(port);
            LogUtils.info(TAG, "num:" + num);
            if (num >= MIN_PORT && num <= MAX_PORT) {
                return true;
            }
        }
        return false;
    }
}