package com.huawei.cloudphone;

import android.app.Application;

public class MyApplication extends Application {
    private static final String TAG = MyApplication.class.getSimpleName();

    public static MyApplication instance;

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
    }
}
