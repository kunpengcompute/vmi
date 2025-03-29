/*
* Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
*/

package com.huawei.cloudphonesdk.gps;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import androidx.core.content.ContextCompat;

import com.huawei.cloudphonesdk.utils.LogUtil;

import java.util.List;

/**
 * Gps位置辅助类
*/
public class LocationHelper {
    private static final String TAG = "VmiGPS";

    // 位置Manager
    private LocationManager mLocationManager;

    private Handler mainHandler;

    // NMEA数据回调
    private VmiNmeaMessageListener mNmeaListener;

    // 位置发生变化回调
    private VmiLocationListener mLocationListener;
    private boolean hasPermission;

    /**
     * 构造器
    *
    * @param context Activity引用
    */
    public LocationHelper(Context context) {
        if (context.getSystemService(Context.LOCATION_SERVICE) instanceof LocationManager) {
            mLocationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        }
        mainHandler = new Handler(Looper.getMainLooper());
        hasPermission = ContextCompat.checkSelfPermission(context,
            android.Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED
            && ContextCompat.checkSelfPermission(context,
            android.Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * 注册位置监听
    */
    @SuppressLint("MissingPermission")
    public void registerLocationListener() {
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                // 确保此代码块在主线程执行
                try {
                    if (mLocationManager == null) {
                        LogUtil.error(TAG, "registerLocationListener failed: no location manager");
                        return;
                    }
                    // 判断是否有位置权限
                    if (!hasPermission) {
                        LogUtil.error(TAG, "registerLocationListener failed: no location permission");
                        return;
                    }
                    // 注册NMEA回调
                    if (mNmeaListener == null) {
                        mNmeaListener = new VmiNmeaMessageListener();
                    }
                    mLocationManager.addNmeaListener(mNmeaListener);
                    // 注册位置变化回调
                    if (mLocationListener == null) {
                        mLocationListener = new VmiLocationListener();
                    }
                    List<String> providers = mLocationManager.getProviders(true);
                    if (providers.contains(LocationManager.GPS_PROVIDER)) {
                        mLocationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,
                            1000, 0, mLocationListener);
                        LogUtil.info(TAG, "gps provider");
                    }
                    if (providers.contains(LocationManager.NETWORK_PROVIDER)) {
                        mLocationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER,
                            1000, 0, mLocationListener);
                        LogUtil.info(TAG, "network provider");
                    }
                } catch (SecurityException e) {
                    LogUtil.error(TAG, "Failed to register listener err=" + e);
                }
            }
        });
    }

    /**
     * 移除位置监听
    */
    public void unregisterLocationListener() {
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                // 确保此代码块在主线程执行
                try {
                    if (mLocationManager != null) {
                        if (mNmeaListener != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                            mLocationManager.removeNmeaListener(mNmeaListener);
                        }
                        if (mLocationListener != null) {
                            mLocationManager.removeUpdates(mLocationListener);
                        }
                    }
                } catch (SecurityException e) {
                    LogUtil.error(TAG, "Failed to unregister listener err=" + e);
                }
            }
        });
    }
}