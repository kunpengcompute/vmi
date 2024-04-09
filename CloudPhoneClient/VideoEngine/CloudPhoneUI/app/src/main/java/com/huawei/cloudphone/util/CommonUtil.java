/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.util;

import android.text.TextUtils;

import com.huawei.cloudphonesdk.utils.LogUtil;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.regex.Pattern;

/**
 * 工具类
 *
 * @author gj
 * @since 2019-09-24
 */
public class CommonUtil {
    private static final String TAG = CommonUtil.class.getSimpleName();

    /**
     * 注册时判断账号是否合法 1.以字母开头 2.只能包含数字和英文字母
     *
     * @param account 账号
     * @return true:合法 false:非法
     */
    public static boolean isRegisterValidAccount(String account) {
        String regex = "^[A-Za-z][0-9A-Za-z]{2,19}$";
        return account.matches(regex);
    }

    /**
     * 注册时判断密码是否合法  1.同时包含数字、英文字母和特殊字符@.!_  2.不可包含两个以上连续字符
     *
     * @param password 密码
     * @return true:合法 false:非法
     */
    public static boolean isRegisterValidPassword(String password) {
        String regex = "^(?![0-9]+$)(?![a-zA-Z]+$)[0-9A-Za-z_@.!]{8,20}$";
        if (!password.matches(regex)) {
            return false;
        }
        if (!password.matches(".*\\d+.*")) {
            return false;
        }
        if (!password.matches(".*[A-Za-z]+.*")) {
            return false;
        }
        return !password.matches("^.*(.)\\1{2}.*$");
    }

    /**
     * 毫秒转时分秒字符串
     *
     * @param timeMills ms
     * @return timeString->hh:mm:ss
     */
    public static String timeMillsToString(long timeMills) {
        long secondMills = 1000;
        long minuteMills = 1000 * 60;
        long hourMills = 1000 * 60 * 60;

        long hours = timeMills / hourMills;
        long minutes = (timeMills - hours * hourMills) / minuteMills;
        long seconds = (timeMills - hours * hourMills - minutes * minuteMills) / secondMills;
        String hourString = hours < 10 ? "0" + hours : "" + hours;
        String minuteString = minutes < 10 ? "0" + minutes : "" + minutes;
        String secondString = seconds < 10 ? "0" + seconds : "" + seconds;
        return hourString + "h" + minuteString + "m" + secondString + "s";
    }

    /**
     * 字符串转int
     *
     * @param string       string Value
     * @param defaultValue default value if parse failed
     * @return int value
     */
    public static int stringToInt(String string, int defaultValue) {
        if (TextUtils.isEmpty(string)) {
            return defaultValue;
        }
        try {
            return Integer.parseInt(string);
        } catch (NumberFormatException e) {
            return defaultValue;
        }
    }

    /**
     * 字符串加密
     *
     * @param strText 待加密字符串
     * @param strType 加密方式
     * @return 加密后字符串
     */
    public static String sha(final String strText, final String strType) {
        // 是否是有效字符串
        if (TextUtils.isEmpty(strText)) {
            return "";
        }
        try {
            // sha 加密开始
            // 创建加密对象，并传入加密类型
            MessageDigest messageDigest = MessageDigest.getInstance(strType);
            // 传入要加密的字符串
            messageDigest.update(strText.getBytes(StandardCharsets.UTF_8));
            // 得到 byte 类型结果
            byte[] byteBuffer = messageDigest.digest();
            // 将 byte 转换为 string
            StringBuilder strHexString = new StringBuilder();
            // 遍历 byte buffer
            for (byte aByteBuffer : byteBuffer) {
                String hex = Integer.toHexString(0xff & aByteBuffer);
                if (hex.length() == 1) {
                    strHexString.append('0');
                }
                strHexString.append(hex);
            }
            // 得到返回結果
            return strHexString.toString();
        } catch (NoSuchAlgorithmException e) {
            LogUtil.error(TAG, "NoSuchAlgorithmException");
        }
        return "";
    }

    /**
     * 校验IP是否合法.
     *
     * @param text ip 地址
     * @return true:valid false:invalid
     */
    public static boolean isValidIp(String text) {
        if (text != null && !text.isEmpty()) {
            String regex = "^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
                + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$";
            return text.matches(regex);
        }
        return false;
    }

    /**
     * 校验端口号是否合法.
     *
     * @param port 端口号
     * @return true:valid false:invalid
     */
    public static boolean isValidPort(String port) {
        Pattern pattern = Pattern.compile("[1-9][0-9]{0,5}");
        if (pattern.matcher(port).matches()) {
            int num = Integer.parseInt(port);
            LogUtil.error(TAG, "num:" + num);
            return num >= 1 && num <= 65535;
        }
        return false;
    }
}
