/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

package com.huawei.cloudphonesdk.sensor;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.List;

/**
 * SensorsEvent.
*
* @since 2023-10-26
*/
public class SensorsEvent {
    // The size of sensorsevent data.
    private static final int EVENT_DATA_LENGTH = 16;

    private static final int SENSOR_SEND_SENSOR_DATA = 67239936;

    private int type = 0;

    private float x = 0;

    private float y = 0;

    private float z = 0;

    public void setType(int sensorsEventType) {
        type = sensorsEventType;
    }

    public int getType() {
        return type;
    }

    public void setX(float sensorsEventX) {
        x = sensorsEventX;
    }

    public float getX() {
        return x;
    }

    public void setY(float sensorsEventY) {
        y = sensorsEventY;
    }

    public float getY() {
        return y;
    }

    public void setZ(float sensorsEventZ) {
        z = sensorsEventZ;
    }

    public float getZ() {
        return z;
    }

    /**
     * construct ByteArray of SensorsEvent data.
    *
    * @return result
    */
    public byte[] toByteArray() {
        // 创建一个ByteBuffer来存储数据, 每个浮点数占4个字节，int占4个字节
        // 并且指定使用小端字节序
        ByteBuffer buffer = ByteBuffer.allocate(EVENT_DATA_LENGTH).order(ByteOrder.LITTLE_ENDIAN);
    
        // 将x、y、z值以小端字节序写入ByteBuffer
        buffer.putFloat(x);
        buffer.putFloat(y);
        buffer.putFloat(z);
    
        // 将type值以小端字节序写入ByteBuffer
        buffer.putInt(type);
    
        // 获取包含打包数据的字节数组
        return buffer.array();
    }

    /**
     * 打包多个 SensorsEvent 实例到一个字节缓冲区中以便发送。
    * 假设每个事件的数据长度是固定的。
    *
    * @param events SensorsEvent 列表
    * @return 打包好的字节数组
    */
    public static byte[] packageEvents(List<SensorsEvent> events) {
        // 根据事件数量和每个事件的长度计算总字节长度
        int totalLength = events.size() * SensorsEvent.EVENT_DATA_LENGTH;

        ByteBuffer buffer = ByteBuffer.allocate(totalLength);

        for (SensorsEvent event : events) {
            // 将每个事件的字节数据添加到缓冲区
            buffer.put(event.toByteArray());
        }

        // 返回包含所有事件数据的字节数组
        return buffer.array();
    }
}
