/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import android.content.DialogInterface;

import androidx.annotation.StringRes;
import androidx.appcompat.app.AlertDialog;

import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.dialog.AudioPlayParamsDialog;
import com.huawei.cloudphone.dialog.EncodeDialog;
import com.huawei.cloudphone.dialog.FrameSizeParamsDialog;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphone.util.ViewUtil;
import com.huawei.cloudphone.widget.TopBar;
import com.huawei.cloudphonesdk.utils.LogUtil;

/**
 * 用户信息页
 *
 * @author gj
 * @since 2019-09-24
 */
public class SettingsActivity extends BaseActivity implements View.OnClickListener,
    CompoundButton.OnCheckedChangeListener {
    private static final String TAG = SettingsActivity.class.getSimpleName();
    private static final int FLAG_VIDEO_ENCODER_TYPE = 0;
    private static final int FLAG_VIDEO_FRAME_TYPE = 1;
    private static final int FLAG_VIDEO_FRAME_RATE = 2;
    private static final int FLAG_AUDIO_STREAM_TYPE = 3;
    private static final int FLAG_MIC_STREAM_TYPE = 4;
    private static final int OPUS_BITRATE_MIN = 500;
    private static final int OPUS_BITRATE_MAX = 512000;
    private static final int OPUS_BITRATE_DEFAULT = 192000;
    private static final int MIC_SAMPLE_INTERVAL_DEFAULT = 10;

    // 导航栏
    private TopBar mTopBar;

    // 分辨率
    private LinearLayout mResolutionLinearLayout;
    private TextView mResolutionTextView;

    // 编码模式
    private LinearLayout mEncodeModeLinearLayout;
    private TextView mEncodeModeTextView;

    // 编码参数
    private LinearLayout mEncodeParamLinearLayout;
    private TextView mEncodeParamTextView;

    // 退出登录
    private Button mLogoutButton;

    // 客户端三个弹窗的数据
    private String[] mDecodeMethodArray;
    private String[] mDefinitionArray;
    private String[] mResolutionArray;

    // 客户端三个弹窗选择的位置
    private int mEncoderTypeIndex;
    private int mVideoFrameTypeIndex;
    private int mFrameRateTypeIndex;

    // 服务端编码模式数据
    private String[] mEncoderTypeArray;

    // 服务端编码模式弹框选择位置
    private int mEncodeModeIndex;
    private EncodeDialog encodeDialog;
    private AudioPlayParamsDialog audioPlayParamsDialog;
    private FrameSizeParamsDialog frameSizeParamsDialog;

    // 显示帧率
    private CheckBox mShowFpsCheckBox;

    // 自动更新
    private CheckBox mAutoUpdateCheckBox;

    // 抓取日志
    private CheckBox mStartLogCheckBox;
    private TextView mEncoderTypeTextView;
    private TextView mVideoFrameTypeTextView;
    private String[] mVideoFrameType;
    private String[] mFrameRateType;
    private TextView mFrameRateTypeTextView;
    private String[] mAudioStreamType;
    private TextView mAudioPlayParamsTextView;
    private TextView mAudioStreamTypeTextView;
    private TextView mMicStreamTypeTextView;
    private EditText opusBitRate;
    private EditText micSampleInterval;

    @Override
    protected int getLayoutRes() {
        return R.layout.activity_settings;
    }

    @Override
    protected void initView() {
        mTopBar = findViewById(R.id.tb_header);
        mEncodeModeLinearLayout = findViewById(R.id.ll_encoder_type);
        mEncodeParamLinearLayout = findViewById(R.id.ll_encode_parameter);
        mEncodeParamTextView = findViewById(R.id.tv_parameter);
        mEncoderTypeTextView = findViewById(R.id.tv_encoder_type);
        mVideoFrameTypeTextView = findViewById(R.id.tv_video_frame_type);
        mFrameRateTypeTextView = findViewById(R.id.tv_video_frame_rate);
        mAudioPlayParamsTextView = findViewById(R.id.tv_audio_play_params);
        mAudioStreamTypeTextView = findViewById(R.id.tv_audio_stream_type);
        mMicStreamTypeTextView = findViewById(R.id.tv_mic_input_type);
        opusBitRate = findViewById(R.id.tv_opus_bitrate);
        micSampleInterval = findViewById(R.id.tv_mic_sample_interval);

        mMicStreamTypeTextView.setText(SPUtil.getInt(SPUtil.MIC_STREAM_TYPE, 0) == 0 ? "OPUS" : "PCM");
        CheckBox forceLandScapeCheckBox = findViewById(R.id.cb_force_Landscape);
        CheckBox renderOptimizeCheckBox = findViewById(R.id.cb_render_optimize);
        forceLandScapeCheckBox.setOnCheckedChangeListener(this);
        renderOptimizeCheckBox.setOnCheckedChangeListener(this);
        forceLandScapeCheckBox.setChecked(SPUtil.getBoolean(SPUtil.VIDEO_FORCE_LANDSCAPE, false));
        renderOptimizeCheckBox.setChecked(SPUtil.getBoolean(SPUtil.VIDEO_RENDER_OPTIMIZE, false));
        int opusBitRateValue = SPUtil.getInt(SPUtil.BITRATE, 192000);
        opusBitRate.setText(String.valueOf(opusBitRateValue));
        int micSampleIntervalValue = SPUtil.getInt(SPUtil.MIC_SAMPLE_INTERVAL, 10);
        micSampleInterval.setText(String.valueOf(micSampleIntervalValue));

        mVideoFrameType = getResources().getStringArray(R.array.array_video_frame_type);
        mEncoderTypeArray = getResources().getStringArray(R.array.array_video_encoder_type);
        mFrameRateType = getResources().getStringArray(R.array.array_frame_rate);
        mAudioStreamType = getResources().getStringArray(R.array.array_audio_stream_type);

        mEncodeModeIndex = SPUtil.getInt(SPUtil.KEY_ENCODE_MODE, 0);
    }

    @Override
    protected void setListener() {
        // 导航栏点击
        mTopBar.setOnTopBarClickListener(new TopBar.OnTopBarClickListener() {
            @Override
            public void onBackClicked(View view) {
                finish();
            }

            @Override
            public void onMenuClicked(View view) {
                saveSettings();
            }
        });
    }

    private void saveSettings() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("是否保存所有修改？");
        builder.setPositiveButton("保存", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                int opusBitRateValue = Integer.parseInt(opusBitRate.getText().toString());
                if (opusBitRateValue > OPUS_BITRATE_MIN && opusBitRateValue < OPUS_BITRATE_MAX) {
                    SPUtil.putInt(SPUtil.BITRATE, opusBitRateValue);
                    Log.i(TAG, "onClick: 麦克风opus编码参数:" + opusBitRateValue);
                } else {
                    SPUtil.putInt(SPUtil.BITRATE, OPUS_BITRATE_DEFAULT);
                    Toast.makeText(SettingsActivity.this, "麦克风opus编码参数不支持，请重新输入", Toast.LENGTH_LONG).show();
                    Log.i(TAG, "onClick: 麦克风opus编码参数不支持,已使用默认值192000");
                    opusBitRate.setText(String.valueOf(OPUS_BITRATE_DEFAULT));
                }
                SPUtil.putInt(SPUtil.BITRATE, opusBitRateValue);
                int micSampleIntervalValue = Integer.parseInt(micSampleInterval.getText().toString());
                if (micSampleIntervalValue != 5 && micSampleIntervalValue != 10 && micSampleIntervalValue != 20) {
                    Toast.makeText(SettingsActivity.this, "麦克风采样间隔不支持，请重新输入", Toast.LENGTH_LONG).show();
                    SPUtil.putInt(SPUtil.MIC_SAMPLE_INTERVAL, MIC_SAMPLE_INTERVAL_DEFAULT);
                    micSampleInterval.setText(String.valueOf(MIC_SAMPLE_INTERVAL_DEFAULT));
                    Log.i(TAG, "onClick: 麦克风采样间隔不支持,已使用默认值10");
                } else {
                    SPUtil.putInt(SPUtil.MIC_SAMPLE_INTERVAL, micSampleIntervalValue);
                    Log.i(TAG, "onClick: 麦克风采样间隔:" + micSampleIntervalValue);
                }
            }
        });
        builder.create().show();
    }

    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {
        int id = compoundButton.getId();
        if (id == R.id.cb_force_Landscape) {
            SPUtil.putBoolean(SPUtil.VIDEO_FORCE_LANDSCAPE, checked);
        } else if (id == R.id.cb_render_optimize) {
            SPUtil.putBoolean(SPUtil.VIDEO_RENDER_OPTIMIZE, checked);
        } else {
            LogUtil.info(TAG, "onCheckedChanged: ignored");
        }
    }

    @Override
    public void onClick(View view) {
        if (ClickUtil.isFastClick()) {
            return;
        }
        switch (view.getId()) {
            case R.id.ll_encoder_type: { // 解码方式
                showChooseDialog(FLAG_VIDEO_ENCODER_TYPE, R.string.encoder_type, mEncoderTypeArray,
                    mEncoderTypeIndex, mEncoderTypeTextView);
                break;
            }
            case R.id.ll_video_frame_type: {
                showChooseDialog(FLAG_VIDEO_FRAME_TYPE, R.string.video_frame_type, mVideoFrameType,
                    mVideoFrameTypeIndex, mVideoFrameTypeTextView);
                break;
            }
            case R.id.ll_video_frame_rate: {
                showChooseDialog(FLAG_VIDEO_FRAME_RATE, R.string.frame_rate, mFrameRateType,
                    mFrameRateTypeIndex, mFrameRateTypeTextView);
                break;
            }
            case R.id.ll_encode_parameter: {
                showEncodeInputDialog();
                break;
            }
            case R.id.ll_video_frame_size: {
                showFrameSizeInputDialog();
                break;
            }
            case R.id.ll_audio_stream_type: {
                showChooseDialog(FLAG_AUDIO_STREAM_TYPE, R.string.audio_stream_type, mAudioStreamType,
                    mFrameRateTypeIndex, mAudioStreamTypeTextView);
                break;
            }
            case R.id.ll_mic_input_type: {
                showChooseDialog(FLAG_MIC_STREAM_TYPE, R.string.mic_stream_type, mAudioStreamType,
                    mFrameRateTypeIndex, mMicStreamTypeTextView);
                break;
            }
            case R.id.ll_audio_play_params: {
                showAudioPlayInputDialog();
                break;
            }
            default: {
                break;
            }
        }
    }

    private void logout() {
    }

    /**
     * 显示弹窗
     *
     * @param flag          标识是弹窗类型
     * @param titleRes      弹窗标题
     * @param data          弹窗列表数据
     * @param selectedIndex 选中的位置
     * @param textView      显示选中的文字
     */
    private void showChooseDialog(final int flag, @StringRes final int titleRes, final String[] data,
                                  final int selectedIndex, final TextView textView) {
        AlertDialog alertDialog = new AlertDialog.Builder(mActivity)
            .setTitle(titleRes)
            .setSingleChoiceItems(data, selectedIndex, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int position) {
                    if (flag == FLAG_VIDEO_ENCODER_TYPE) {
                        SPUtil.putInt(SPUtil.VIDEO_ENCODER_TYPE, position);
                    } else if (flag == FLAG_VIDEO_FRAME_TYPE) {
                        SPUtil.putInt(SPUtil.VIDEO_FRAME_TYPE, position);
                    } else if (flag == FLAG_VIDEO_FRAME_RATE) {
                        SPUtil.putInt(SPUtil.VIDEO_FRAME_RATE, position == 0 ? 30 : 60);
                    } else if (flag == FLAG_AUDIO_STREAM_TYPE) {
                        SPUtil.putInt(SPUtil.AUDIO_PLAY_STREAM_TYPE, position);
                    } else if (flag == FLAG_MIC_STREAM_TYPE) {
                        SPUtil.putInt(SPUtil.MIC_STREAM_TYPE, position);
                    }
                    ViewUtil.setText(textView, data[position]);
                    dialog.dismiss();
                }
            }).create();
        alertDialog.show();
    }

    /**
     * 显示输入弹窗
     */
    private void showEncodeInputDialog() {
        if (encodeDialog == null) {
            encodeDialog = new EncodeDialog(mActivity);
            encodeDialog.setNegativeButton(new View.OnClickListener() {
                @Override
                public void onClick(View mView) {
                    encodeDialog.cancel();
                }
            });
            encodeDialog.setPositiveButton(new View.OnClickListener() {
                @Override
                public void onClick(View mView) {
                    sendEncodeParam();
                }
            });
        }
        encodeDialog.show();
    }

    /**
     * 显示输入弹窗
     */
    private void showFrameSizeInputDialog() {
        if (frameSizeParamsDialog == null) {
            frameSizeParamsDialog = new FrameSizeParamsDialog(mActivity);
            frameSizeParamsDialog.setTitle("分辨率");
            frameSizeParamsDialog.setNegativeButton(new View.OnClickListener() {
                @Override
                public void onClick(View mView) {
                    frameSizeParamsDialog.cancel();
                }
            });
            frameSizeParamsDialog.setPositiveButton(new View.OnClickListener() {
                @Override
                public void onClick(View mView) {
                    sendFrameSizeParam();
                }
            });
        }
        frameSizeParamsDialog.show();
    }

    private void sendFrameSizeParam() {
        saveFrameSizeWidth();
        saveFrameSizeHeight();
        saveFrameSizeWidthAligned();
        saveFrameSizeHeightAligned();
    }

    private boolean saveFrameSizeWidth() {
        String frameSizeWidth = frameSizeParamsDialog.getFrameSizeWidth();
        if (!TextUtils.isEmpty(frameSizeWidth)) {
            int value = Integer.parseInt(frameSizeWidth);
            if (value != 720 && value != 1080) {
                ToastUtil.showToast("宽度只能输入720或1080");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH, value);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_FRAME_SIZE_WIDTH);
        }
        return true;
    }

    private boolean saveFrameSizeHeight() {
        String frameSizeHeight = frameSizeParamsDialog.getFrameSizeHeight();
        if (!TextUtils.isEmpty(frameSizeHeight)) {
            int value = Integer.parseInt(frameSizeHeight);
            if (value != 1280 && value != 1920) {
                ToastUtil.showToast("宽度只能输入1280或1920");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT, value);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_FRAME_SIZE_HEIGHT);
        }
        return true;
    }

    private boolean saveFrameSizeWidthAligned() {
        String frameSizeWidthAligned = frameSizeParamsDialog.getFrameSizeWidthAligned();
        if (!TextUtils.isEmpty(frameSizeWidthAligned)) {
            int value = Integer.parseInt(frameSizeWidthAligned);
            if (value != 720 && value != 1080) {
                ToastUtil.showToast("宽度只能输入720或1080");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH_ALIGNED, value);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_FRAME_SIZE_WIDTH_ALIGNED);
        }
        return true;
    }

    private boolean saveFrameSizeHeightAligned() {
        String frameSizeHeight = frameSizeParamsDialog.getFrameSizeHeightAligned();
        if (!TextUtils.isEmpty(frameSizeHeight)) {
            int value = Integer.parseInt(frameSizeHeight);
            if (value != 1280 && value != 1920) {
                ToastUtil.showToast("宽度只能输入1280或1920");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT_ALIGNED, value);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_FRAME_SIZE_HEIGHT_ALIGNED);
        }
        return true;
    }

    private void showAudioPlayInputDialog() {
        if (audioPlayParamsDialog == null) {
            audioPlayParamsDialog = new AudioPlayParamsDialog(mActivity);
            audioPlayParamsDialog.setNegativeButton(new View.OnClickListener() {
                @Override
                public void onClick(View mView) {
                    audioPlayParamsDialog.cancel();
                }
            });
            audioPlayParamsDialog.setPositiveButton(new View.OnClickListener() {
                @Override
                public void onClick(View mView) {
                    sendAudioPlayParam();
                }
            });
        }
        audioPlayParamsDialog.show();
    }

    private void sendAudioPlayParam() {
        // 保存bitrate
        if (!saveAudioPlayBitrate()) {
            return;
        }
        // 保存profile
        if (!saveSampleInterval()) {
        }
    }

    private boolean saveAudioPlayBitrate() {
        String bitRate = audioPlayParamsDialog.getBitRate();
        if (!TextUtils.isEmpty(bitRate)) {
            int bitRateValue = Integer.parseInt(bitRate);
            if (bitRateValue < 500 || bitRateValue > 512000) {
                ToastUtil.showToast("bitRate范围为500~512000");
                return false;
            }
            SPUtil.putInt(SPUtil.AUDIO_PLAY_BITRATE, bitRateValue);
        } else {
            SPUtil.removeKey(SPUtil.AUDIO_PLAY_BITRATE);
        }
        return true;
    }

    private boolean saveSampleInterval() {
        String sampleInterval = audioPlayParamsDialog.getSampleInterval();
        if (!TextUtils.isEmpty(sampleInterval)) {
            int bitRateValue = Integer.parseInt(sampleInterval);
            if (bitRateValue != 5 && bitRateValue != 10 && bitRateValue != 20) {
                ToastUtil.showToast("音频播放采样间隔仅支持5ms/10ms/20ms");
                return false;
            }
            SPUtil.putInt(SPUtil.AUDIO_SAMPLE_INTERVAL, bitRateValue);
        } else {
            SPUtil.removeKey(SPUtil.AUDIO_SAMPLE_INTERVAL);
        }
        return true;
    }

    /**
     * 清空用户信息
     */
    private void clearUserInfo() {
        // 清空内存缓存
        MyApplication.instance.setUserBean(null);
        // 清空本地缓存
        SPUtil.removeKey(SPUtil.KEY_USER_INFO);
    }

    /**
     * 发送编码参数
     */
    private void sendEncodeParam() {
        // 保存bitrate
        if (!saveBitrate()) {
            return;
        }
        // 保存profile
        if (!saveProfile()) {
            return;
        }
        // 保存gopSize
        if (!saveGopSize()) {
            return;
        }

        if (!saveRcMode()) {
            return;
        }

        if (!saveInterpolation()) {
            return;
        }

        if (!saveForceKeyFrame()) {
            return;
        }
        encodeDialog.cancel();
    }

    /**
     * 保存bitrate
     *
     * @return isSuccess
     */
    private boolean saveBitrate() {
        String bitRate = encodeDialog.getBitRate();
        if (!TextUtils.isEmpty(bitRate)) {
            int bitRateValue = Integer.parseInt(bitRate);
            if (bitRateValue < 1000000 || bitRateValue > 10000000) {
                ToastUtil.showToast("bitRate范围为1M~10M");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_BIT_RATE, bitRateValue);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_BIT_RATE);
        }
        return true;
    }

    /**
     * 保存bitrate
     *
     * @return isSuccess
     */
    private boolean saveRcMode() {
        String rcMode = encodeDialog.getRcModeEditText();
        if (!TextUtils.isEmpty(rcMode)) {
            int rcModeValue = Integer.parseInt(rcMode);
            if (rcModeValue != 0 && rcModeValue != 1 && rcModeValue != 2 && rcModeValue != 3) {
                ToastUtil.showToast("rcModeValue只可以取0，1，2，3.");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_RC_MODE, rcModeValue);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_RC_MODE);
        }
        return true;
    }

    private boolean saveForceKeyFrame() {
        String keyFrame = encodeDialog.getKeyFrameEditText();
        if (!TextUtils.isEmpty(keyFrame)) {
            int keyFrameValue = Integer.parseInt(keyFrame);
            if (keyFrameValue < 0 || keyFrameValue > 3000) {
                ToastUtil.showToast("rcModeValue只可以取0~3000,0表示不生效。");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_FORCE_KET_FRAME, keyFrameValue);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_FORCE_KET_FRAME);
        }
        return true;
    }

    private boolean saveInterpolation() {
        String interpolation = encodeDialog.getInterpolationEditText();
        if (!TextUtils.isEmpty(interpolation)) {
            int interpolationValue = Integer.parseInt(interpolation);
            if (interpolationValue != 0 && interpolationValue != 1) {
                ToastUtil.showToast("interpolationValue只可以取0，1.");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_INTERPOLATION, interpolationValue);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_INTERPOLATION);
        }
        return true;
    }

    /**
     * 保存profile
     *
     * @return isSuccess
     */
    private boolean saveProfile() {
        String profile = encodeDialog.getProfile();
        if (!TextUtils.isEmpty(profile)) {
            int profileValue = Integer.parseInt(profile);
            if (profileValue != 0 && profileValue != 1 && profileValue != 2) {
                ToastUtil.showToast("profile 只能取0,1,2");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_PROFILE, profileValue);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_PROFILE);
        }
        return true;
    }

    /**
     * 保存gopSize
     *
     * @return isSuccess
     */
    private boolean saveGopSize() {
        String gopSize = encodeDialog.getGopSize();
        if (!TextUtils.isEmpty(gopSize)) {
            int gopSizeValue = Integer.parseInt(gopSize);
            if (gopSizeValue < 30 || gopSizeValue > 3000) {
                ToastUtil.showToast("gopSize范围为30~3000");
                return false;
            }
            SPUtil.putInt(SPUtil.VIDEO_GOP_SIZE, gopSizeValue);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_GOP_SIZE);
        }
        return true;
    }
}
