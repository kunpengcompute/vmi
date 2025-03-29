/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

package com.huawei.cloudphonesdk.gps;

import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;
import com.huawei.cloudphonesdk.utils.LogUtil;

/**
 * LocationCallBack.
*
* @since 2024-04-29
*/
public class LocationCallBack implements NewPacketCallback {

    private static final String TAG = "VmiGPS";

    private int cnt = 0;

    private long now = 0L;

    private long last = 0L;

    private final long LOG_INTERVAL = 1000L;

    public static VmiLocation vmiLocation = null;

    /**
     * LocationCallBack.
    */
    public LocationCallBack() {
        vmiLocation = VmiLocation.getInstance();
    }

    @Override
    public void onNewPacket(byte[] data) {
        int ret = vmiLocation.onRecvGpsPacket(data, data.length);
        if (ret == vmiLocation.VMI_SUCCESS && (++cnt % 200 == 0)) {
            LogUtil.info(TAG, "Suceeded in receiving gps data packets.");
        }

        if (ret == vmiLocation.VMI_GPS_ENGINE_CLIENT_RECV_FAIL) {
            now = System.currentTimeMillis();
            if ((last == 0) || (now - last >= LOG_INTERVAL)) {
                LogUtil.error(TAG, "Failed to receive gps data packets.");
                last = now;
            }
        }
    }
}