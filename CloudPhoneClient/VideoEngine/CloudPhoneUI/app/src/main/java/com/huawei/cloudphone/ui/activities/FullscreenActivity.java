/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import static com.huawei.cloudphone.dialog.FpsTestSettingDialog.DEFAULT_TEST_TIME_MINUTE;
import static com.huawei.cloudphonesdk.audio.record.AudioRecordService.timeInterval;
import static com.huawei.cloudphonesdk.maincontrol.Constant.VMI_720P_HEIGHT;
import static com.huawei.cloudphonesdk.maincontrol.Constant.VMI_720P_WIDTH;
import static com.huawei.cloudphonesdk.maincontrol.Constant.VMI_ENGINE_EVENT_READY;
import static com.huawei.cloudphonesdk.maincontrol.Constant.VMI_VIDEO_ENGINE_EVENT_ORIENTATION_CHANGED;
import static com.huawei.cloudphonesdk.maincontrol.Constant.VMI_VIDEO_ENGINE_EVENT_SOCK_DISCONN;
import static com.huawei.cloudphonesdk.maincontrol.OpenGLJniWrapper.getEncodeParam;
import static com.huawei.cloudphonesdk.maincontrol.UpstreamReceiveDispatcher.MAX_TIME_INTERVAL;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.net.TrafficStats;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Range;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudgame.touch.VmiTouch;
import com.huawei.cloudphone.BuildConfig;
import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.dialog.AudioPlayParamsDialog;
import com.huawei.cloudphone.dialog.EncodeDialog;
import com.huawei.cloudphone.dialog.ExitDialog;
import com.huawei.cloudphone.dialog.FpsTestReportDialog;
import com.huawei.cloudphone.dialog.FpsTestSettingDialog;
import com.huawei.cloudphone.helper.FloatingHelper;
import com.huawei.cloudphone.model.UserBean;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphone.util.ViewUtil;
import com.huawei.cloudphone.widget.StatisticLayout;
import com.huawei.cloudphone.widget.VmiSurfaceView;
import com.huawei.cloudphonesdk.audio.play.AudioPlayerCallback;
import com.huawei.cloudphonesdk.audio.record.AudioDataListener;
import com.huawei.cloudphonesdk.audio.record.AudioRecordService;
import com.huawei.cloudphonesdk.fps.FpsTestCallBack;
import com.huawei.cloudphonesdk.gps.LocationCallBack;
import com.huawei.cloudphonesdk.gps.LocationHelper;
import com.huawei.cloudphonesdk.gps.VmiLocation;
import com.huawei.cloudphonesdk.maincontrol.Constant;
import com.huawei.cloudphonesdk.maincontrol.DataPipe;
import com.huawei.cloudphonesdk.maincontrol.NativeListener;
import com.huawei.cloudphonesdk.maincontrol.NetConfig;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniCallback;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniWrapper;
import com.huawei.cloudphonesdk.maincontrol.UpstreamReceiveDispatcher;
import com.huawei.cloudphonesdk.maincontrol.VideoConf;
import com.huawei.cloudphonesdk.maincontrol.config.EncodeParams;
import com.huawei.cloudphonesdk.maincontrol.config.VmiConfigAudio;
import com.huawei.cloudphonesdk.maincontrol.config.VmiConfigVideo;
import com.huawei.cloudphonesdk.sensor.SensorCallBack;
import com.huawei.cloudphonesdk.sensor.SensorService;
import com.huawei.cloudphonesdk.sensor.VmiSensor;
import com.huawei.cloudphonesdk.utils.LogUtil;
import com.huawei.cloudphonesdk.utils.ThreadPool;
import com.huawei.cloudphonesdk.video.EncodeParamsCallback;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.text.DecimalFormat;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Locale;

/**
 * An example full-screen activity that shows and hides the system UI
 *
 * @since 2017-12-22
 */
public class FullscreenActivity extends BaseActivity implements NativeListener {
    private static final String TAG = FullscreenActivity.class.getSimpleName();

    /**
     * 最大重连的次数
     */
    private static final int MAX_RECONNECT_TIMES = 3;

    /**
     * 任务标识,检测连接状态的任务触发
     */
    private static final int WHAT_CHECK_CONNECT_STATUS = 1;

    /**
     * 任务标识,重连的任务触发
     */
    private static final int WHAT_RECONNECT = 2;

    /**
     * 任务标识,连接成功(包括重连)触发
     */
    private static final int WHAT_CONNECT_SUCCESS = 3;

    /**
     * 任务标识,连接失败
     */
    private static final int WHAT_CONNECT_FAILED = 4;

    /**
     * 任务标识,重连失败
     */
    private static final int WHAT_RECONNECT_FAILED = 5;

    /**
     * 任务标识,分辨率不支持弹窗
     */
    private static final int WHAT_UNSUPPORTED_RESOLUTION = 6;

    /**
     * 任务标识,触控延时统计
     */
    private static final int WHAT_CALCULATE_TIME = 7;

    /**
     * 定时任务:每隔1s检测一次连接状态
     */
    private static final int INTERVAL_CHECK_CONNECT_STATUS = 1000;

    /**
     * 定时任务:断线状态下每隔8s执行1次重连
     */
    private static final int INTERVAL_RECONNECT = 8000;
    private static final int DELAT_TIME = 500;
    private static final int DELAY_BLACK_SCREEN = 2000;
    private static final int MAX_LENGTH = 9;
    private static final int LAG_INVALID = -1;
    private static final int UNIT_TIME = 1000;
    private static final int BYTE_UNIT = 1024;
    private static final int OFFSET = 4;
    private static final String MONBOX_MODEL = "Monbox";
    private static final String KBOX_MODEL = "Kbox";
    private static final int INITIAL_VALUE_ZERO = 0;
    private static final int GREEN_WIFI_MAX_DELAY = 80;
    private static final int YELLOW_WIFI_MAX_DELAY = 160;
    public static final int START_MIC = 0x3020001;
    public static final int STOP_MIC = 0x3020002;
    public static final int MINUTE_TO_SECOND = 60;
    public static final int FPS_TEST_CMD_LENTH = 1;
    public static final int FPS_TEST_FAIL_FLAG = -1;
    public static final int FPS_TEST_START_FLAG = 1;
    public static final int FPS_TEST_STOP_FLAG = 0;
    public static final int FPS_MAX_DELAY_SECONDS = 5;
    public static final int TEN_MINUTE_TO_SECOND = 600;

    // 数据分发器
    private UpstreamReceiveDispatcher upstreamReceiveDispatcher;

    // 带宽统计,分别为接收字节数和发送字节数
    private long mLastTxBytes;
    private long mLastRxBytes;

    // 记录重连的次数
    private int mReconnectTimes;

    // 图像显示和帧率显示控件
    private VmiSurfaceView mSurfaceView;
    private TextView mFrameRateTextView;
    private StatisticLayout mStatisticLayout;

    // 视频流参数配置
    private VideoConf videoConf;

    // 重连弹窗,分辨率不支持弹窗,退出弹窗
    private ProgressDialog mWaitingDialog;
    private AlertDialog mUnsupportedResolutionDialog;
    private ExitDialog mExitDialog;
    private AlertDialog mLagDialog;
    private EditText mEditText;

    // 标识分辨率是否支持
    private volatile boolean mResolutionSupported = true;

    // 悬浮按钮,传感器,定位,触控按键辅助类
    private FloatingHelper floatingHelper;
    private SensorService mSensorService;
    private LocationHelper mLocationHelper;

    // 线程池管理各个线程
    private ThreadPool mThreadPool = new ThreadPool(4, 20);

    // 用户信息
    private final UserBean userBean = MyApplication.instance.getUserBean();
    private Range<Integer> mWidthRange;
    private Range<Integer> mHeightRange;
    private volatile boolean mTouched;
    private ImageView flashNormal;
    private ImageView flashLandscape;
    private boolean isSimulator = false;
    private int engineOrientation;
    int oritentation = -1;
    DisplayMetrics metric = new DisplayMetrics();
    private int guestWidth = 1080;
    private int guestHeight = 1920;
    private float displayWidth = 1080.0f;

    /**
     * display height.
     */
    private float displayHeight = 1920.0f;
    public float inputXScale = 1.0f;

    /**
     * input Y scale.
     */
    public float inputYScale = 1.0f;

    /**
     * vmi display width.
     */
    private int vmWidth = 720;

    /**
     * vmi display height.
     */
    private int vmHeight = 1280;

    /**
     * 处理定时任务的Handler
     */
    private ThreadHandler mThreadHandler;
    private boolean stopHandlerFlag = false;
    private long lastTotalTraffic;
    private AudioRecordService.AudioBinder binder;
    boolean isFpsTestRunning = false;

    public ServiceConnection audioConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            binder = (AudioRecordService.AudioBinder) service;
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
        }
    };
    private EncodeDialog encodeDialog;
    private AudioPlayParamsDialog audioPlayParamsDialog;
    private AlertDialog fpsTestStartFailedDialog;
    private boolean simulatorMicFlag = false;
    private boolean isMicStarted = false;
    private OpenGLJniCallback callback;
    private FpsTestCallBack fpsTestCallBack;
    private EncodeParamsCallback encodeParamsCallback;
    private InnerBroadcastReceiver innerBroadcastReceiver;
    private FpsTestReportDialog fpsTestReportDialog;
    private FpsTestSettingDialog fpsTestSettingDialog;
    private ArrayList<Integer> fpsArray = new ArrayList<>();
    private ArrayList<Integer> jankArray = new ArrayList<>();
    private ArrayList<Integer> bJankArray = new ArrayList<>();
    private LocalDateTime startTestTime;
    private LocalDateTime stopTestTime;
    private int renderFps = 0;
    private int lastJank = 0;
    private int lastBJank = 0;
    private int seconds = 600;
    private StringBuilder writeFileBuilder = new StringBuilder();
    private StringBuilder writelogBuilder = new StringBuilder();
    private StringBuilder statusBuilder = new StringBuilder();
    private StringBuilder statusBuilderWithRenderFps = new StringBuilder();
    private String fileName = "";
    private Runnable checkFpsTestStart = new Runnable() {
        @Override
        public void run() {
            if (isFpsTestRunning && fpsArray.size() == 0) {
                Log.e(TAG, "Fps test failed, the startup time exceeds 5s.");
                showFpsTestStartFailedDialog();
            }
        }
    };

    public void sendKeyEvent(int keycode, int action) {
        final int SEND_KEY_CMD = 0x2020001;
        final int KEY_EVENT_DATA_LENGTH = 8;

        byte[] data = new byte[KEY_EVENT_DATA_LENGTH];
        // cmd
        data[0] = (byte) (SEND_KEY_CMD);
        data[1] = (byte) (SEND_KEY_CMD >> 8);
        data[2] = (byte) (SEND_KEY_CMD >> 16);
        data[3] = (byte) (SEND_KEY_CMD >> 24);
        // key event
        data[4] = (byte) keycode;
        data[5] = (byte) (keycode >> 8);
        data[6] = (byte) action;
        data[7] = (byte) (action >> 8);

        OpenGLJniWrapper.sendKeyEvent(data, data.length);
    }

    @Override
    public void onVmiVideoEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3, String additionInfo) {
        switch (event) {
            case VMI_ENGINE_EVENT_READY:
                LogUtil.debug(TAG, "onVmiVideoEngineEvent： 第一帧出流");
                runOnUiThread(() -> {
                    flashLandscape.setVisibility(View.INVISIBLE);
                    flashNormal.setVisibility(View.INVISIBLE);
                });
            case VMI_VIDEO_ENGINE_EVENT_SOCK_DISCONN:
                Log.d(TAG, "OnVmiVideoEngineEvent: 网络断开回调");
                runOnUiThread(() -> {
                    stopHandlerFlag = true;
                    flashLandscape.setVisibility(View.VISIBLE);
                    flashNormal.setVisibility(View.VISIBLE);
                    showReconnectDialog();
                });
                stopMonitor();
                // 移除传感器、位置监听
                unregisterListener();
                OpenGLJniWrapper.stop();
                break;
            case VMI_VIDEO_ENGINE_EVENT_ORIENTATION_CHANGED:
                engineOrientation = reserved0;
                runOnUiThread(() -> {
                    setRotation(engineOrientation);
                });
                break;
            case Constant.VMI_ENGINE_EVENT_GET_VERSION_TIMEOUT:
            case Constant.VMI_ENGINE_EVENT_VERSION_MISMATCH:
            case Constant.VMI_ENGINE_EVENT_ENGINE_MISMATCH:
                runOnUiThread(() -> {
                    stopMonitor();
                    OpenGLJniWrapper.stop();
                    AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
                    if (event == Constant.VMI_ENGINE_EVENT_GET_VERSION_TIMEOUT) {
                        builder.setMessage("获取版本号信息超时。原因可能是:" + "\n1.端口已被连接。" +
                                "\n2.服务端使用了旧版本，客户端版本号：" + BuildConfig.VERSION_NAME + "。");
                    } else {
                        String delimeter = "\n";
                        String[] splitErrorInfo = additionInfo.split(delimeter);
                        if (splitErrorInfo.length == 2) {
                            if (event == Constant.VMI_ENGINE_EVENT_VERSION_MISMATCH) {
                                builder.setMessage("服务端和客户端版本不匹配\n" + "服务端版本号：" + splitErrorInfo[0] +
                                        "\n客户端版本号：" + splitErrorInfo[1]);
                            } else if (event == Constant.VMI_ENGINE_EVENT_ENGINE_MISMATCH) {
                                builder.setMessage("服务端和客户端引擎不匹配\n" + "服务端引擎：" + splitErrorInfo[0] +
                                        "\n客户端引擎：" + splitErrorInfo[1]);
                            }
                            LogUtil.info(TAG, splitErrorInfo[0]);
                            LogUtil.info(TAG, splitErrorInfo[1]);
                        } else {
                            builder.setMessage("服务端和客户端版本不匹配\n" + "错误详情：" + additionInfo);
                        }
                    }
                    builder.setTitle(getStringFromRes(R.string.warning));
                    builder.setNegativeButton(getStringFromRes(R.string.exit), new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    });
                    builder.setCancelable(false);
                    builder.show();
                });
                break;
            case START_MIC:
                runOnUiThread(() -> {
                    if (isMicStarted) {
                        showLongToast("麦克风已启动");
                        return;
                    }
                    showLongToast("正在启动麦克风");
                    isMicStarted = true;
                    if (isSimulator) {
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                simulatorMicFlag = true;
                                readFromByteFile();
                            }
                        }).start();
                        return;
                    }
                    realPhoneStartMic();
                });
                break;
            case STOP_MIC:
                if (binder != null) {
                    binder.getService().stop();
                    showLongToast("正在停止麦克风");
                } else {
                    showLongToast("麦克风已停止");
                }
                isMicStarted = false;
                if (isSimulator) {
                    simulatorMicFlag = false;
                }
                break;
            default:
        }
    }

    private void realPhoneStartMic() {
        applyMicPermission();
        Intent intent = new Intent(FullscreenActivity.this, AudioRecordService.class);
        intent.putExtra("bitrate", videoConf.getBitrate());
        boolean result = bindService(intent, audioConnection, Context.BIND_AUTO_CREATE);
        Log.d(TAG, "start mic service :" + result);
        int audioType = videoConf.getMicStreamType();
        int sampleInterval = videoConf.getMicSampleInterval();
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (binder == null) {
                    try {
                        Thread.sleep(timeInterval);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    Log.d(TAG, "run:麦克风正在启动... ");
                    if (binder != null) {
                        break;
                    }
                }
                showLongToast("麦克风启动成功");
                int bitrate = videoConf.getBitrate();
                if (bitrate >= 500 && bitrate <= 512000) {
                    binder.getService().startRecord(bitrate, audioType, sampleInterval);
                } else {
                    binder.getService().startRecord(192000, audioType, sampleInterval);
                }
                binder.getService().setAudioDataListener(new AudioDataListener() {
                    @Override
                    public void onAudioDataReceived(byte[] bytes) {
                        boolean result = OpenGLJniWrapper.composeMicData(bytes, bytes.length, audioType, sampleInterval);
                        Log.d(TAG, "run: 正在发送麦克风数据:" + bytes.length + ",结果：" + result);
                    }
                });
            }
        }).start();
    }

    private void showLongToast(String msg) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(FullscreenActivity.this, msg, Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void readFromByteFile() {
        int audioType = videoConf.getMicStreamType();
        int sampleInterval = videoConf.getMicSampleInterval();
        String inputFilePath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/input.opus";
        String inputPcmFilePath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/input";
        File filename = new File(audioType == 0 ? inputFilePath : inputPcmFilePath);
        if (!filename.exists()) {
            Log.e(TAG, "readFromByteFile: 仿真机发送本地" + filename.getAbsolutePath() + "文件,文件不存在" + ",audioType:" + audioType);
            return;
        }
        BufferedInputStream in = null;
        while (simulatorMicFlag && isMicStarted) {
            try {
                in = new BufferedInputStream(new FileInputStream(filename));
                // 每次从麦克风获取的原始数据=采样率*时间间隔（s）*声道数*位深/8=48000*(sampleInterval/1000)*2*16/8=192*sampleInterval
                // opus编码后数组大小=码率*时间间隔(s)/8
                int arrayLen = (int) ((audioType == 0) ? (videoConf.getBitrate() * sampleInterval / 1000 / 8) : 192 * sampleInterval);
                byte[] temp = new byte[arrayLen];
                int size = 0;
                int index = 0;
                long startTime = System.currentTimeMillis();
                while (((size = in.read(temp)) != -1) && isMicStarted) {
                    boolean result = OpenGLJniWrapper.composeMicData(temp, temp.length, audioType, sampleInterval);
                    delayCompensation(startTime, index);
                    index++;
                    Log.d(TAG, "run: 正在发送麦克风数据:" + size + ",结果：" + result);
                }
            } catch (IOException e) {
                Log.e(TAG, "readFromByteFile: " + e.getMessage());
            }
        }
    }

    private void delayCompensation(long startTime, int index) {
        long currentTime = System.currentTimeMillis();
        long value = currentTime - startTime - index * 10L;
        Log.d(TAG, "delayCompensation: currentTime:" + currentTime + ",startTime:" + startTime + ",value:" + value + ",index:" + index);
        if (value >= 0 && value < timeInterval) {
            try {
                Thread.sleep(timeInterval - value);
            } catch (InterruptedException e) {
                Log.e(TAG, "delayCompensation: " + e.getMessage());
            }
        }
    }


    private void applyMicPermission() {
        boolean recordPermission = ContextCompat.checkSelfPermission(this,
                Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED;
        if (!recordPermission) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.RECORD_AUDIO
                    }, 89);
        }

    }

    private void setRotation(int rotation) {
        final int rota = rotation;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (oritentation != rota) {
                    LogUtil.info(TAG, "client begin rotate: " + oritentation + " ---> " + rota);
                    oritentation = rota;
                    switch (oritentation) {
                        case Surface.ROTATION_0:
                            mSurfaceView.setScreenRotation(VmiSurfaceView.Rotation.ROTATION0);
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                            break;
                        case Surface.ROTATION_90:
                            mSurfaceView.setScreenRotation(VmiSurfaceView.Rotation.ROTATION90);
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                            break;
                        case Surface.ROTATION_180:
                            mSurfaceView.setScreenRotation(VmiSurfaceView.Rotation.ROTATION180);
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT);
                            break;
                        case Surface.ROTATION_270:
                            mSurfaceView.setScreenRotation(VmiSurfaceView.Rotation.ROTATION270);
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
                            break;
                        default:
                            break;
                    }
                }
            }
        });
    }

    private void checkSimulator() {
        if (Build.MODEL.contains(MONBOX_MODEL) || Build.MODEL.contains(KBOX_MODEL)) {
            isSimulator = true;
        }
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        LogUtil.startLogs(LogUtil.CLIENT_LOG);
        super.onCreate(savedInstanceState);
        innerBroadcastReceiver = new InnerBroadcastReceiver();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("receive_adb_data");
        registerReceiver(innerBroadcastReceiver, intentFilter);
        // 在这里初始化SensorService
        mSensorService = new SensorService(this);
        VmiSensor.getInstance().setSensorService(mSensorService);
        // 在这里初始化LocationHelper
        mLocationHelper = new LocationHelper(this);
        VmiLocation.getInstance().setLocationHelper(mLocationHelper);
    }

    private void showReconnectDialog() {
        Intent intent = new Intent(this, ReconnectActivity.class);
        startActivity(intent);
        finish();
    }

    public void showFpsTestSettingDialog() {
        if (fpsTestSettingDialog == null) {
            fpsTestSettingDialog = new FpsTestSettingDialog(mActivity);
        }
        fpsTestSettingDialog.inputEd.setText(SPUtil.getString(SPUtil.TIME_MINUTE, DEFAULT_TEST_TIME_MINUTE));
        fpsTestSettingDialog.setPositiveButton(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (ClickUtil.isFastClick()) {
                    return;
                }
                if (isFpsTestRunning) {
                    isFpsTestRunning = false;
                    mThreadHandler.removeCallbacks(checkFpsTestStart);
                    fpsTestSettingDialog.positionTextView.setText("开始测试");
                    computeDate();
                } else {
                    if (TextUtils.isEmpty(fpsTestSettingDialog.inputEd.getText().toString())) {
                        ToastUtil.showToast("请设置测试时间!");
                        return;
                    } else {
                        seconds = Integer.parseInt(fpsTestSettingDialog.inputEd.getText().toString()) * MINUTE_TO_SECOND;
                        SPUtil.putString(SPUtil.TIME_MINUTE, fpsTestSettingDialog.inputEd.getText().toString());
                    }
                    isFpsTestRunning = true;
                    lastJank = 0;
                    lastBJank = 0;
                    OpenGLJniWrapper.sendFpsTestCommand(new byte[]{FPS_TEST_START_FLAG}, 1);
                    fpsTestSettingDialog.positionTextView.setText("停止测试");
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                        startTestTime = LocalDateTime.now();
                        mThreadHandler.postDelayed(checkFpsTestStart, MAX_TIME_INTERVAL);
                        fpsTestSettingDialog.dismiss();
                        ToastUtil.showToast("正在进行卡顿率测试...");
                    }
                }
            }
        }).setNegativeButton(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fpsTestSettingDialog.dismiss();
            }
        });
        if (!fpsTestSettingDialog.isShowing()) {
            fpsTestSettingDialog.show();
        }
    }

    private String getStringFromRes(int resId) {
        return getResources().getString(resId);
    }

    /**
     * Handler
     */
    static class ThreadHandler extends Handler {
        private WeakReference<FullscreenActivity> weakReference;

        ThreadHandler(FullscreenActivity activity) {
            this.weakReference = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            FullscreenActivity activity = weakReference.get();
            if (activity == null || activity.isFinishing()) {
                return;
            }
            switch (msg.what) {
                case WHAT_CHECK_CONNECT_STATUS: { // 检测连接状态
                    if (!activity.stopHandlerFlag) {
                        activity.checkConnectStatus();
                    }
                    break;
                }
                case WHAT_RECONNECT: { // 重连
                    break;
                }
                case WHAT_CONNECT_SUCCESS: { // 连接成功(包括重连成功)
                    if (!activity.stopHandlerFlag) {
                        activity.startMonitor();
                    }
                    break;
                }
                case WHAT_CONNECT_FAILED: { // 连接失败
                    if (activity.mResolutionSupported) {
                        activity.showConnectFailedDialog();
                    }
                    break;
                }
                case WHAT_RECONNECT_FAILED: { // 重连失败
                    if (activity.mResolutionSupported) {
                        activity.showCannotReconnectDialog();
                    }
                    break;
                }
                case WHAT_UNSUPPORTED_RESOLUTION: { // 弹窗分辨率不支持弹窗
                    activity.showUnsupportedResolutionDialog();
                    break;
                }
                case WHAT_CALCULATE_TIME:
                    activity.mTouched = false;
                    activity.mStatisticLayout.startDraw();
                    break;
                default: {
                    break;
                }
            }
        }
    }

    private void showFpsTestStartFailedDialog() {
        isFpsTestRunning = false;
        OpenGLJniWrapper.sendFpsTestCommand(new byte[]{FPS_TEST_STOP_FLAG}, FPS_TEST_CMD_LENTH);
        if (fpsTestSettingDialog != null) {
            fpsTestSettingDialog.positionTextView.setText("开始测试");
        }
        if (fpsTestStartFailedDialog == null) {
            fpsTestStartFailedDialog = new AlertDialog.Builder(this)
                    .setTitle("温馨提示")
                    .setMessage("卡顿率测试启动失败,建议重新开始测试!")
                    .setNegativeButton(R.string.ensure, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int which) {
                            dialogInterface.dismiss();
                        }
                    })
                    .setCancelable(false)
                    .create();
            fpsTestStartFailedDialog.setCanceledOnTouchOutside(false);
        }
        if (!fpsTestStartFailedDialog.isShowing()) {
            fpsTestStartFailedDialog.show();
        }
    }

    @Override
    protected int getLayoutRes() {
        if (!(SPUtil.getBoolean(SPUtil.IS_SCREENSHOT, true))) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_SECURE);
        }
        mThreadHandler = new ThreadHandler(this);
        setFullScreen();
        return R.layout.vmi_activity_fullscreen;
    }

    @Override
    protected void initView() {
        setupViews();
        setupConfig();
        setupSurfaceHolder();
        setupTouch();
        initDataPipe();
        checkSimulator();
        setupFloatingButton();
    }

    public void startMicThread() {
        onVmiVideoEngineEvent(START_MIC, 0, 0, 0, 0, "");
    }

    public void stopMicThread() {
        onVmiVideoEngineEvent(STOP_MIC, 0, 0, 0, 0, "");
    }

    /**
     * 提示分辨率不支持
     */
    private void showUnsupportedResolutionDialog() {
        if (mUnsupportedResolutionDialog == null) {
            mUnsupportedResolutionDialog = new AlertDialog.Builder(this)
                    .setTitle("温馨提示")
                    .setMessage("当前分辨率不支持，无法使用!")
                    .setNegativeButton(R.string.ensure, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int which) {
                            dialogInterface.dismiss();
                            finish();
                        }
                    })
                    .setCancelable(false)
                    .create();
            mUnsupportedResolutionDialog.setCanceledOnTouchOutside(false);
        }
        if (!mUnsupportedResolutionDialog.isShowing()) {
            mUnsupportedResolutionDialog.show();
        }
    }

    @Override
    protected void setListener() {
        fpsTestCallBack = new FpsTestCallBack();
        fpsTestCallBack.setNativeListener(this);
        encodeParamsCallback = new EncodeParamsCallback();
        encodeParamsCallback.setNativeListener(this);
        callback = new OpenGLJniCallback();
        callback.setObj();
        callback.setNativeListener(this);
        callback.setJniCallback(new OpenGLJniCallback.JniCallback() {
            @Override
            public void onResolutionUnsupported() {
                mResolutionSupported = false;
                LogUtil.error(TAG, "resolution unsupported !");
                mThreadHandler.sendEmptyMessage(WHAT_UNSUPPORTED_RESOLUTION);
            }

            @Override
            public void onFrameChanged(int frameType, int frameSize, long timeout, long timestamp) {
                if (mStatisticLayout.getVisibility() != View.VISIBLE) {
                    return;
                }
                LogUtil.info(TAG, "onFrameChanged-> frameType:" + frameType + "; frameSize:"
                        + frameSize + "; timeout:" + timeout + "; timestamp:" + timestamp);
                mStatisticLayout.refreshFrameData(frameType, frameSize, (int) timeout);
                if (mTouched) {
                    mStatisticLayout.refreshFrameTime(frameType, frameSize, timestamp / 1000);
                }
            }
        });
    }

    @Override
    public void onVmiFpsDataReceive(int fps, int jank, int bjank) {
        if (!isFpsTestRunning) {
            return;
        }
        if (fps == FPS_TEST_FAIL_FLAG && jank == FPS_TEST_FAIL_FLAG && bjank == FPS_TEST_FAIL_FLAG) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                        // 只有按下按钮5秒后才会报错，避免误报
                        if (LocalDateTime.now().toEpochSecond(ZoneOffset.of("+8")) -
                                startTestTime.toEpochSecond(ZoneOffset.of("+8")) > FPS_MAX_DELAY_SECONDS) {
                            Log.e(TAG, "Fps test failed, the interval between two data records exceeds 5s.");
                            showFpsTestStartFailedDialog();
                        }
                    }
                }
            });
            return;
        }
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            fileName = startTestTime.format(DateTimeFormatter
                    .ofPattern("yyyy-MM-dd-HH-mm-ss")) + "_videoFreezeRate.txt";
            LocalDateTime currentTime = LocalDateTime.now();
            long currentTimeBySecond = currentTime.toEpochSecond(ZoneOffset.of("+8"));
            String timeInfo = currentTime.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            renderFps = fps;
            fpsArray.add(fps);
            jankArray.add(jank - lastJank);
            bJankArray.add(bjank - lastBJank);
            writeFileBuilder.append(timeInfo).append(" fps:").append(fps).append(" jank:")
                    .append(jank - lastJank).append(" bjank:").append(bjank - lastBJank);
            LogUtil.writeFpsData("fps", fileName, writeFileBuilder.toString());
            writelogBuilder.append(" fps:").append(fps).append(" jank:")
                    .append(jank - lastJank).append(" bjank:").append(bjank - lastBJank);
            Log.i(TAG, "onVmiFpsDataReceive: " + writelogBuilder);
            writelogBuilder.setLength(0);
            writeFileBuilder.setLength(0);
            lastJank = jank;
            lastBJank = bjank;
            if (currentTimeBySecond - startTestTime.toEpochSecond(ZoneOffset.of("+8")) == seconds) {
                computeDate();
            }
        }
    }

    @Override
    public void onVmiEncodeParamsReceive(EncodeParams encodeParams) {
        saveEncodeParams(encodeParams);
    }

    private void computeDate() {
        isFpsTestRunning = false;
        OpenGLJniWrapper.sendFpsTestCommand(new byte[]{FPS_TEST_STOP_FLAG}, FPS_TEST_CMD_LENTH);
        DecimalFormat df = new DecimalFormat("0.00");
        // 每秒均值
        String fpsAverage = df.format((float) fpsArray.stream().reduce(Integer::sum).orElse(0) / fpsArray.size());
        // 10min均值
        String jankAverage = df.format((float) jankArray.stream().reduce(Integer::sum).orElse(0) / jankArray.size() * TEN_MINUTE_TO_SECOND);
        // 10min均值
        String bJankAverage = df.format((float) bJankArray.stream().reduce(Integer::sum).orElse(0) / bJankArray.size() * TEN_MINUTE_TO_SECOND);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                showFpsTestReportDailog(fpsAverage, jankAverage, bJankAverage);
                fpsArray.clear();
                jankArray.clear();
                bJankArray.clear();
            }
        });
    }

    private void showFpsTestReportDailog(String fpsAverage, String jankAverage, String bJankAverage) {
        if (fpsTestSettingDialog != null) {
            fpsTestSettingDialog.positionTextView.setText("开始测试");
        }
        if (fpsTestReportDialog == null) {
            fpsTestReportDialog = new FpsTestReportDialog(mActivity)
                    .setNegativeButton(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            fpsTestReportDialog.dismiss();
                        }
                    });
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            stopTestTime = LocalDateTime.now();
            fpsTestReportDialog.averageFpsTextView.setText(fpsAverage);
            fpsTestReportDialog.averageJankTextView.setText(jankAverage);
            fpsTestReportDialog.averageBJankTextView.setText(bJankAverage);
            String startTestTimeValue = startTestTime.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            fpsTestReportDialog.startTime.setText(startTestTimeValue);
            String stopTestTimeValue = stopTestTime.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            fpsTestReportDialog.stopTime.setText(stopTestTimeValue);
            StringBuilder averageBuilder = new StringBuilder();
            averageBuilder.append("测试开始时间: ").append(startTestTimeValue).append("  测试结束时间: ").append(stopTestTimeValue).append(
                            "  帧率平均值: ").append(fpsAverage).append("  jank/10min平均值: ").append(jankAverage).append("  bjank/10min平均值: ")
                    .append(bJankAverage);
            LogUtil.writeFpsData("fps", fileName, averageBuilder.toString());
        }
        if (!fpsTestReportDialog.isShowing()) {
            fpsTestReportDialog.show();
        }
    }

    /**
     * 初始化悬浮按钮
     */
    private void setupFloatingButton() {
        floatingHelper = new FloatingHelper(this);
        floatingHelper.init();
        ViewUtil.setVisibility(mFrameRateTextView, View.VISIBLE);
    }

    /**
     * 设置全屏显示.
     */
    private void setFullScreen() {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        Window window = getWindow();
        WindowManager.LayoutParams params = window.getAttributes();
        params.systemUiVisibility = View.SYSTEM_UI_FLAG_LOW_PROFILE;
        window.setAttributes(params);
        int vis = window.getDecorView().getSystemUiVisibility();
        window.getDecorView().setSystemUiVisibility(vis | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE);
        WindowManager wm = (WindowManager) getApplicationContext().getSystemService(WINDOW_SERVICE);
        wm.getDefaultDisplay().getRealMetrics(metric);
        LogUtil.info(TAG, "width pixels:" + metric.widthPixels + ", height pixels:" + metric.heightPixels
                + ",densityDpi:" + metric.densityDpi);
        guestWidth = metric.widthPixels;
        guestHeight = metric.heightPixels;
    }

    /**
     * 初始化配置参数
     */
    private void setupConfig() {
        Intent intent = getIntent();
        if (intent != null) {
            videoConf = intent.getParcelableExtra("video_conf");
            // 分辨率
            DisplayMetrics metrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
            videoConf.setWidth(Math.min(metrics.widthPixels, metrics.heightPixels));
            videoConf.setHeight(Math.max(metrics.widthPixels, metrics.heightPixels));
            videoConf.setDensity(metrics.densityDpi);
        }
    }

    /**
     * 仅自适应分辨装开关打开时生效
     */
    private void startModule() {
        if (!SPUtil.getBoolean(SPUtil.VIDEO_ADAPTIVE_RESOLUTION, Constant.VMI_ADAPTIVE_RESOLUTION)) {
            LogUtil.info(TAG, "disable adaptive resolution, startModule skip");
            return;
        }

        // VIDEO MODULE
        VmiConfigVideo vmiConfigVideo = new VmiConfigVideo();
        vmiConfigVideo.setEncoderType(SPUtil.getInt(SPUtil.VIDEO_ENCODER_TYPE, 0));
        vmiConfigVideo.setVideoFrameType(SPUtil.getInt(SPUtil.VIDEO_FRAME_TYPE, 0));
        vmiConfigVideo.setFrameRate(SPUtil.getInt(SPUtil.VIDEO_FRAME_RATE, 30));// 暂不使用
        vmiConfigVideo.setForceLandscape(SPUtil.getBoolean(SPUtil.VIDEO_FORCE_LANDSCAPE, false));// 暂不使用
        // FrameSize
        vmiConfigVideo.setWidth(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH, VMI_720P_WIDTH));
        vmiConfigVideo.setHeight(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT, VMI_720P_HEIGHT));
        vmiConfigVideo.setWidthAligned(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH_ALIGNED, VMI_720P_WIDTH));
        vmiConfigVideo.setHeightAligned(SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT_ALIGNED, VMI_720P_HEIGHT));
        vmiConfigVideo.setDensity(SPUtil.getInt(SPUtil.VIDEO_DENSITY, metric.densityDpi));
        vmiConfigVideo.setRenderOptimize(SPUtil.getBoolean(SPUtil.VIDEO_RENDER_OPTIMIZE, true));
        // EncodeParams
        vmiConfigVideo.setBitrate(SPUtil.getInt(SPUtil.VIDEO_BIT_RATE, 3000000));
        vmiConfigVideo.setGopSize(SPUtil.getInt(SPUtil.VIDEO_GOP_SIZE, 30));
        vmiConfigVideo.setProfile(SPUtil.getInt(SPUtil.VIDEO_PROFILE, 1));
        vmiConfigVideo.setRcMode(SPUtil.getInt(SPUtil.VIDEO_RC_MODE, 2));
        vmiConfigVideo.setForceKeyFrame(SPUtil.getInt(SPUtil.VIDEO_FORCE_KEY_FRAME, 0));
        vmiConfigVideo.setInterpolation(SPUtil.getInt(SPUtil.VIDEO_INTERPOLATION, 0) != 0); // 沿用原来的int类型，以后可考虑改为bool
        vmiConfigVideo.setCrf(SPUtil.getInt(SPUtil.VIDEO_CRF, 34));
        vmiConfigVideo.setMaxCrfRate(SPUtil.getInt(SPUtil.VIDEO_MAX_CRF_RATE, 20000000));
        vmiConfigVideo.setVbvBufferSize(SPUtil.getInt(SPUtil.VIDEO_VBV_BUFFER_SIZE, 1000));
        vmiConfigVideo.setStreamWidth(SPUtil.getInt(SPUtil.VIDEO_STREAM_WIDTH, Constant.VMI_720P_WIDTH));
        vmiConfigVideo.setStreamHeight(SPUtil.getInt(SPUtil.VIDEO_STREAM_HEIGHT, Constant.VMI_720P_HEIGHT));
        LogUtil.error(TAG, "streamWidth" + vmiConfigVideo.getStreamWidth());
        OpenGLJniWrapper.startVideo(vmiConfigVideo);
        LogUtil.info(TAG, "startModule success");
    }

    /**
     * 初始化各个控件
     */
    private void setupViews() {
        mStatisticLayout = CastUtil.toStatisticLayout(findViewById(R.id.statistic));
        mFrameRateTextView = CastUtil.toTextView(findViewById(R.id.textRate));
        if (!SPUtil.getBoolean(SPUtil.KEY_SHOW_FPS, false)) {
            ViewUtil.setVisibility(mFrameRateTextView, View.GONE);
        } else {
            ViewUtil.setVisibility(mFrameRateTextView, View.VISIBLE);
        }
        mSurfaceView = CastUtil.toVmiSurfaceView(findViewById(R.id.vmi_surfaceView));
        flashNormal = CastUtil.toImageView(findViewById(R.id.flashNormal));
        flashLandscape = CastUtil.toImageView(findViewById(R.id.flashLandscape));
    }

    private void initDataPipe() {
        if (videoConf == null) {
            LogUtil.info(TAG, "InitDataPipe Fail: mVideoConf is null");
            finish();
            return;
        }
        if (videoConf.isTestTouch()) {
            DataPipe.setTestVmiTouchJava(true);
        }
        if (videoConf.isTestAudioClient()) {
            DataPipe.setTestAudioClient(true);
        }
        DataPipe.registerHookToTouch();
        DataPipe.registerHookToAudio();
    }

    @SuppressLint("ClickableViewAccessibility")
    private void setupTouch() {
        mSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_DOWN
                        && mStatisticLayout.getVisibility() == View.VISIBLE
                        && !mThreadHandler.hasMessages(WHAT_CALCULATE_TIME)) {
                    startCalculateTime(System.currentTimeMillis());
                }
                floatingHelper.closeFloatButton();
                VmiTouch.getInstance().onTouch(view, event);
                return true;
            }
        });
    }

    /**
     * 开始统计延迟
     *
     * @param eventTime 触控时间
     */
    private void startCalculateTime(long eventTime) {
        mTouched = true;
        mStatisticLayout.setTouchTime(eventTime);
        mThreadHandler.sendEmptyMessageDelayed(WHAT_CALCULATE_TIME, 1000);
    }

    private void setupMediaCodec() {
        if (mWidthRange == null || mHeightRange == null) {
            MediaCodec mediaCodec = null;
            try {
                mediaCodec = MediaCodec.createDecoderByType("video/avc");
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (mediaCodec == null) {
                LogUtil.error("tag", "mediaCodec null");
                return;
            }
            MediaCodecInfo codecInfo = mediaCodec.getCodecInfo();
            MediaCodecInfo.CodecCapabilities capabilities = codecInfo.getCapabilitiesForType("video/avc");
            if (capabilities == null) {
                LogUtil.error("tag", "capabilities null");
                return;
            }
            MediaCodecInfo.VideoCapabilities videoCapabilities = capabilities.getVideoCapabilities();
            if (videoCapabilities == null) {
                LogUtil.error("tag", "videoCapabilities null");
                return;
            }
            mWidthRange = videoCapabilities.getSupportedWidths();
            mHeightRange = videoCapabilities.getSupportedHeights();
        }
    }

    /**
     * 初始化SurfaceHolder
     */
    private void setupSurfaceHolder() {
        SurfaceHolder surfaceHolder = mSurfaceView.getHolder();
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mSurfaceView.getHolder().setFixedSize(guestWidth, guestHeight);
        mSurfaceView.initialize(guestWidth, guestHeight);
        initLayoutChangeListener();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                LogUtil.info(TAG, "surfaceCreated");
                startConnect();
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                LogUtil.info(TAG, "surfaceChanged");
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                LogUtil.info(TAG, "surfaceDestroyed");
                stopMonitor();
                OpenGLJniWrapper.stop();
            }
        });
    }

    /**
     * 开启工作任务
     */
    private void startMonitor() {
        LogUtil.debug(TAG, "startMonitor");
        // 隐藏重连弹窗
        dismissWaitingDialog();
        // 开启音频播放功能
        int ret = AudioTrackPlayer.startAudio();
        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            LogUtil.info(TAG, "VMI_SUCCESS start Audio success ");
        } else if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_START_FAIL) {
            LogUtil.error(TAG, "VMI_AUDIO_ENGINE_CLIENT_START_FAIL start Audio fail ");
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                flashLandscape.setVisibility(View.INVISIBLE);
                flashNormal.setVisibility(View.INVISIBLE);
            }
        });
    }

    /**
     * 停止工作任务
     */
    private void stopMonitor() {
        LogUtil.debug(TAG, "stopMonitor");
        // 移除Handler任务
        mThreadHandler.removeCallbacksAndMessages(null);
        // 停止数据处理
        if (upstreamReceiveDispatcher != null) {
            upstreamReceiveDispatcher.stopBlocked();
            upstreamReceiveDispatcher = null;
        }
        // 停止监听
        unregisterListener();
    }

    /**
     * 停止监听
     */
    private void unregisterListener() {
        // 停止传感器监听
        if (mSensorService != null) {
            mSensorService.unregisterSensorListener();
        }
        // 停止地理位置监听
        if (mLocationHelper != null) {
            mLocationHelper.unregisterLocationListener();
        }
    }

    public void showAudioPlayInputDialog() {
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
                    if (sendAudioPlayParam()) {
                        audioPlayParamsDialog.dismiss();
                    }
                }
            });
        }
        audioPlayParamsDialog.show();
    }

    private boolean sendAudioPlayParam() {
        SPUtil.putInt(SPUtil.AUDIO_PLAY_BITRATE, Integer.parseInt(audioPlayParamsDialog.getBitRate()));
        SPUtil.putInt(SPUtil.AUDIO_SAMPLE_INTERVAL, Integer.parseInt(audioPlayParamsDialog.getSampleInterval()));
        VmiConfigAudio vmiConfigAudio = new VmiConfigAudio();
        vmiConfigAudio.setSampleInterval(SPUtil.getInt(SPUtil.AUDIO_SAMPLE_INTERVAL, 10));
        vmiConfigAudio.setBitrate(SPUtil.getInt(SPUtil.AUDIO_PLAY_BITRATE, 192000));
        boolean ret = OpenGLJniWrapper.setAudioParam(vmiConfigAudio);
        if (ret) {
            Toast.makeText(this, "音频编码参数发送成功", Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(this, "音频编码参数发送失败", Toast.LENGTH_LONG).show();
        }
        return true;
    }

    private void saveEncodeParams(EncodeParams encodeParams) {
        try {
            SPUtil.putInt(SPUtil.VIDEO_BIT_RATE, encodeParams.getBitrate());
            SPUtil.putInt(SPUtil.VIDEO_GOP_SIZE, encodeParams.getGopSize());
            SPUtil.putInt(SPUtil.VIDEO_PROFILE, encodeParams.getProfile());
            SPUtil.putInt(SPUtil.VIDEO_RC_MODE, encodeParams.getRcMode());
            SPUtil.putInt(SPUtil.VIDEO_FORCE_KEY_FRAME, encodeParams.getForceKeyFrame());
            SPUtil.putBoolean(SPUtil.VIDEO_INTERPOLATION, encodeParams.isInterpolation());
            SPUtil.putInt(SPUtil.VIDEO_CRF, encodeParams.crf);
            SPUtil.putInt(SPUtil.VIDEO_MAX_CRF_RATE, encodeParams.getMaxCrfRate());
            SPUtil.putInt(SPUtil.VIDEO_VBV_BUFFER_SIZE, encodeParams.getVbvBufferSize());
            SPUtil.putInt(SPUtil.VIDEO_STREAM_WIDTH, encodeParams.getStreamWidth());
            SPUtil.putInt(SPUtil.VIDEO_STREAM_HEIGHT, encodeParams.getStreamHeight());
            LogUtil.info("Receive EncodeParams", encodeParams.toString());
        } catch (Throwable e) {
            LogUtil.error(TAG, e.toString());
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
            int sampleIntervalValue = Integer.parseInt(sampleInterval);
            if (sampleIntervalValue != 5 && sampleIntervalValue != 10 && sampleIntervalValue != 20) {
                ToastUtil.showToast("音频播放采样间隔仅支持5ms/10ms/20ms");
                return false;
            }
            SPUtil.putInt(SPUtil.AUDIO_SAMPLE_INTERVAL, sampleIntervalValue);
        } else {
            SPUtil.removeKey(SPUtil.AUDIO_SAMPLE_INTERVAL);
        }
        return true;
    }

    /**
     * 检测连接状态
     */
    private void checkConnectStatus() {
        // 连接正常,刷新帧率信息
        refreshFrameRate();
        // 定时再次检测
        mThreadHandler.sendEmptyMessageDelayed(WHAT_CHECK_CONNECT_STATUS, INTERVAL_CHECK_CONNECT_STATUS);
    }

    /**
     * 刷新帧率信息
     */
    private void refreshFrameRate() {
        int uid = getApplicationInfo().uid;
        long currentRxBytes = TrafficStats.getUidRxBytes(uid); // 接收字节数
        long currentTxBytes = TrafficStats.getUidTxBytes(uid); // 发送字节数
        long totalBytes = currentRxBytes + currentTxBytes - lastTotalTraffic;
        if (mLastRxBytes == 0 && mLastTxBytes == 0) {
            mLastRxBytes = currentRxBytes;
            mLastTxBytes = currentTxBytes;
        }
        long bandWidthBytes = (currentRxBytes - mLastRxBytes) + (currentTxBytes - mLastTxBytes);
        int bandWidth = (int) (bandWidthBytes / BYTE_UNIT);
        int trafficSum = (int) (totalBytes / BYTE_UNIT / BYTE_UNIT);
        LogUtil.info(TAG, "updateTrafficInfo: -> " + bandWidth + " - " + trafficSum);
        String stat = OpenGLJniWrapper.getStatistics();
        int lag = 0;
        if (!stat.isEmpty()) {
            String receiveFps = stat.substring(stat.indexOf("接收帧率"));
            String lagString = stat.substring(
                    stat.indexOf("LAG:") + OFFSET, stat.indexOf("ms"));
            if (lagString.length() > MAX_LENGTH) {
                lag = LAG_INVALID;
            } else {
                final int lagFromNative = Integer.parseInt(lagString.trim());
                lag = lagFromNative / UNIT_TIME;
            }
            String statString = "";
            if (isFpsTestRunning) {
                statusBuilderWithRenderFps.append("LAG: ").append(lag).append("ms").append(System.lineSeparator())
                        .append(receiveFps).append("渲染帧率：").append(renderFps).append("fps").append(System.lineSeparator())
                        .append("带宽：").append(Long.toString(bandWidthBytes / BYTE_UNIT)).append("KB").append(System.lineSeparator())
                        .append("流量：").append(Long.toString(totalBytes / BYTE_UNIT / BYTE_UNIT)).append("MB");
                statString = statusBuilderWithRenderFps.toString();
                statusBuilderWithRenderFps.setLength(0);
            } else {
                statusBuilder.append("LAG: ").append(lag).append("ms").append(System.lineSeparator())
                        .append(receiveFps).append("带宽：").append(Long.toString(bandWidthBytes / BYTE_UNIT)).append("KB")
                        .append(System.lineSeparator()).append("流量：").append(Long.toString(totalBytes / BYTE_UNIT / BYTE_UNIT))
                        .append("MB");
                statString = statusBuilder.toString();
                statusBuilder.setLength(0);
            }
            mFrameRateTextView.setText(statString);
            updateFloater(lag);
            LogUtil.info(TAG, "refreshFrameRate: ->" + statString);
        }
        mLastRxBytes = currentRxBytes;
        mLastTxBytes = currentTxBytes;
    }

    private void updateFloater(int lag) {
        if (floatingHelper == null) {
            return;
        }
        if (lag < INITIAL_VALUE_ZERO) {
            floatingHelper.setMainImage(R.drawable.image_menu_red);
        } else if (lag < GREEN_WIFI_MAX_DELAY) {
            floatingHelper.setMainImage(R.drawable.image_menu_green);
        } else if (lag < YELLOW_WIFI_MAX_DELAY) {
            floatingHelper.setMainImage(R.drawable.image_menu_yellow);
        } else {
            floatingHelper.setMainImage(R.drawable.image_menu_red);
        }
    }

    /**
     * Reset帧率信息
     */
    private void resetFrameRate() {
        String statString = "LAG : N/A" + System.lineSeparator()
                + "接收帧率 : N/A" + System.lineSeparator()
                + "解码帧率 : N/A" + System.lineSeparator()
                + "带宽 : N/A" + System.lineSeparator()
                + "流量 : N/A ";
        mFrameRateTextView.setText(statString);
    }

    /**
     * 显示重连弹窗
     */
    private void showWaitingDialog() {
        if (mWaitingDialog == null) {
            mWaitingDialog = new ProgressDialog(FullscreenActivity.this);
            mWaitingDialog.setMessage("重连中 .... 请稍候 ...");
            mWaitingDialog.setIndeterminate(true);
            mWaitingDialog.setCancelable(true);
            mWaitingDialog.setCanceledOnTouchOutside(false);
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mWaitingDialog != null && !mWaitingDialog.isShowing()) {
                    resetFrameRate();
                    LogUtil.info(TAG, "showWaitingDialog");
                    mWaitingDialog.show();
                }
            }
        });
    }

    /**
     * 隐藏重连弹窗
     */
    public void dismissWaitingDialog() {
        if (mWaitingDialog != null && mWaitingDialog.isShowing()) {
            LogUtil.info(TAG, "dismissWaitingDialog");
            mWaitingDialog.dismiss();
        }
    }

    /**
     * 重连失败弹窗
     */
    public void showCannotReconnectDialog() {
        dismissWaitingDialog();
        AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
        builder.setMessage("重连失败，请重新启动手机");
        builder.setTitle("alert");
        builder.setPositiveButton("ok", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                LogUtil.error(TAG, "reconnect cloud phone fail, will exit");
                dialog.dismiss();
                finish();
            }
        });
        builder.setCancelable(false);
        builder.create().show();
    }

    /**
     * 初始化配置信息
     */
    private void setupJniConf() {
        if (BuildConfig.DEBUG) {
            LogUtil.info(TAG, "setupJniConf: videoConf -> " + videoConf.toString());
        }
        if (!NetConfig.initialize()) {
            LogUtil.error(TAG, "NetConfig initialize failed");
            // showPopup(getStringFromRes(R.string.failed_instruction), -1);
            finish();
            return;
        }

        // TODO: 待实现intent.getIntExtra(SPUtil.INS_SETTING, 0)
        if (!NetConfig.setNetConfig(videoConf.getIp(), videoConf.getVmiAgentPort(), 0)) {
            LogUtil.error(TAG, "NetConfig setNetConfig failed, " + "port:" + videoConf.getVmiAgentPort());
            // showPopup(getStringFromRes(R.string.failed_instruction), -1);
            finish();
        }

        int initResult = OpenGLJniWrapper.initialize();
        if (initResult == 0) {
            LogUtil.info(TAG, "VideoEngine initialize success");
        } else {
            LogUtil.error(TAG, "VideoEngine initialize failed, result:" + initResult);
            finish();
        }
    }

    private void setupUpstream() {
        LogUtil.info(TAG, "setupUpstream");
        // 为防止音频在刚开始连接的时候就播放，先停止音频播放功能
        int ret = AudioTrackPlayer.stopAudio();
        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            LogUtil.info(TAG, "VMI_SUCCESS stop Audio success ");
        } else if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL) {
            LogUtil.error(TAG, "VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL stop Audio fail ");
        }
        // 开始数据接收分发
        upstreamReceiveDispatcher = new UpstreamReceiveDispatcher();
        upstreamReceiveDispatcher.addNewPacketCallback(OpenGLJniWrapper.AUDIO, new AudioPlayerCallback());
        upstreamReceiveDispatcher.addNewPacketCallback(OpenGLJniWrapper.SENSOR, new SensorCallBack());
        upstreamReceiveDispatcher.addNewPacketCallback(OpenGLJniWrapper.GPS, new LocationCallBack());
        upstreamReceiveDispatcher.addNewPacketCallback(OpenGLJniWrapper.MIC, callback);
        upstreamReceiveDispatcher.addNewPacketCallback(OpenGLJniWrapper.FPS, fpsTestCallBack);
        upstreamReceiveDispatcher.addNewPacketCallback(OpenGLJniWrapper.CONFIG, encodeParamsCallback);
        upstreamReceiveDispatcher.start();
    }

    /**
     * 开始执行连接
     */
    private void startConnect() {
        stopHandlerFlag = false;
        setupMediaCodec();
        setupJniConf();
        setupUpstream();
        mThreadPool.submit(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(DELAT_TIME);
                } catch (InterruptedException e) {
                    Log.d(TAG, "InterruptedException");
                }
                int startResult = OpenGLJniWrapper.start(mSurfaceView.getHolder().getSurface(), guestWidth, guestHeight, metric.densityDpi);
                if (startResult == 0) {
                    startModule();
                    mThreadHandler.sendEmptyMessageDelayed(WHAT_CONNECT_SUCCESS, DELAY_BLACK_SCREEN);
                    lastTotalTraffic = getLastTotalTraffic();
                    mThreadHandler.sendEmptyMessage(WHAT_CHECK_CONNECT_STATUS);
                    // 打印接入云手机成功审计日志
                    LogUtil.info(TAG, "@AUDIT_INFO: username: " + userBean.getUsername()
                            + ", action: startCloudPhone, result: true");
                } else {
                    // 打印接入云手机失败审计日志
                    LogUtil.error(TAG, "@AUDIT_INFO: username:" + userBean.getUsername() +
                            ", action: startCloudPhone, result: false start failed, startResult:" +
                            startResult);
                    mThreadHandler.sendEmptyMessage(WHAT_CONNECT_FAILED);
                }
            }
        });
    }

    private void startConnectQuickly() {
        stopHandlerFlag = false;
        setupMediaCodec();
        setupJniConf();
        setupUpstream();
        mThreadPool.submit(new Runnable() {
            @Override
            public void run() {
                int startResult = OpenGLJniWrapper.start(mSurfaceView.getHolder().getSurface(), guestWidth, guestHeight, metric.densityDpi);
                if (startResult == 0) {
                    startModule();
                    mThreadHandler.sendEmptyMessage(WHAT_CONNECT_SUCCESS);
                    lastTotalTraffic = getLastTotalTraffic();
                    mThreadHandler.sendEmptyMessage(WHAT_CHECK_CONNECT_STATUS);
                    // 打印接入云手机成功审计日志
                    LogUtil.info(TAG, "@AUDIT_INFO: "
                            + ", action: startCloudPhone, result: true");

                } else {
                    // 打印接入云手机失败审计日志
                    LogUtil.error(TAG, "@AUDIT_INFO: " +
                            ", action: startCloudPhone, result: false start failed, startResult:" +
                            startResult);
                    mThreadHandler.sendEmptyMessage(WHAT_CONNECT_FAILED);
                }
            }
        });
    }

    private long getLastTotalTraffic() {
        int uid = getApplicationInfo().uid;
        long currentRxBytes = TrafficStats.getUidRxBytes(uid); // 接收字节数
        long currentTxBytes = TrafficStats.getUidTxBytes(uid); // 发送字节数
        return currentRxBytes + currentTxBytes;
    }

    /**
     * 显示连接失败弹窗
     */
    private void showConnectFailedDialog() {
        AlertDialog.Builder dlgAlert = new AlertDialog.Builder(FullscreenActivity.this);
        LogUtil.debug(TAG, "building AlertDialog");
        dlgAlert.setMessage("连接云手机服务器失败，请稍后重试");
        dlgAlert.setTitle("alert");
        dlgAlert.setPositiveButton("ok", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                LogUtil.debug(TAG, "Destroying AlertDialog");
                dialog.dismiss();
                LogUtil.error(TAG, "can not reconnect");
                finish();
            }
        });
        dlgAlert.setCancelable(false);
        dlgAlert.create().show();
    }

    /**
     * 点击返回键触发
     */
    @Override
    public void onBackPressed() {
        if (mExitDialog != null && mExitDialog.isShowing()) {
            mExitDialog.dismiss();
        } else if (mLagDialog != null && mLagDialog.isShowing()) {
            mLagDialog.dismiss();
        } else {
            showExitDialog();
        }
    }

    /**
     * 显示退出弹窗
     */
    public void showExitDialog() {
        if (mExitDialog == null) {
            mExitDialog = new ExitDialog(mActivity)
                    .setNegativeButton(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            mExitDialog.dismiss();
                        }
                    })
                    .setPositiveButton(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            LogUtil.info(TAG, "exit from dialog");
                            mExitDialog.dismiss();
                            stopMonitor();
                            OpenGLJniWrapper.stop();
                            android.os.Process.killProcess(android.os.Process.myPid());
                            LogUtil.info(TAG, "@AUDIT_INFO: username: "
                                    + MyApplication.instance.getUserBean().getUsername()
                                    + ", action: stopCloudPhone, result: true");
                        }
                    })
                    .setReconnectButton(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            stopHandlerFlag = true;
                            flashLandscape.setVisibility(View.VISIBLE);
                            flashNormal.setVisibility(View.VISIBLE);
                            stopMonitor();
                            unregisterListener();
                            OpenGLJniWrapper.stop();
                            startConnectQuickly();
                        }
                    });
        }
        if (!mExitDialog.isShowing()) {
            mExitDialog.show();
        }
    }

    private void initLayoutChangeListener() {
        mSurfaceView.addOnLayoutChangeListener(new View.OnLayoutChangeListener() {
            @Override
            public void onLayoutChange(View view, int left, int top, int right, int bottom,
                                       int oldLeft, int oldTop, int oldRight, int oldBottom) {
                LogUtil.error("[layoutchange]:", String.format(Locale.ENGLISH, "{%d %d %d %d}, {%d %d %d %d}",
                        left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom));
                int width = right - left;
                int height = bottom - top;
                int orient = FullscreenActivity.this.getRequestedOrientation();
                screenInputFitting(width, height, orient);
            }
        });
    }

    public void showEncodeInputDialog() {
        getEncodeParam();
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
        // 由于获取服务端EncodeParam接口是异步接收的。当服务端EncodeParam和本地EncodeParam不一致时，直接调用会读取本地错误的值，暂时使用200ms来等待异步接口写入本地数据
        mThreadHandler.postDelayed(() -> {
            encodeDialog.show();
        }, 200);

    }

    /**
     * 发送编码参数
     */
    private void sendEncodeParam() {
        try {
            int bitRate = Integer.parseInt(encodeDialog.getBitRate());
            int profile = Integer.parseInt(encodeDialog.getProfile());
            int gopSize = Integer.parseInt(encodeDialog.getGopSize());
            int rcMode = Integer.parseInt(encodeDialog.getRcModeEditText());
            int interpolation = Integer.parseInt(encodeDialog.getInterpolationEditText());
            int forceKeyFrame = Integer.parseInt(encodeDialog.getKeyFrameEditText());
            int crf = Integer.parseInt(encodeDialog.getCrfEditText());
            int vbvBufferSize = Integer.parseInt(encodeDialog.getVbvBufferSizeEditText());
            int maxCrfRate = Integer.parseInt(encodeDialog.getMaxCrfRateEditText());

            SPUtil.putInt(SPUtil.VIDEO_BIT_RATE, bitRate);
            SPUtil.putInt(SPUtil.VIDEO_PROFILE, profile);
            SPUtil.putInt(SPUtil.VIDEO_GOP_SIZE, gopSize);
            SPUtil.putInt(SPUtil.VIDEO_RC_MODE, rcMode);
            SPUtil.putInt(SPUtil.VIDEO_INTERPOLATION, interpolation);
            SPUtil.putInt(SPUtil.VIDEO_FORCE_KEY_FRAME, forceKeyFrame);
            SPUtil.putInt(SPUtil.VIDEO_CRF, crf);
            SPUtil.putInt(SPUtil.VIDEO_VBV_BUFFER_SIZE, vbvBufferSize);
            SPUtil.putInt(SPUtil.VIDEO_MAX_CRF_RATE, maxCrfRate);
            SPUtil.putInt(SPUtil.VIDEO_STREAM_WIDTH, encodeDialog.getStreamWidth());
            SPUtil.putInt(SPUtil.VIDEO_STREAM_HEIGHT, encodeDialog.getStreamHeight());

            EncodeParams encodeParams = new EncodeParams();
            encodeParams.setBitrate(SPUtil.getInt(SPUtil.VIDEO_BIT_RATE, 3000000));
            encodeParams.setProfile(SPUtil.getInt(SPUtil.VIDEO_PROFILE, 1));
            encodeParams.setGopSize(SPUtil.getInt(SPUtil.VIDEO_GOP_SIZE, 30));
            encodeParams.setRcMode(SPUtil.getInt(SPUtil.VIDEO_RC_MODE, 2));
            encodeParams.setForceKeyFrame(SPUtil.getInt(SPUtil.VIDEO_FORCE_KEY_FRAME, 0));
            encodeParams.setInterpolation(!(SPUtil.getInt(SPUtil.VIDEO_INTERPOLATION, 0) == 0));
            encodeParams.setCrf(SPUtil.getInt(SPUtil.VIDEO_CRF, 34));
            encodeParams.setVbvBufferSize(SPUtil.getInt(SPUtil.VIDEO_VBV_BUFFER_SIZE, 1000));
            encodeParams.setMaxCrfRate(SPUtil.getInt(SPUtil.VIDEO_MAX_CRF_RATE, 20000000));
            encodeParams.setStreamWidth(SPUtil.getInt(SPUtil.VIDEO_STREAM_WIDTH, Constant.VMI_720P_WIDTH));
            encodeParams.setStreamHeight(SPUtil.getInt(SPUtil.VIDEO_STREAM_HEIGHT, Constant.VMI_720P_HEIGHT));
            boolean b = OpenGLJniWrapper.setEncodeParam(encodeParams);
            if (b) {
                Toast.makeText(this, R.string.video_encode_param_send_success, Toast.LENGTH_LONG).show();
            } else {
                Toast.makeText(this, R.string.video_encode_param_send_fail, Toast.LENGTH_LONG).show();
            }
        } catch (Throwable throwable) {
            LogUtil.error(TAG, throwable.toString());
            Toast.makeText(this, R.string.video_encode_param_send_fail, Toast.LENGTH_LONG).show();
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
            SPUtil.putInt(SPUtil.VIDEO_FORCE_KEY_FRAME, keyFrameValue);
        } else {
            SPUtil.removeKey(SPUtil.VIDEO_FORCE_KEY_FRAME);
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

    /**
     * screenInputFitting.
     *
     * @param width  width.
     * @param height height.
     * @param orient orient.
     */
    private void screenInputFitting(int width, int height, int orient) {
        if ((orient == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE)
                || (orient == ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE)) {
            // swap the width and height
            displayHeight = width;
            displayWidth = height;
        } else {
            displayWidth = width;
            displayHeight = height;
        }
        inputXScale = displayWidth / vmWidth; // vmWidth
        inputYScale = displayHeight / vmHeight; // vmHeight
        LogUtil.info(TAG, String.format(Locale.ROOT, "Input fitting: surfaceView vm (%d x %d)," +
                        "surfaceView device (%f x %f), surfaceView input (%f x %f)",
                vmWidth, vmHeight, displayWidth, displayHeight, inputXScale, inputYScale));
    }

    @Override
    protected void onResume() {
        super.onResume();
        LogUtil.debug(TAG, "onResume");
    }

    // 此参数保证onDestroy只会执行一次
    private volatile boolean mExited = false;

    @Override
    protected void onStop() {
        super.onStop();
        LogUtil.debug(TAG, "onStop");
        if (!mExited) {
            // 退后台不保持连接
            finish();
        }
    }

    @Override
    protected void onDestroy() {
        LogUtil.debug(TAG, "onDestroy");
        unregisterReceiver(innerBroadcastReceiver);
        mThreadHandler.removeCallbacksAndMessages(null);
        mThreadPool.destroy();
        // 移除传感器、位置监听
        unregisterListener();
        super.onDestroy();
    }

    @Override
    public void finish() {
        mExited = true;
        android.os.Process.killProcess(android.os.Process.myPid());
        super.finish();
    }

    /**
     * get getStatisticView
     *
     * @return mStatisticLayout
     */
    public View getStatisticView() {
        return mStatisticLayout;
    }

    /**
     * get getFrameRateView
     *
     * @return mFrameRateTextView
     */
    public View getFrameRateView() {
        return mFrameRateTextView;
    }

    public class InnerBroadcastReceiver extends BroadcastReceiver {
        public static final String TAG = "receive_adb_data";

        @Override
        public void onReceive(Context context, Intent intent) {
            if (!intent.getAction().equals(TAG)) {
                return;
            }
            if (intent.getExtras() == null) {
                return;
            }
            String cmd = intent.getExtras().getString("cmd", "");
            Log.d(TAG, "onReceive cmd:" + cmd);
            switch (cmd) {
                case "video":
                    int videoBitrate = intent.getIntExtra("videoBitrate", 3000000);
                    int videoProfile = intent.getIntExtra("videoProfile", 1);
                    int videoGopSize = intent.getIntExtra("videoGopSize", 30);
                    int videoRcMode = intent.getIntExtra("videoRcMode", 2);
                    int videoForceKeyFrame = intent.getIntExtra("videoForceKeyFrame", 0);
                    int videoInterpolation = intent.getIntExtra("videoInterpolation", 0);
                    int videoCrf = intent.getIntExtra("videoCrf", 34);
                    int videoVbvBufferSize = intent.getIntExtra("videoVbvBufferSize", 1000);
                    int videoMaxrate = intent.getIntExtra("videoMaxrate", 20000000);
                    EncodeParams encodeParams = new EncodeParams();
                    encodeParams.setBitrate(videoBitrate);
                    encodeParams.setProfile(videoProfile);
                    encodeParams.setGopSize(videoGopSize);
                    encodeParams.setRcMode(videoRcMode);
                    encodeParams.setForceKeyFrame(videoForceKeyFrame);
                    encodeParams.setInterpolation(!(videoInterpolation == 0));
                    encodeParams.setCrf(videoCrf);
                    encodeParams.setVbvBufferSize(videoVbvBufferSize);
                    encodeParams.setMaxCrfRate(videoMaxrate);
                    boolean videoSendResult = OpenGLJniWrapper.setEncodeParam(encodeParams);
                    if (videoSendResult) {
                        Log.i(TAG, "onReceive audio:视频编码参数发送成功.videoBitrate:" +
                                videoBitrate + ",videoProfile:" + videoProfile + ",videoGopSize:" +
                                videoGopSize + ",videoRcMode:" + videoRcMode + ",videoForceKeyFrame:"
                                + videoForceKeyFrame + ",videoInterpolation:" + videoInterpolation
                                + ",videoCrf:" + videoCrf + ",videoVbvBufferSize:"
                                + videoVbvBufferSize + ",videoMaxrate:" + videoMaxrate);
                    } else {
                        Log.e(TAG, "onReceive audio:视频编码参数发送失败.videoBitrate:" +
                                videoBitrate + ",videoProfile:" + videoProfile + ",videoGopSize:" +
                                videoGopSize + ",videoRcMode:" + videoRcMode + ",videoForceKeyFrame:"
                                + videoForceKeyFrame + ",videoInterpolation:" + videoInterpolation
                                + ",videoCrf:" + videoCrf + ",videoVbvBufferSize:"
                                + videoVbvBufferSize + ",videoMaxrate:" + videoMaxrate);
                    }
                    break;
                case "audio":
                    int audioBitrate = intent.getIntExtra("audioBitrate", 192000);
                    int audioSampleInterval = intent.getIntExtra("audioSampleInterval", 10);
                    VmiConfigAudio vmiConfigAudio = new VmiConfigAudio();
                    vmiConfigAudio.setSampleInterval(audioSampleInterval);
                    vmiConfigAudio.setBitrate(audioBitrate);
                    boolean audioSendResult = OpenGLJniWrapper.setAudioParam(vmiConfigAudio);
                    if (audioSendResult) {
                        Log.i(TAG, "onReceive audio:音频编码参数发送成功.audioBitrate:" +
                                audioBitrate + ",audioSampleInterval:" + audioSampleInterval);
                    } else {
                        Log.i(TAG, "onReceive audio:音频编码参数发送失败.audioBitrate:" +
                                audioBitrate + ",audioSampleInterval:" + audioSampleInterval);
                    }
                    break;
                case "vmi_config":

                    break;
                default:
                    Log.i(TAG, "onReceive: adb接收到无效的cmd命令。");
                    break;
            }
        }
    }
}
