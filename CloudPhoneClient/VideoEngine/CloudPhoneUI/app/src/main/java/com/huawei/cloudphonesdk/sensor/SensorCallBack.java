/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

package com.huawei.cloudphonesdk.sensor;

import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;
import com.huawei.cloudphonesdk.utils.LogUtil;

/**
 * SensorCallBack.
*
* @since 2023-11-01
*/
public class SensorCallBack implements NewPacketCallback {

    private static final String TAG = "SensorCallBack";

    private int cnt = 0;

    private long now = 0L;

    private long last = 0L;

    private final long LOG_INTERVAL = 1000L;

    public static VmiSensor vmiSensor = null;

    /**
     * SensorCallBack.
    */
    public SensorCallBack() {
        vmiSensor = VmiSensor.getInstance();
    }

    @Override
    public void onNewPacket(byte[] data) {
        int ret = vmiSensor.onRecvSensorPacket(data, data.length);
        if (ret == vmiSensor.VMI_SUCCESS && (++cnt % 200 == 0)) {
            LogUtil.info(TAG, "Suceeded in receiving Sensor data packets.");
        }

        if (ret == vmiSensor.VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL) {
            now = System.currentTimeMillis();
            if ((last == 0) || (now - last >= LOG_INTERVAL)) {
                LogUtil.error(TAG, "Failed to receive Sensor data packets.");
                last = now;
            }
        }
    }
}