/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone;

import android.annotation.SuppressLint;
import android.app.Application;

import com.huawei.cloudphone.model.UserBean;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * MyApplication
 *
 * @author zhh
 * @since 2019-09-24
 */
public class MyApplication extends Application {
    private static final String TAG = MyApplication.class.getSimpleName();

    /**
     * ApplicationContext
     */
    public static MyApplication instance;

    // user info
    private UserBean mUserBean;

    // 配置服IP
    private String mBaseUrl;

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
        disableApiDialog();
    }

    /**
     * get user info from cache or local
     *
     * @return user info
     */
    public UserBean getUserBean() {
        if (mUserBean == null) {
            mUserBean = SPUtil.getObject(SPUtil.KEY_USER_INFO, UserBean.class, new UserBean());
        }
        return mUserBean;
    }

    /**
     * save user info to cache
     *
     * @param userBean user info
     */
    public void setUserBean(UserBean userBean) {
        mUserBean = userBean;
    }

    /**
     * get request base url
     *
     * @return base url
     */
    public String getBaseUrl() {
        if (mBaseUrl == null) {
            mBaseUrl = SPUtil.getString(SPUtil.KEY_BASE_URL, "");
        }
        return "http://" + mBaseUrl + ":8080";
    }

    /**
     * set request base url
     *
     * @param baseUrl request base url
     */
    public void setBaseUrl(String baseUrl) {
        mBaseUrl = baseUrl;
        SPUtil.putString(SPUtil.KEY_BASE_URL, baseUrl);
    }

    /**
     * 消除Android P对于使用非SDK接口的检查弹框.
     */
    @SuppressLint("PrivateApi")
    private void disableApiDialog() {
        try {
            Class reflectedClass = Class.forName("android.content.pm.PackageParser$Package");
            Constructor declaredConstructor = reflectedClass.getDeclaredConstructor(String.class);
            declaredConstructor.setAccessible(true);
        } catch (ClassNotFoundException | NoSuchMethodException e) {
            LogUtil.error(TAG, "e1->" + e.getLocalizedMessage());
        }
        try {
            Class cls = Class.forName("android.app.ActivityThread");
            Method declaredMethod = cls.getDeclaredMethod("currentActivityThread");
            declaredMethod.setAccessible(true);
            Object activityThread = declaredMethod.invoke(null);
            Field hiddenApiWarningShown = cls.getDeclaredField("mHiddenApiWarningShown");
            hiddenApiWarningShown.setAccessible(true);
            hiddenApiWarningShown.setBoolean(activityThread, true);
        } catch (ClassNotFoundException | NoSuchMethodException | InvocationTargetException
            | IllegalAccessException | NoSuchFieldException e) {
            LogUtil.error(TAG, "e2->" + e.getLocalizedMessage());
        }
    }
}
