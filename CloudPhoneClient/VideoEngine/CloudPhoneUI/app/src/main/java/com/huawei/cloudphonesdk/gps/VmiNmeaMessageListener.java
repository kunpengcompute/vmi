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

import android.location.OnNmeaMessageListener;
import android.os.Build;

import com.huawei.cloudphonesdk.maincontrol.OpenGLJniWrapper;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.io.UnsupportedEncodingException;

/**
 * GPS NMEA数据回调
*/
public class VmiNmeaMessageListener implements OnNmeaMessageListener {
    private static final String TAG = "VmiGPS";
    @Override
    public void onNmeaMessage(String nmea, long l) {
        try {
            // 移除可能存在的换行符
            LogUtil.info(TAG, "onNmeaMessage nmea: " + nmea);
            String trimmedNmea = nmea.trim();
            LogUtil.info(TAG, "onNmeaMessage trimmedNmea: " + trimmedNmea);
            String newNmea = trimmedNmea + "\n";
            LogUtil.info(TAG, "onNmeaMessage newNmea: " + newNmea);
            byte[] bytes = newNmea.getBytes("UTF-8");
            OpenGLJniWrapper.sendGpsNmea(bytes, bytes.length);
        } catch (NullPointerException | ArrayIndexOutOfBoundsException | UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }
}
