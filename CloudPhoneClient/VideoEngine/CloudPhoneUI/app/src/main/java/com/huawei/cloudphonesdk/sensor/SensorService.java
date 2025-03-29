/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

package com.huawei.cloudphonesdk.sensor;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.util.Log;
import androidx.annotation.Keep;

import java.util.List;
import java.util.Collections;

/**
 * 传感器服务类
*
* @since 2023-10-26
*/
@Keep
public class SensorService {
    private static final String TAG = "SensorService";

    // 传感器Manager
    private SensorManager mSensorManager;

    // 传感器数据回调
    private VmiSensor mSensorListener;

    private Sensor mAccelerometer;

    private Sensor mSensor;

    /**
     * 构造器
    *
    * @param context Activity引用
    */
    public SensorService(Context context) {
        if (context.getSystemService(Context.SENSOR_SERVICE) instanceof SensorManager) {
            mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        }
    }

    /**
     * 获取可激活的传感器列表
    */
    public List<Sensor> getSensorList() {
        if (mSensorManager == null) {
            Log.e(TAG, "getSensorList failed: no sensor manager");
            return Collections.emptyList(); // Return an empty list if the sensor manager is not available
        }
    
        return mSensorManager.getSensorList(Sensor.TYPE_ALL); // Return the list of sensors
    }

    /**
     * 移除所有传感器监听
    */
    public void unregisterSensorListener() {
        if (mSensorManager != null && mSensorListener != null) {
            mSensorManager.unregisterListener(mSensorListener);
            mSensorListener.resetSensorsArray();
        }
    }

    /**
     * 基于传入的传感器类型和采样率注册监听
    */
    public void registerActivatedSensorListener(int sensorType, long samplingPeriod) {
        if (mSensorManager == null) {
            Log.e(TAG, "registerActivatedSensorListener failed: no sensor manager");
            return;
        }
        if (mSensorListener == null) {
            mSensorListener = VmiSensor.getInstance();
        }
        mSensor = mSensorManager.getDefaultSensor(sensorType);
        if (mSensor == null) {
            Log.e(TAG, "Sensor with type " + sensorType + " not found");
            return;
        }
        // 转换纳秒为微秒并确保值适合 int 类型
        int samplingPeriodUs = (int) Math.min(samplingPeriod / 1000, Integer.MAX_VALUE);
        mSensorManager.registerListener(mSensorListener, mSensor, samplingPeriodUs);
    }

    /**
     * 基于传入的传感器类型移除监听
    */
    public void unregisterDeactivateSensorListener(int sensorType) {
        if (mSensorManager != null && mSensorListener != null) {
            mSensor = mSensorManager.getDefaultSensor(sensorType);
            if (mSensor == null) {
                Log.e(TAG, "Sensor with type " + sensorType + " not found");
                return;
            }
            mSensorManager.unregisterListener(mSensorListener, mSensor);
        }
    }
}