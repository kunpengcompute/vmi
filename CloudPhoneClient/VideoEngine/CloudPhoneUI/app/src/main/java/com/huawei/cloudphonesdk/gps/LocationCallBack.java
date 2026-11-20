/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
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