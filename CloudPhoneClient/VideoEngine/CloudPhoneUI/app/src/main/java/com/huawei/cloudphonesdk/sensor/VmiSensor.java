/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

package com.huawei.cloudphonesdk.sensor;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;

import com.huawei.cloudphonesdk.maincontrol.DataPipe;

public class VmiSensor implements SensorEventListener {

    private static final String TAG = "VmiSensor";
    private static VmiSensor instance = new VmiSensor();

    private SensorsEvent sensorsEvent;

    private SensorService mSensorService;

    /** return when execution succeeds */
    public static final int VMI_SUCCESS = 0;

    /** return when receiving msg fails */
    public static final int VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL = 0x0A040001;

    /** commond word received from server */
    protected static final int RETURN_REGISTER_CLIENT_SENSOR = 67239937;
    protected static final int RETURN_UNREGISTER_CLIENT_SENSOR = 67239938;
    protected static final int RETURN_UPDATE_CLIENT_SENSOR_RATE = 67239939;

    /** maximum packet length */
    protected static final int maxPkgLen = 4096;

    private static final int SIZE_OF_INT = Integer.SIZE; // 32 bits in an Integer
    private static final int SIZE_OF_BYTE = Byte.SIZE; // 8 bits in a Byte
    private static final int SIZE_OF_LONG = Long.SIZE; // 64 bits in an Integer
    private static final int INT_BYTES = SIZE_OF_INT / SIZE_OF_BYTE;; // 4 bytes in an Integer
    private static final int LONG_BYTES = SIZE_OF_LONG / SIZE_OF_BYTE; // 8 bytes in an Long

    private static final int SENSOR_ACTIVATE_DATA = INT_BYTES + LONG_BYTES;

    // 当前客户端只支持激活加速度计和陀螺仪传感器(0,1)
    private static final int COMMUNICATION_SENSORS_ACCELERATION = 0;
    private static final int COMMUNICATION_SENSORS_GYROSCOPE = 1;
    private static final int COMMUNICATION_SENSORS_MAGNETIC_FIELD = 2;
    private static final int COMMUNICATION_SENSORS_ORIENTATION = 3;
    private static final int COMMUNICATION_SENSORS_TEMPERATURE = 4;
    private static final int COMMUNICATION_SENSORS_PROXIMITY = 5;
    private static final int COMMUNICATION_SENSORS_LIGHT = 6;
    private static final int COMMUNICATION_SENSORS_PRESSURE = 7;
    private static final int COMMUNICATION_SENSORS_HUMIDITY = 8;
    private static final int COMMUNICATION_SENSORS_MAGNETIC_FIELD_UNCALIBRATED = 9;
    private static final int COMMUNICATION_SENSORS_MAX = 10;

    // 数据缓冲区
    private List<SensorsEvent> sensorsDataBuffer = new ArrayList<>();

    // 数据缓冲区的最大容量
    private static final int BUFFER_CAPACITY = 50; // 这个值可以根据需要调整

    // 传感器激活缓存
    boolean[] enableSensorsArray = new boolean[COMMUNICATION_SENSORS_MAX];

    // 传感器采样率缓存
    long[] samplingPeriodArray = new long[COMMUNICATION_SENSORS_MAX];

    // 发送数据的时间间隔, 单位为毫秒
    private long SEND_INTERVAL_MS = 200;

    private long lastSendTime = System.currentTimeMillis();

    private boolean isAgentConnected = false;

    private static final long nsPerSec = 1000000000; // 单位微妙 nanosecond

    private VmiSensor() {
        Arrays.fill(enableSensorsArray, false);
        Arrays.fill(samplingPeriodArray, nsPerSec);
    }

    DataPipe dataPipeInstance = DataPipe.getInstance();

    /**
     * Get instance of VmiSensor.
    *
    * @param Null
    * @return VmiSensor instance
    */
    public static VmiSensor getInstance() {
        return instance;
    }

    public void setSensorService(SensorService service) {
        this.mSensorService = service;
    } 

    /**
     * public interface to process sensor data packet.
    *
    * @param data   The received sensor data packet.
    * @param length The byte length of sensor data packet.
    * @return VMI_SUCCESS or VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL.
    */
    public int onRecvSensorPacket(byte[] data, int length) {
        if (data == null || length != data.length || length > maxPkgLen || data.length < INT_BYTES) {
            Log.e(TAG, "wrong sensor msg, input length:" + length + " , data length:" + data.length);
            return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
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
            case RETURN_REGISTER_CLIENT_SENSOR:
                return handleRegisterSensor(buffer);
            case RETURN_UNREGISTER_CLIENT_SENSOR:
                return handleUnregisterSensor(buffer);
            case RETURN_UPDATE_CLIENT_SENSOR_RATE:
                return handleRegisterSensor(buffer);
            default:
                Log.e(TAG, "Unknown cmd" + cmd + " received");
                return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
        }
    }

    private int handleRegisterSensor(ByteBuffer buffer) {
        if (mSensorService == null) {
            Log.e(TAG, "SensorService has not been initialized.");
            return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
        }

        while (buffer.remaining() >= SENSOR_ACTIVATE_DATA) { // While there's enough data for a group
            // ByteBuffer在每次调用getInt()或getLong()等方法后，会自动更新其内部指针来指向下一个未读的数据项
            int sensorType = buffer.getInt();
            Log.i(TAG, "activate sensor sensorType=" + sensorType);
            if (sensorType != COMMUNICATION_SENSORS_ACCELERATION && sensorType != COMMUNICATION_SENSORS_GYROSCOPE) {
                Log.e(TAG, "sensorType: " + sensorType + " is not supported.");
                return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
            }
            int androidSensorType = convertToAndroidSensorType(sensorType);
            long samplingPeriod = buffer.getLong();
            Log.i(TAG, "activate sensor samplingPeriod=" + samplingPeriod);
            if (!enableSensorsArray[sensorType] || samplingPeriod != samplingPeriodArray[sensorType]) {
                mSensorService.registerActivatedSensorListener(androidSensorType, samplingPeriod);
                enableSensorsArray[sensorType] = true;
                samplingPeriodArray[sensorType] = samplingPeriod;
            }
        }

        // 如果在处理所有数据组合后还有剩余的字节，这可能意味着有错误或不完整的数据
        if (buffer.remaining() > 0) {
            Log.e(TAG, "Extra bytes remaining in buffer");
            return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
        }

        updateSendInterval();

        isAgentConnected = true;
        return VMI_SUCCESS;
    }

    private int handleUnregisterSensor(ByteBuffer buffer) {
        if (mSensorService == null) {
            Log.e(TAG, "SensorService has not been initialized.");
            return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
        }
        if (buffer.remaining() < INT_BYTES) {
            Log.e(TAG, "No data for deactivate sensor");
            return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
        }
        int sensorType = buffer.getInt();
        Log.i(TAG, "deactivate sensor sensorType=" + sensorType);
        if (sensorType != COMMUNICATION_SENSORS_ACCELERATION && sensorType != COMMUNICATION_SENSORS_GYROSCOPE) {
            Log.e(TAG, "sensorType: " + sensorType + " is not supported.");
            return VMI_SENSOR_ENGINE_CLIENT_RECV_FAIL;
        }
        int androidSensorType = convertToAndroidSensorType(sensorType);
        if (enableSensorsArray[sensorType]) {
            mSensorService.unregisterDeactivateSensorListener(androidSensorType);
            enableSensorsArray[sensorType] = false;
            samplingPeriodArray[sensorType] = nsPerSec;
            updateSendInterval();
        } else {
            Log.e(TAG, "sensorType: " + sensorType + " is not activated.");
        }
        return VMI_SUCCESS;
    }

    private void updateSendInterval() {
        // 找到samplingPeriodArray中的最小值
        long minPeriodNs = Long.MAX_VALUE;
        for (long period : samplingPeriodArray) {
            if (period < minPeriodNs) {
                minPeriodNs = period;
            }
        }
    
        // 将纳秒转换为毫秒
        long minPeriodMs = minPeriodNs / 1000000;
    
        // 确保转换后的值适合int类型
        SEND_INTERVAL_MS = (int) Math.min(minPeriodMs, Integer.MAX_VALUE);
        Log.i(TAG, "SEND_INTERVAL_MS updated to: " + SEND_INTERVAL_MS);
    }

    public static int convertToAndroidSensorType(int communicationSensorType) {
        switch (communicationSensorType) {
            case COMMUNICATION_SENSORS_ACCELERATION:
                return Sensor.TYPE_ACCELEROMETER;
            case COMMUNICATION_SENSORS_GYROSCOPE:
                return Sensor.TYPE_GYROSCOPE;
            case COMMUNICATION_SENSORS_MAGNETIC_FIELD:
                return Sensor.TYPE_MAGNETIC_FIELD;
            case COMMUNICATION_SENSORS_ORIENTATION:
                return Sensor.TYPE_ORIENTATION;
            case COMMUNICATION_SENSORS_TEMPERATURE:
                return Sensor.TYPE_AMBIENT_TEMPERATURE;
            case COMMUNICATION_SENSORS_PROXIMITY:
                return Sensor.TYPE_PROXIMITY;
            case COMMUNICATION_SENSORS_LIGHT:
                return Sensor.TYPE_LIGHT;
            case COMMUNICATION_SENSORS_PRESSURE:
                return Sensor.TYPE_PRESSURE;
            case COMMUNICATION_SENSORS_HUMIDITY:
                return Sensor.TYPE_RELATIVE_HUMIDITY;
            case COMMUNICATION_SENSORS_MAGNETIC_FIELD_UNCALIBRATED:
                return Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED;
            default:
                throw new IllegalArgumentException("Unknown communication sensor type: " + communicationSensorType);
        }
    }

    // 将所有传感器的激活状态设置为 false, 采集率都设置成1秒
    public void resetSensorsArray() {
        Arrays.fill(enableSensorsArray, false);
        Arrays.fill(samplingPeriodArray, nsPerSec);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (!isAgentConnected) {
            return;
        }
        int communicationType = convertToCommunicationSensorType(event.sensor.getType());
        if (!enableSensorsArray[communicationType]) {
            return;
        }
        float xValue = event.values[0]; // Always at least one value in event.values
        float yValue = event.values.length > 1 ? event.values[1] : 0.0f;
        float zValue = event.values.length > 2 ? event.values[2] : 0.0f;
        // Instantiate a new SensorsEvent object for each sensor event
        SensorsEvent sensorsEvent = new SensorsEvent();
        sensorsEvent.setType(communicationType);
        sensorsEvent.setX(xValue);
        sensorsEvent.setY(yValue);
        sensorsEvent.setZ(zValue);
        synchronized (sensorsDataBuffer) {
            sensorsDataBuffer.add(sensorsEvent);
            if (sensorsDataBuffer.size() >= BUFFER_CAPACITY || 
                (System.currentTimeMillis() - lastSendTime) >= SEND_INTERVAL_MS) {
                sendData();
                lastSendTime = System.currentTimeMillis();
            }
        }
    }

    private void sendData() {
        synchronized (sensorsDataBuffer) {
            if (!sensorsDataBuffer.isEmpty()) {
                // 将缓冲区中的数据打包并发送
                byte[] data = SensorsEvent.packageEvents(sensorsDataBuffer);
                if (data != null) {
                    dataPipeInstance.sensorSendData(data, data.length);
                } else {
                    Log.e(TAG, "Failed to serialize sensor data");
                }
                // 清空缓冲区
                sensorsDataBuffer.clear();
            }
        }
    }

    public static int convertToCommunicationSensorType(int sensorType) {
        switch (sensorType) {
            case Sensor.TYPE_ACCELEROMETER:
                return COMMUNICATION_SENSORS_ACCELERATION;
            case Sensor.TYPE_GYROSCOPE:
                return COMMUNICATION_SENSORS_GYROSCOPE;
            case Sensor.TYPE_MAGNETIC_FIELD:
                return COMMUNICATION_SENSORS_MAGNETIC_FIELD;
            case Sensor.TYPE_ORIENTATION:
                return COMMUNICATION_SENSORS_ORIENTATION;
            case Sensor.TYPE_AMBIENT_TEMPERATURE:
                return COMMUNICATION_SENSORS_TEMPERATURE;
            case Sensor.TYPE_PROXIMITY:
                return COMMUNICATION_SENSORS_PROXIMITY;
            case Sensor.TYPE_LIGHT:
                return COMMUNICATION_SENSORS_LIGHT;
            case Sensor.TYPE_PRESSURE:
                return COMMUNICATION_SENSORS_PRESSURE;
            case Sensor.TYPE_RELATIVE_HUMIDITY:
                return COMMUNICATION_SENSORS_HUMIDITY;
            case Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                return COMMUNICATION_SENSORS_MAGNETIC_FIELD_UNCALIBRATED;
            default:
                throw new IllegalArgumentException("Unknown android sensor type: " + sensorType);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // 传感器精度发生变化触发
    }
}