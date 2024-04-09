/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.util;

import com.huawei.cloudphonesdk.utils.LogUtil;

import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;

/**
 * 工具类
 *
 * @since 2020-1-13
 */
public class CheckRootUtil {
    private static String TAG = CheckRootUtil.class.getName();

    /**
     * 是否root
     *
     * @return boolean true/false
     */
    public static boolean isDeviceRooted() {
        if (checkSu()) {
            return true;
        }
        return checkGetRootAuth();
    }

    /**
     * 执行su命令,看是否有root权限
     *
     * @return boolean true/false
     */
    private static synchronized boolean checkGetRootAuth() {
        Process process = null;
        DataOutputStream os = null;
        try {
            LogUtil.info(TAG, "to exec su");
            process = Runtime.getRuntime().exec("su");
            os = new DataOutputStream(process.getOutputStream());
            os.writeBytes("exit\n");
            os.flush();
            int exitValue = process.waitFor();
            LogUtil.info(TAG, "exitValue=" + exitValue);
            return exitValue == 0;
        } catch (IOException | InterruptedException e) {
            LogUtil.info(TAG, "Unexpected error - Here is what I know: "
                + e.getMessage());
            return false;
        } finally {
            try {
                if (os != null) {
                    os.close();
                }
                if (process != null) {
                    process.destroy();
                }
            } catch (IOException e) {
                LogUtil.info(TAG, "checkGetRootAuth: " + e);
            }
        }
    }

    /**
     * 检查文件夹,有一个存在,就已root
     *
     * @return boolean true/false
     */
    private static boolean checkSu() {
        boolean res;
        if ((!new File("/system/bin/su").exists()) &&
            (!new File("/system/xbin/su").exists())) {
            res = false;
        } else {
            res = true;
        }
        return res;
    }
}
