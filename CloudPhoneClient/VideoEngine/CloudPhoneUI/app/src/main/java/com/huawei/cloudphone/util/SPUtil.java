/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.util;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.text.TextUtils;

import com.google.gson.Gson;
import com.tencent.mmkv.MMKV;

/**
 * SharedPreference工具类.
 *
 * @author gj
 * @since 2019-09-24
 */
public class SPUtil {
    /**
     * 登录账号
     */
    public static final String KEY_ACCOUNT = "account";

    /**
     * 登录密码
     */
    public static final String KEY_PASSWORD = "password";

    /**
     * 用户信息
     */
    public static final String KEY_USER_INFO = "user_info";

    /**
     * BaseUrl
     */
    public static final String KEY_BASE_URL = "base_url";

    /**
     * 直连 IP
     */
    public static final String KEY_SERVER_IP = "server_ip";

    /**
     * 直连 Vmi Agent Port
     */
    public static final String KEY_SERVER_PORT = "server_port";

    /**
     * 直连 Video Agent Port
     */
    public static final String KEY_VIDEO_PORT = "video_port";

    /**
     * GameId
     */
    public static final String KEY_GAME_ID = "game_id";

    /**
     * ShowFps
     */
    public static final String KEY_SHOW_FPS = "show_fps";

    /**
     * 自动更新
     */
    public static final String KEY_AUTO_UPDATE = "auto_update";

    /**
     * 编码模式
     */
    public static final String KEY_ENCODE_MODE = "encode_mode";

    public static final String FRAME_RATE = "frame_rate";

    public static final String VIDEO_ENCODER_TYPE = "video_encoder_type";
    public static final String VIDEO_FRAME_TYPE = "video_frame_type";
    public static final String VIDEO_FRAME_RATE = "video_frame_rate";
    public static final String VIDEO_DENSITY = "video_density";
    public static final String VIDEO_FORCE_LANDSCAPE = "video_force_landscape";
    public static final String VIDEO_RENDER_OPTIMIZE = "video_render_optimize";
    public static final String VIDEO_ADAPTIVE_RESOLUTION = "video_adaptive_resolution";
    // 视频分辨率
    public static final String VIDEO_FRAME_SIZE_WIDTH = "video_frame_size_width";
    public static final String VIDEO_FRAME_SIZE_HEIGHT = "video_frame_size_height";
    public static final String VIDEO_FRAME_SIZE_WIDTH_ALIGNED = "video_frame_size_width_aligned";
    public static final String VIDEO_FRAME_SIZE_HEIGHT_ALIGNED = "video_frame_size_height_aligned";
    // 视频编码参数
    public static final String VIDEO_BIT_RATE = "video_bit_Rate";
    public static final String VIDEO_RC_MODE = "video_rc_mode";
    public static final String VIDEO_FORCE_KEY_FRAME = "video_force_key_frame";
    public static final String VIDEO_INTERPOLATION = "video_interpolation";
    public static final String VIDEO_PROFILE = "video_profile";
    public static final String VIDEO_GOP_SIZE = "video_gop_size";
    public static final String VIDEO_CRF = "video_crf";
    public static final String VIDEO_VBV_BUFFER_SIZE = "video_vbv_buffer_size";
    public static final String VIDEO_MAX_CRF_RATE = "video_max_rate";
    public static final String VIDEO_STREAM_WIDTH = "video_stream_width";
    public static final String VIDEO_STREAM_HEIGHT = "video_stream_height";
    // 出流分辨率默认index 540P\720P\1080P
    public static final String VIDEO_RESOLUTION_INDEX = "video_resolution_index";

    // 音频播放参数
    public static final String AUDIO_SAMPLE_INTERVAL = "audio_sampleInterval";
    public static final String AUDIO_PLAY_BITRATE = "audio_play_bitrate";
    public static final String AUDIO_PLAY_STREAM_TYPE = "audio_play_stream_type";
    // 麦克风
    public static final String MIC_STREAM_TYPE = "mic_stream_type";
    public static final String MIC_SAMPLE_INTERVAL = "mic_sample_interval";
    public static final String MIC_BITRATE = "mic_bitrate";
    public static final String TIME_MINUTE = "time_minute";

    /**
     * 录（截）屏权限
     */
    public static final String SCREEN_LIMIT = "screen_limit";

    /**
     * 是否允许录（截）屏
     */
    public static final String IS_SCREENSHOT = "is_screen_shot";

    /**
     * 是否显示悬浮按钮（仅测试使用）
     */
    public static final String IS_SHOW_FLOAT_VIEW = "is_show_float_view";

    /**
     * 录取日志
     */
    public static final String KEY_START_LOG = "start_log";

    /**
     * 文件名
     */
    private static final String FILE_NAME = "sp_b100";

    /**
     * 结束标志
     */
    public static final String STOP_FLAG = "last_write_flag";
    public static final String RECORD_FLAG = "record_flag";


    static MMKV preferences;

    public static void initMMKV(Application application) {
        Context context = application.createDeviceProtectedStorageContext();
        MMKV.initialize(context);
        preferences = MMKV.mmkvWithID(FILE_NAME, MMKV.MULTI_PROCESS_MODE);
    }

    /**
     * 保存String类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putString(String key, String value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(key, value);
    }

    /**
     * 获取String类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static String getString(String key, String defaultValue) {
        return preferences.getString(key, defaultValue);
    }

    /**
     * 保存int类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putInt(String key, int value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putInt(key, value);
    }

    /**
     * 获取int类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static int getInt(String key, int defaultValue) {
        return preferences.getInt(key, defaultValue);
    }

    /**
     * 保存long类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putLong(String key, long value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putLong(key, value);
    }

    /**
     * 获取long类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static long getLong(String key, long defaultValue) {
        return preferences.getLong(key, defaultValue);
    }

    /**
     * 保存boolean类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static void putBoolean(String key, boolean value) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean(key, value);
    }

    /**
     * 获取boolean类型字段
     *
     * @param key          Key
     * @param defaultValue default value if not found
     * @return Value
     */
    public static boolean getBoolean(String key, boolean defaultValue) {
        return preferences.getBoolean(key, defaultValue);
    }

    /**
     * 保存object类型字段
     *
     * @param key   Key
     * @param value Value
     */
    public static <T> void putObject(String key, T value) {
        putString(key, new Gson().toJson(value));
    }

    /**
     * 获取object类型字段
     *
     * @param key          Key
     * @param clazz        object class
     * @param defaultValue default value if not found
     * @return Value
     */
    public static <T> T getObject(String key, Class<T> clazz, T defaultValue) {
        String value = getString(key, null);
        if (!TextUtils.isEmpty(value)) {
            return new Gson().fromJson(value, clazz);
        }
        return defaultValue;
    }

    /**
     * 移除Key
     *
     * @param key Key
     */
    public static void removeKey(String key) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.remove(key);
    }
}
