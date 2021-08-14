/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui;

import android.app.Application;

import com.huawei.instructionstream.appui.utils.LogUtils;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * CGPApplication.
 *
 * @since 2018-07-05
 */
public class CGPApplication extends Application {
    /**
     * application context.
     */
    public static CGPApplication instance;
    private static final String TAG = "CGPApplication";

    /**
     * getUser.
     *
     * @return user info
     */

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
        LogUtils.info(TAG, "onCreate()");
        closeAndroidPDialog();
    }

    /**
     * 消除Android P对于使用非SDK接口的检查弹框.
     */
    private void closeAndroidPDialog() {
        try {
            Class reflectedClass = Class.forName("android.content.pm.PackageParser$Package");
            Constructor declaredConstructor = reflectedClass.getDeclaredConstructor(String.class);
            declaredConstructor.setAccessible(true);
        } catch (ClassNotFoundException e) {
            LogUtils.error(TAG, "ClassNotFoundException:android.content.pm.PackageParser$Package.");
        } catch (NoSuchMethodException e) {
            LogUtils.error(TAG, "Cann't find constructor of android.content.pm.PackageParser$Package.");
        }
        try {
            Class cls = Class.forName("android.app.ActivityThread");
            Method declaredMethod = cls.getDeclaredMethod("currentActivityThread");
            declaredMethod.setAccessible(true);
            Object activityThread = declaredMethod.invoke(null);
            Field hiddenApiWarningShown = cls.getDeclaredField("mHiddenApiWarningShown");
            hiddenApiWarningShown.setAccessible(true);
            hiddenApiWarningShown.setBoolean(activityThread, true);
        } catch (InvocationTargetException e) {
            LogUtils.error(TAG, "closeAndroidPDialog with InvocationTargetException.");
        } catch (NoSuchMethodException e) {
            LogUtils.error(TAG, "closeAndroidPDialog with NoSuchMethodException.");
        } catch (IllegalAccessException e) {
            LogUtils.error(TAG, "closeAndroidPDialog with IllegalAccessException.");
        } catch (NoSuchFieldException e) {
            LogUtils.error(TAG, "closeAndroidPDialog with NoSuchFieldException.");
        } catch (ClassNotFoundException e) {
            LogUtils.error(TAG, "closeAndroidPDialog with ClassNotFoundException.");
        }
    }
}
