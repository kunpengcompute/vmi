/*
* Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
*/

package com.huawei.cloudphonesdk.gps;

import android.location.Location;
import android.location.LocationListener;
import android.location.LocationProvider;
import android.os.Bundle;

import com.huawei.cloudphonesdk.maincontrol.OpenGLJniWrapper;
import com.huawei.cloudphonesdk.utils.LogUtil;

/**
 * GPS 位置信息变化回调
*/
public class VmiLocationListener implements LocationListener {
    private static final String TAG = "VmiGPS";

    @Override
    public void onLocationChanged(Location location) {
        OpenGLJniWrapper.sendGpsLocation(location.getLatitude(), location.getLongitude(),
            location.getAltitude(), location.getSpeed(), location.getBearing(), location.getAccuracy(), location.getTime());
        LogUtil.info(TAG, "Time: " + location.getTime() + System.lineSeparator()
            + "Latitude: " + location.getLatitude() + System.lineSeparator()
            + "Longitude: " + location.getLongitude() + System.lineSeparator()
            + "Altitude: " + location.getAltitude() + System.lineSeparator()
            + "Bearing: " + location.getBearing() + System.lineSeparator()
            + "Speed: " + location.getSpeed());
    }

    @Override
    public void onStatusChanged(String s, int status, Bundle bundle) {
        switch (status) {
            case LocationProvider.AVAILABLE: {
                LogUtil.info(TAG, "LocationProvider AVAILABLE");
                break;
            }
            case LocationProvider.OUT_OF_SERVICE: {
                LogUtil.info(TAG, "LocationProvider OUT_OF_SERVICE");
                break;
            }
            case LocationProvider.TEMPORARILY_UNAVAILABLE: {
                LogUtil.info(TAG, "LocationProvider TEMPORARILY_UNAVAILABLE");
                break;
            }
            default: {
                LogUtil.error(TAG, "error, invalid status " + status);
            }
        }
    }

    @Override
    public void onProviderEnabled(String s) {
    }

    @Override
    public void onProviderDisabled(String s) {
    }
}
