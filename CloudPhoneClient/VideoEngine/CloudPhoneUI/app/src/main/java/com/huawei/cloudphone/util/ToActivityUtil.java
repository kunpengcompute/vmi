/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.util;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import com.huawei.cloudphonesdk.maincontrol.VideoConf;
import com.huawei.cloudphone.ui.activities.CreateDeviceActivity;
import com.huawei.cloudphone.ui.activities.FullscreenActivity;
import com.huawei.cloudphone.ui.activities.LoginActivity;
import com.huawei.cloudphone.ui.activities.RegisterActivity;
import com.huawei.cloudphone.ui.activities.TestActivity;

/**
 * 页面跳转.
 *
 * @author zhh
 * @since 2019-09-24
 */
public class ToActivityUtil {
    /**
     * 跳转登录页面
     *
     * @param context Context
     */
    public static void toLoginActivity(Context context) {
        Intent intent = new Intent(context, LoginActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        context.startActivity(intent);
    }

    /**
     * 跳转注册页面
     *
     * @param context Context
     */
    public static void toRegisterActivity(Context context) {
        Intent intent = new Intent(context, RegisterActivity.class);
        context.startActivity(intent);
    }

    /**
     * 跳转创建设备页面
     *
     * @param context Context
     */
    public static void toCreateDeviceActivity(Context context) {
        Intent intent = new Intent(context, CreateDeviceActivity.class);
        context.startActivity(intent);
    }

    /**
     * 跳转test页面
     *
     * @param context Context
     */
    public static void toTestActivity(Context context) {
        Intent intent = new Intent(context, TestActivity.class);
        context.startActivity(intent);
    }

    /**
     * 跳转视频流页面
     *
     * @param activity  Context
     * @param videoConf Video stream configuration
     */
    public static void toFullScreenActivity(Activity activity, VideoConf videoConf) {
        toFullScreenActivity(activity, videoConf, 0);
    }

    /**
     * 跳转视频流页面
     *
     * @param activity  Context
     * @param videoConf Video stream configuration
     * @param reqCode   request code
     */
    public static void toFullScreenActivity(Activity activity, VideoConf videoConf, int reqCode) {
        // opus编码比特率
        videoConf.setBitrate(SPUtil.getInt(SPUtil.BITRATE,192000));
        // 增加编码参数
        videoConf.setVideoEncoderType(SPUtil.getInt(SPUtil.VIDEO_ENCODER_TYPE, 0));
        videoConf.setVideoFrameType(SPUtil.getInt(SPUtil.VIDEO_FRAME_TYPE, 0));
        videoConf.setVideoFrameRate(SPUtil.getInt(SPUtil.VIDEO_FRAME_RATE, 30));
        videoConf.setVideoForceLandscape(SPUtil.getBoolean(SPUtil.VIDEO_FORCE_LANDSCAPE, false));
        videoConf.setVideoRenderOptimize(SPUtil.getBoolean(SPUtil.VIDEO_RENDER_OPTIMIZE, false));
        videoConf.setVideoFrameSizeWidth(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH, 720));
        videoConf.setVideoFrameSizeHeight(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT, 1280));
        videoConf.setVideoFrameSizeWidthAligned(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH_ALIGNED, 720));
        videoConf.setVideoFrameSizeHeightAligned(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT_ALIGNED, 1280));

        videoConf.setVideoBitRate(SPUtil.getInt(SPUtil.VIDEO_BIT_RATE, 3000000));
        videoConf.setVideoRcMode(SPUtil.getInt(SPUtil.VIDEO_RC_MODE, 2));
        videoConf.setVideoForceKeyFrame(SPUtil.getInt(SPUtil.VIDEO_FORCE_KET_FRAME, 0));
        videoConf.setVideoInterpolation(SPUtil.getInt(SPUtil.VIDEO_INTERPOLATION, 0));
        videoConf.setVideoProFile(SPUtil.getInt(SPUtil.VIDEO_PROFILE, 1));
        videoConf.setVideoGopSize(SPUtil.getInt(SPUtil.VIDEO_GOP_SIZE, 30));

        videoConf.setAudioSampleInterval(SPUtil.getInt(SPUtil.AUDIO_SAMPLE_INTERVAL, 10));
        videoConf.setAudioPlayBitrate(SPUtil.getInt(SPUtil.AUDIO_PLAY_BITRATE, 192000));
        videoConf.setAudioPlayStreamType(SPUtil.getInt(SPUtil.AUDIO_PLAY_STREAM_TYPE, 0));
        videoConf.setMicStreamType(SPUtil.getInt(SPUtil.MIC_STREAM_TYPE, 0));
        videoConf.setMicSampleInterval(SPUtil.getInt(SPUtil.MIC_SAMPLE_INTERVAL, 10));

        Bundle bundle = new Bundle();
        bundle.putParcelable("video_conf", videoConf);
        Intent intent = new Intent(activity, FullscreenActivity.class);
        intent.putExtras(bundle);
        if (reqCode > 0) {
            activity.startActivityForResult(intent, reqCode);
        } else {
            activity.startActivity(intent);
        }
    }
}