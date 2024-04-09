/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import android.os.Environment;
import androidx.annotation.Keep;

import android.util.Log;
import com.huawei.cloudgame.audioplay.AUDIOSENDHOOK;
import com.huawei.cloudgame.audioplay.AUDIOSAVEHOOK;
import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudgame.touch.TOUCHSENDHOOK;
import com.huawei.cloudgame.touch.VmiTouch;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * An example full-screen activity that shows and hides the system UI (i.e. status bar and navigation/system bar) with
 * user interaction.
 *
 * @since 2018-07-05
 */
@Keep
public class DataPipe implements TOUCHSENDHOOK, AUDIOSENDHOOK, AUDIOSAVEHOOK {
    // just for testVmiTouchJava : registerTouchSendHook\
    private static final String AUDIO_SAVE_PATH = Environment.getExternalStorageDirectory() + "/com.huawei.cloudphone/cache";
    private static final String CLASS_NAME = "android.os.SystemProperties";
    private static boolean testVmiTouchJava = false;
    private static boolean testAudioClient = false;
    private static final String TAG = "DataPipe";
    private static final String TEST_AUDIO_TAG = "TestVmiAudioClient";
    private static final String TEST_TOUCH_TAG = "TestVmiTouch";
    private static DataPipe instance = new DataPipe();
    private static boolean ready = false;

    private DataPipe() {
    }

    /**
     * DataPipe.
     *
     * @return instance.
     */
    public static DataPipe getInstance() {
        return instance;
    }

    /**
     * setTestTouch.
     *
     * @param enable enable
     */
    public static void setTestVmiTouchJava(boolean enable) {
        testVmiTouchJava = enable;
    }

    /**
     * set test audio client.
     *
     * @param enable enable
     */
    public static void setTestAudioClient(boolean enable) {
        testAudioClient = enable;
    }

    /**
     * registerHookToTouch.
     */
    public static void registerHookToTouch() {
        int ret;
        LogUtil.error(TAG, " testJava DataPipe " + testVmiTouchJava);
        if (testVmiTouchJava) {
            LogUtil.error(TEST_TOUCH_TAG, " testJava " + " register null");
            ret = VmiTouch.getInstance().registerTouchSendHook(null);
        } else {
            ret = VmiTouch.getInstance().registerTouchSendHook(DataPipe.getInstance());
            LogUtil.error(TEST_TOUCH_TAG, " testJava " + " register normal");
        }

        if (ret == VmiTouch.VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL) {
            LogUtil.error(TEST_TOUCH_TAG, " testJava registerHookFail: VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL"
                + " input " + null
                + " output " + VmiTouch.VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL
            );
        }

        if (ret == VmiTouch.VMI_SUCCESS) {
            LogUtil.error(TEST_TOUCH_TAG, "testJava registerHookSuccess: VMI_SUCCESS"
                + " input " + DataPipe.getInstance()
                + " output " + VmiTouch.VMI_SUCCESS
            );
        }
    }

    /**
     * registerHooksToAudio.
     */
    public static void registerHookToAudio() {
        int ret;
        LogUtil.info(TAG, " testAudioClient DataPipe " + testAudioClient);
        // audio send hook
	    if (testAudioClient) {
            LogUtil.error(TEST_AUDIO_TAG, "testAudioClient " + " redister audio send hook null");
            ret = AudioTrackPlayer.getInstance().registerAudioSendHook(null);
        } else {
            ret = AudioTrackPlayer.getInstance().registerAudioSendHook(DataPipe.getInstance());
            LogUtil.error(TEST_AUDIO_TAG, "testAudioClient " + " redister audio send hook normal");
        }

        if (ret == AudioTrackPlayer.VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL) {
            LogUtil.error(TEST_AUDIO_TAG, " testAudioClient registerSendHookFail: VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL"
                + " input " + null
                + " output " + AudioTrackPlayer.VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL
            );
        }

        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            LogUtil.error(TEST_AUDIO_TAG, " testAudioClient registerSendHookSuccess: VMI_SUCCESS"
                + " input " + DataPipe.getInstance()
                + " output " + AudioTrackPlayer.VMI_SUCCESS
            );
        }
        // audio save hook
        if (testAudioClient) {
            LogUtil.error(TEST_AUDIO_TAG, "testAudioClient " + " redister audio save hook null");
            ret = AudioTrackPlayer.getInstance().registerAudioSaveHook(null);
        } else {
            ret = AudioTrackPlayer.getInstance().registerAudioSaveHook(DataPipe.getInstance());
            LogUtil.info(TEST_AUDIO_TAG, "testAudioClient " + " redister audio save hook normal");
        }

        if (ret == AudioTrackPlayer.VMI_AUDIO_CLIENT_SAVEHOOK_REGISTER_FAIL) {
            LogUtil.error(TEST_AUDIO_TAG, " testAudioClient registerHookFail: VMI_AUDIO_CLIENT_SAVEHOOK_REGISTER_FAIL"
                + " input " + null
                + " output " + AudioTrackPlayer.VMI_AUDIO_CLIENT_SAVEHOOK_REGISTER_FAIL
            );
        }

        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            LogUtil.info(TEST_AUDIO_TAG, " testAudioClient registerSaveHookSuccess: VMI_SUCCESS"
                + " input " + DataPipe.getInstance()
                + " output " + AudioTrackPlayer.VMI_SUCCESS
            );
        }
    }

    /**
     * onNewPacket.
     *
     * @param data   byte[].
     * @param length int lenth.
     */
    @Override
    public void touchSendData(byte[] data, int length) {
        if (!OpenGLJniWrapper.sendTouchEvent(data, length)) {
            LogUtil.error(TEST_TOUCH_TAG, "sendTouchEvent failed");
        }
    }

    /**
     * send audio client data interface.
     *
     * @param data   byte[]. int length.
     * @param length int length
     */
    @Override
    public void audioSendData(byte[] data, int length) {
        if (!OpenGLJniWrapper.sendAudioDataArray(data, length)) {
            LogUtil.error(TEST_AUDIO_TAG, "sendAudioDataArray failed");
        }
    }

    /**
     * save audio client data interface.
     *
     * @param data   byte[]. int length.
     * @param length int length
     */
    @Override
    public void audioSaveData(byte[] data, int length) {
        String isSaveAudio = getProperty("audio.save.status", "0");
        if (isSaveAudio.equals("1")) {
            createFile();
            try {
                OutputStream outputStream = new FileOutputStream(new File(AUDIO_SAVE_PATH + "/audio"), true);
                outputStream.write(data, 0, length);
                Log.d(TAG, "audioSaveData: " + length);
                outputStream.flush();
            } catch (FileNotFoundException e) {
                throw new RuntimeException(e);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void createFile() {
        File fileDir = new File(AUDIO_SAVE_PATH);
        if (!fileDir.exists()) {
            boolean result = fileDir.mkdirs();
            Log.d(TAG, "创建文件夹:" + result);
        }
        File logFile = new File(AUDIO_SAVE_PATH + "/audio");
        if (logFile.exists() && logFile.isFile()) {
            return;
        }
        if (!logFile.exists()) {
            try {
                logFile.createNewFile();
            } catch (IOException e) {
                LogUtil.error(TAG, "createLogFile: " + e.getMessage());
            }
        }
    }

    private String getProperty(String key, String defaultValue) {
        String value = defaultValue;
        try {
            Class<?> className = Class.forName(CLASS_NAME);
            Method get = className.getMethod("get", String.class, String.class);
            value = (String) get.invoke(className, key, defaultValue);
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        } catch (NoSuchMethodException e) {
            throw new RuntimeException(e);
        } catch (InvocationTargetException e) {
            throw new RuntimeException(e);
        } catch (IllegalAccessException e) {
            throw new RuntimeException(e);
        } finally {
            return value;
        }
    }
}
