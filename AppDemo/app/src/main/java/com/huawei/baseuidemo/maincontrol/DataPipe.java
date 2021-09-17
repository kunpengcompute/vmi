
package com.huawei.baseuidemo.maincontrol;

import android.util.Log;

import com.huawei.baseuidemo.engine.InstructionEngine;
import com.huawei.cloudgame.audioplay.AUDIOSENDHOOK;
import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudgame.touch.TOUCHSENDHOOK;
import com.huawei.cloudgame.touch.VmiTouch;

public class DataPipe implements TOUCHSENDHOOK, AUDIOSENDHOOK {

    private static final String AUDIO_TAG = "VMIAUDIOCLIENT";
    private static final String TOUCH_TAG = "VMITOUCHCLIENT";
    private static InstructionEngine instructionEngine;
    private static final DataPipe instance = new DataPipe();

    private DataPipe() {

    }

    private static DataPipe getInstance() {
        return instance;
    }

    /**
     * 设置指令流引擎对象
     *
     * @param engine
     */
    public static void setInstructionEngine(InstructionEngine engine) {
        instructionEngine = engine;
    }

    /**
     * 注册触控引擎接口
     */
    public static void registerHookToTouch() {
        // 调用触控引擎注册
        int ret = VmiTouch.getInstance().registerTouchSendHook(DataPipe.getInstance());
        if (ret == VmiTouch.VMI_SUCCESS) {
            // 注册触控引擎成功
            Log.i(TOUCH_TAG, "touch registerHookToTouch: VMI_SUCCESS" + "input"
                    + DataPipe.getInstance() + "output" + VmiTouch.VMI_SUCCESS);
        }
    }

    /**
     * 注册音频引擎接口
     */
    public static void registerAudioSendHook() {
        // 调用音频引擎注册
        int ret = AudioTrackPlayer.getInstance().registerAudioSendHook(DataPipe.getInstance());
        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            // 注册音频引擎成功
            Log.i(AUDIO_TAG, "audio registerAudioSendHook: VMI_SUCCESS" + "input"
                    + DataPipe.getInstance() + "output" + AudioTrackPlayer.VMI_SUCCESS);
        }
    }

    /**
     * 调用指令流引擎触控发送接口
     */
    public void touchSendData(byte[] data, int length) {
        if (!instructionEngine.sendTouchEventArray(data, length)) {
            Log.e(TOUCH_TAG, "touch send data failed.");
        }
    }

    /**
     * 调用录音发送接口
     */
    public void audioSendData(byte[] data, int length) {
        if (!instructionEngine.sendAudioDataArray(data, length)) {
            Log.e(AUDIO_TAG, "audio send data failed.");
        }
    }
}
