package com.huawei.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.view.View;

import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.FullscreenActivity;

import java.util.regex.Pattern;

public class Utils {


    public static final String STOP_FLAG = "lastWriteFlag";
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

    public static SharedPreferences getSharedPreferences() {
        return MyApplication.instance.getSharedPreferences("sp", Context.MODE_PRIVATE);
    }

    public static void putString(String key, String value) {

        SharedPreferences.Editor editor = getSharedPreferences().edit();
        editor.putString(key, value);
        editor.apply();
    }

    public static String getString(String key, String value) {
        SharedPreferences preferences = getSharedPreferences();
        return preferences.getString(key, value);
    }

    public static void putBoolean(String key, Boolean value) {

        SharedPreferences.Editor editor = getSharedPreferences().edit();
        editor.putBoolean(key, value);
        editor.apply();
    }

    public static Boolean getBoolean(String key, Boolean value) {
        SharedPreferences preferences = getSharedPreferences();
        return preferences.getBoolean(key, value);
    }

    public static void setNavigationBarVisible(FullscreenActivity fullscreenActivity, boolean isHide) {
        View decorView = fullscreenActivity.getWindow().getDecorView();
        if (isHide) {
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_FULLSCREEN);
        } else {
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_VISIBLE);
        }
    }
}
