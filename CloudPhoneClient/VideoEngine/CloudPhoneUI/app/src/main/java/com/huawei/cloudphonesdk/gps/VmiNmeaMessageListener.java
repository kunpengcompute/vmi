/*
* Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
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
