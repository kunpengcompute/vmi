package com.huawei.cloudphone;

import java.util.regex.Pattern;

public class Utils {


    private static long lastClickTime;

    public static boolean isFastClick() {
        long currentClickTime = System.currentTimeMillis();
        if ((currentClickTime - lastClickTime) >= 1000) {
            return false;
        }
        lastClickTime = currentClickTime;
        return true;
    }

    public static boolean isValidIp(String ip) {
        if (ip != null && !ip.isEmpty()) {
            String regex = "^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\." +
                    "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\.(1\\d{2}|2[0-4]" +
                    "\\d|25[0-5]|[1-9]\\d|\\d)\\.(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$";
            return ip.matches(regex);
        }
        return false;
    }

    public static boolean isValidPort(String port) {
        Pattern pattern = Pattern.compile("[1-9][0-9]{0,5}");
        if (pattern.matcher(port).matches()) {
            int num = Integer.parseInt(port);
            return num >= 1 && num <= 65535;
        }
        return false;
    }
}
