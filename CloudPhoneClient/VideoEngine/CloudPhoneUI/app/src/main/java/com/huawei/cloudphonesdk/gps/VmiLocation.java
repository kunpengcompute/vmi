/*
* Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
*/

package com.huawei.cloudphonesdk.gps;

import com.huawei.cloudphonesdk.utils.LogUtil;
import java.util.List;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Gps位置辅助类
*/
public class VmiLocation {
    private static final String TAG = "VmiGPS";

    private static VmiLocation instance = new VmiLocation();

    private LocationHelper mLocationHelper;

    /** return when execution succeeds */
    public static final int VMI_SUCCESS = 0;

    /** return when receiving msg fails */
    public static final int VMI_GPS_ENGINE_CLIENT_RECV_FAIL = 0x0A040001;

    /** commond word received from server */
    protected static final int RETURN_START_GPS = 84017154;
    protected static final int RETURN_STOP_GPS = 84017155;

    private static final int SIZE_OF_INT = Integer.SIZE; // 32 bits in an Integer
    private static final int SIZE_OF_BYTE = Byte.SIZE; // 8 bits in a Byte
    private static final int INT_BYTES = SIZE_OF_INT / SIZE_OF_BYTE;; // 4 bytes in an Integer

    /**
     * Get instance of VmiLocation.
    *
    * @param Null
    * @return VmiLocation instance
    */
    public static VmiLocation getInstance() {
        return instance;
    }

    public void setLocationHelper(LocationHelper locationHelper) {
        this.mLocationHelper = locationHelper;
    } 

    /**
     * public interface to process gps data packet.
    *
    * @param data   The received gps data packet.
    * @param length The byte length of gps data packet.
    * @return VMI_SUCCESS or VMI_GPS_ENGINE_CLIENT_RECV_FAIL.
    */
    public int onRecvGpsPacket(byte[] data, int length) {
        if (data == null || length != data.length || data.length < INT_BYTES ) {
            LogUtil.error(TAG, "wrong gps msg, input length:" + length + " , data length:" + data.length);
            return VMI_GPS_ENGINE_CLIENT_RECV_FAIL;
        }

        ByteBuffer buffer = ByteBuffer.wrap(data);
        buffer.order(ByteOrder.LITTLE_ENDIAN); // ensure big-endian byte order
        int result = handleMsgType(buffer);
        return result;
    }

    private int handleMsgType(ByteBuffer buffer) {
        // read the first int to determine the message type
        int cmd = buffer.getInt();
        switch (cmd) {
            case RETURN_START_GPS:
                return handleStartGps(buffer);
            case RETURN_STOP_GPS:
                return handleStopGps(buffer);
            default:
            LogUtil.error(TAG, "Unknown cmd" + cmd + " received");
                return VMI_GPS_ENGINE_CLIENT_RECV_FAIL;
        }
    }

    private int handleStartGps(ByteBuffer buffer) {
        if (mLocationHelper == null) {
            LogUtil.error(TAG, "handle start gps failed, location helper has not been initialized");
            return VMI_GPS_ENGINE_CLIENT_RECV_FAIL;
        }
        mLocationHelper.registerLocationListener();
        return VMI_SUCCESS;
    }

    private int handleStopGps(ByteBuffer buffer) {
        if (mLocationHelper == null) {
            LogUtil.error(TAG, "handle stop gps failed, location helper has not been initialized");
            return VMI_GPS_ENGINE_CLIENT_RECV_FAIL;
        }
        mLocationHelper.unregisterLocationListener();
        return VMI_SUCCESS;
    }
}