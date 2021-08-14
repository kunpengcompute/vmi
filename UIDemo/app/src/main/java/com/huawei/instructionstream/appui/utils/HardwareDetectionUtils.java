/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.utils;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Debug;
import android.text.format.Formatter;
import android.util.Log;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.math.BigDecimal;
import java.util.Locale;

/**
 * 获取android系统版本、CPU信息、内存信息的工具类。
 *
 * @since 2021-06-04
 */
public class HardwareDetectionUtils {
    private static final String TAG = "CGP-VMI-HardwareDetection";
    // 初始化大小
    private static final int INIT_SIZE = 8192;
    // 进制转换
    private static final int MEMORY_CONVERT_UNIT = 1024;
    private static final double CONVERT_UNIT = 1024.0D;
    private static final String HZ_CONVERT_UNIT = "100000.0";

    /**
     * 获取android系统版本
     *
     * @return android系统版本
     */
    public String getSystemVersion() {
        return android.os.Build.VERSION.RELEASE;
    }

    /**
     * 获取cpu最大频率
     *
     * @return cpu最大频率
     * @throws IOException
     */
    public String getCpuMaxFreq() throws IOException {
        BufferedReader br = null;
        long result = 0L;
        try {
            String line;
            br = new BufferedReader(new FileReader(
                    "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"));
            if ((line = br.readLine()) != null) {
                result = Long.parseLong(line);
            }
        } catch (IOException e) {
            LogUtils.error(TAG, "get cpu max frequency with IOException");
        } finally {
            br.close();
        }
        BigDecimal income = new BigDecimal(result);
        // 进制转换
        BigDecimal out = new BigDecimal(HZ_CONVERT_UNIT);
        BigDecimal re = income.divide(out);
        Double aDouble = re.doubleValue();
        String res = aDouble.toString();
        return res + " GHz";
    }

    /**
     * 获取cpu的型号
     *
     * @return cpu的型号
     * @throws IOException
     */
    public static String getCpuName() throws IOException {
        FileReader fr = null;
        BufferedReader br = null;
        try {
            fr = new FileReader("/proc/cpuinfo");
            br = new BufferedReader(fr);
            String text = br.readLine();
            String[] array = text.split(":\\s+", 2);
            if (array.length > 1) {
                return array[1];
            }
        } catch (FileNotFoundException e) {
            LogUtils.error(TAG, "get cpu name with FileNotFoundException");
        } finally {
            fr.close();
            br.close();
        }
        return "";
    }

    /**
     * 获取android总运行内存大小
     *
     * @param context 上下文
     * @return android总运行内存大小
     * @throws IOException
     */
    public String getTotalMemory(Context context) throws IOException {
        // 系统内存信息文件
        String memoryFileName = "/proc/meminfo";
        String memoryInfo;
        String[] arrayOfString;
        String totalMemory = "";
        long initialMemory = 0;
        FileReader localFileReader = null;
        BufferedReader localBufferedReader = null;
        try {
            localFileReader = new FileReader(memoryFileName);
            localBufferedReader = new BufferedReader(localFileReader, INIT_SIZE);
            // 读取meminfo第一行，系统总内存大小
            memoryInfo = localBufferedReader.readLine();
            arrayOfString = memoryInfo.split("\\s+");
            for (String num : arrayOfString) {
                LogUtils.info(memoryInfo, num + "\t");
            }
            if (arrayOfString.length > 1) {
                // 获得系统总内存，单位是KB
                int temp = Integer.valueOf(arrayOfString[1]).intValue();
                // int值乘以1024转换为long类型
                initialMemory = new Long((long) temp * MEMORY_CONVERT_UNIT);
            }
            totalMemory = Formatter.formatFileSize(context, initialMemory);
        } catch (IOException e) {
            LogUtils.error(TAG, "get total memory with FileNotFoundException");
        } finally {
            localFileReader.close();
            localBufferedReader.close();
        }
        return totalMemory;
    }

    /**
     * 获取android当前可用运行内存大小
     *
     * @param context 上下文
     * @return android当前可用运行内存大小
     */
    public String getAvailableMemory(Context context) {
        ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memoryInfo);
        // memoryInfo.availMem：当前系统的可用内存
        String availableMemory = Formatter.formatFileSize(context, memoryInfo.availMem);
        return availableMemory;
    }

    /**
     * 获取当前应用使用的内存大小
     *
     * @param context 上下文
     * @return 当前应用使用的内存大小
     */
    private String getCurrentAppUsingMemory(Context context) {
        ActivityManager activityManager = null;
        if (context.getSystemService(Context.ACTIVITY_SERVICE) instanceof ActivityManager) {
            activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        }
        double memory = 0.0D;
        if (activityManager.getProcessMemoryInfo(new int[]{android.os.Process.myPid()}) != null) {
            final Debug.MemoryInfo[] memInfo = activityManager.getProcessMemoryInfo(
                    new int[]{android.os.Process.myPid()});
            if (memInfo.length > 0) {
                final int totalPss = memInfo[0].getTotalPss();
                if (totalPss >= 0) {
                    // 单位转换
                    memory = totalPss / CONVERT_UNIT;
                }
            }
        } else {
            LogUtils.error(TAG, "当前进程不存在");
        }
        String result = String.format(Locale.ROOT, "%.2f", memory);
        return result + " MB";
    }
}
