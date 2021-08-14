/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.text.TextUtils;
import com.google.gson.Gson;
import com.huawei.instructionstream.appui.CGPApplication;

/**
 * SP工具
 *
 * @since 2020-04-02
 */
public class SPUtil {
    /**
     * 指令流设置
     */
    public static final String INS_SETTING = "instruction_settings";

    /**
     * 文件名
     */
    private static final String FILE_NAME = "instruction";

    private static SharedPreferences getSharedPreferences() {
        return CGPApplication.instance.getSharedPreferences(FILE_NAME, Context.MODE_PRIVATE);
    }

    /**
     * 获取boolean类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static boolean getBoolean(String key, boolean defaultValue) {
        return getSharedPreferences().getBoolean(key, defaultValue);
    }

    /**
     * 获取int类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static int getInt(String key, int defaultValue) {
        return getSharedPreferences().getInt(key, defaultValue);
    }

    /**
     * 获取long类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static long getLong(String key, long defaultValue) {
        return getSharedPreferences().getLong(key, defaultValue);
    }

    /**
     * 获取object类型字段
     *
     * @param key          Key
     * @param clazz        Object class
     * @param defaultValue default value if not found
     * @return Value
     */
    public static <T> T getObject(String key, Class<T> clazz, T defaultValue) {
        String value = getString(key, null);
        if (!TextUtils.isEmpty(value)) {
            return new Gson().fromJson(value, clazz);
        }
        return defaultValue;
    }

    /**
     * 获取String类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static String getString(String key, String defaultValue) {
        return getSharedPreferences().getString(key, defaultValue);
    }

    /**
     * 保存boolean类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putBoolean(String key, boolean value) {
        SharedPreferences.Editor localEditor = getSharedPreferences().edit();
        localEditor.putBoolean(key, value);
        localEditor.apply();
    }

    /**
     * 保存int类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putInt(String key, int value) {
        SharedPreferences.Editor localEditor = getSharedPreferences().edit();
        localEditor.putInt(key, value);
        localEditor.apply();
    }

    /**
     * 保存long类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putLong(String key, long value) {
        SharedPreferences.Editor localEditor = getSharedPreferences().edit();
        localEditor.putLong(key, value);
        localEditor.apply();
    }

    /**
     * 保存object类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static <T> void putObject(String key, T value) {
        putString(key, new Gson().toJson(value));
    }

    /**
     * 保存String类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putString(String key, String value) {
        SharedPreferences.Editor localEditor = getSharedPreferences().edit();
        localEditor.putString(key, value);
        localEditor.apply();
    }

    /**
     * 移除Key
     *
     * @param key Key
     */
    public static void removeKey(String key) {
        SharedPreferences.Editor localEditor = getSharedPreferences().edit();
        localEditor.remove(key);
        localEditor.apply();
    }
}