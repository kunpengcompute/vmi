/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudgame.touch;

/**
 * TouchEvent.
 *
 * @since 2018-07-05
 */
public class TouchEvent {
    private static final int EVENT_DATA_LENGTH = 17;
    // cmd
    private static final int SEND_TOUCH_CMD = 0x2020000;
    // client status
    private int orientation = -1;
    private int width = 0;
    private int height = 0;
    // touch event
    private int id = 0;
    private int action = 0;
    private int cloudX = 0;
    private int cloudY = 0;
    private int pressure = 0;

    public void setOrientation(int orientation) {
        this.orientation = orientation;
    }

    public int getOrientation() {
        return orientation;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getWidth() {
        return width;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public int getHeight() {
        return this.height;
    }

    public void setId(int touchEventId) {
        id = touchEventId;
    }

    public int getId() {
        return id;
    }

    public void setAction(int touchEventAction) {
        action = touchEventAction;
    }

    public int getAction() {
        return action;
    }

    public void setCloudX(int touchEventCloudX) {
        cloudX = touchEventCloudX;
    }

    public int getCloudX() {
        return cloudX;
    }

    public void setCloudY(int touchEventCloudY) {
        cloudY = touchEventCloudY;
    }

    public int getCloudY() {
        return cloudY;
    }

    public void setPressure(int touchEventPressure) {
        pressure = touchEventPressure;
    }

    public int getPressure() {
        return pressure;
    }

    /**
     * construct ByteArray of TouchEvent data.
     *
     * @return result
     */
    public byte[] toByteArray() {
        byte[] result = new byte[EVENT_DATA_LENGTH];
        // cmd
        result[0] = (byte) SEND_TOUCH_CMD;
        result[1] = (byte) (SEND_TOUCH_CMD >> 8);
        result[2] = (byte) (SEND_TOUCH_CMD >> 16);
        result[3] = (byte) (SEND_TOUCH_CMD >> 24);
        // client status
        result[4] = (byte) orientation;
        result[5] = (byte) width;
        result[6] = (byte) (width >> 8);
        result[7] = (byte) height;
        result[8] = (byte) (height >> 8);
        // touch event
        result[9] = (byte) id;
        result[10] = (byte) action;
        result[11] = (byte) cloudX;
        result[12] = (byte) (cloudX >> 8);
        result[13] = (byte) cloudY;
        result[14] = (byte) (cloudY >> 8);
        result[15] = (byte) pressure;
        result[16] = (byte) (pressure >> 8);

        return result;
    }
}
