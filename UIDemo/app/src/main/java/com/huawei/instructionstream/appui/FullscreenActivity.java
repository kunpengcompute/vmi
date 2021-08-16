/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.net.TrafficStats;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.HandlerThread;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;
import java.text.SimpleDateFormat;
import java.util.Date;
import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudgame.touch.VmiTouch;
import com.huawei.instructionstream.appui.audio.play.AudioPlayerCallback;
import com.huawei.instructionstream.appui.common.RotateDialog;
import com.huawei.instructionstream.appui.model.SettingsBean;
import com.huawei.instructionstream.appui.utils.FileUtil;
import com.huawei.instructionstream.appui.utils.LogUtils;
import com.huawei.instructionstream.appui.maincontrol.DataPipe;
import com.huawei.instructionstream.appui.maincontrol.Setting;
import com.huawei.instructionstream.appui.maincontrol.NetConfig;
import com.huawei.instructionstream.appui.maincontrol.UpstreamReceiveDispatcher;
import com.huawei.instructionstream.appui.maincontrol.floatingview.DragView;
import com.huawei.instructionstream.appui.maincontrol.floatingview.FloatingViewManager;
import com.huawei.instructionstream.appui.maincontrol.floatingview.MagnetViewListener;
import com.huawei.instructionstream.appui.utils.SPUtil;
import com.huawei.instructionstream.appui.widget.VmiSurfaceView;
import com.huawei.instructionstream.appui.utils.InstanceofUtils;
import com.huawei.instructionstream.appui.utils.ThreadPool;
import com.huawei.instructionstream.appui.common.CommonDialog;
import com.huawei.instructionstream.appui.engine.InstructionEngine;
import com.huawei.instructionstream.appui.engine.InstructionWrapper;
import com.huawei.instructionstream.appui.engine.JniCallBack;
import com.huawei.instructionstream.appui.engine.NativeListener;

import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

/**
 * An example full-screen activity that shows and hides the system UI (i.e. status bar and navigation/system bar) with
 * user interaction.
 *
 * @since 2017-12-22
 */
public class FullscreenActivity extends FragmentActivity implements NativeListener {
    private static final String TAG = "CGP_VMI_CG_FA";
    private static final String AUDIO_TAG = "VmiAudioClient";
    private static final int CODE_AUDIO_RECORD = 1;
    private static final Map<String, Integer> LOGGING_LEVEL_MAP = new HashMap<>();
    private static final int MSG_RECONNECT = 3;
    private static final int MSG_ENGINE_READY = 4;

    /**
     * identify simulator,  monbox and kbox
     */
    private static final String MONBOX_MODEL = "Monbox";
    private static final String KBOX_MODEL = "Kbox";
    private static final int INITIAL_VALUE_ZERO = 0;
    private static final int GREEN_WIFI_MAX_DELAY = 80000;
    private static final int YELLOW_WIFI_MAX_DELAY = 160000;
    private static final int GREEN_WIFI_SIGN = 1;
    private static final int YELLOW_WIFI_SIGN = 2;
    private static final int RED_WIFI_SIGN = 3;
    private boolean isSimulator = false;

    /**
     * for reconnect cloud phone.
     */
    public volatile boolean stopFlag = true;

    /**
     * Monitor Thread stop flag.
     */
    public volatile boolean stopped = true;

    /**
     * reconnect alert dialog.
     */
    public AlertDialog reconnectDialog = null;

    boolean checkingPermission = false;
    int computedLogLevel = android.util.Log.INFO;
    UpstreamReceiveDispatcher upstreamReceiveDispatcher = null;
    AudioPlayerCallback audioPlayerCallback = null;
    JniCallBack jniCallback = new JniCallBack();
    TextView textViewHint = null;
    ImageButton imageButtonRunPhone = null;
    ImageButton iconImage = null;
    Intent intent;
    Setting settings;
    boolean isShowState = false;
    boolean isShow = true;
    int oritentation = -1;
    boolean newlyCreated = true;
    DisplayMetrics metric = new DisplayMetrics();

    private HandlerThread workThread = null;
    private Handler sdkHandler = null;
    private String nativeStatString = "";
    private long lastTxBytes = 0;
    private long lastRxBytes = 0;
    private long totalBandwidthBytes = 0;
    private int guestWidth = 1080;
    private int guestHeight = 1920;
    /**
     * display width.
     */
    private float displayWidth = 1080.0f;

    /**
     * display height.
     */
    private float displayHeight = 1920.0f;

    /**
     * input X scale.
     */
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
    private VmiSurfaceView surfaceView;
    private TextView textViewRate;
    private TextView textPop;
    private String statDetail = "";
    private final ThreadPool threadPool = new ThreadPool(4, 20);
    private Timer timer = null;
    private TimerTask timerTask = null;
    private SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss.SSS");
    private InstructionEngine engine = new InstructionEngine();
    private RotateDialog rotateDialog;
    private int engineOrientation;
    private View inflateView;
    private boolean isEngineReady = false;

    class ConnectRunable implements Runnable {
        @Override
        public void run() {
            final int startResult = engine.start(surfaceView.getHolder().getSurface(),
                guestWidth, guestHeight, metric.densityDpi);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (startResult == InstructionEngine.VMI_INSTRUCTION_CLIENT_START_FAIL) {
                        LogUtils.error(TAG, "Security Audit: InstructionEngine start failed, result:" +
                                startResult);
                        showRetryDialog();
                    } else {
                        buildPacketCallback();
                        LogUtils.debug(TAG, "Security Audit: InstructionEngine start success, result:" +
                                startResult);
                    }
                }
            });
        }
    }

    private void setRotation(int rotation) {
        final int rota = rotation;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (oritentation != rota) {
                    LogUtils.info(TAG , "client begin rotate: " + oritentation + " ---> " + rota);
                    oritentation = rota;
                    switch (oritentation) {
                        case Surface.ROTATION_0:
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION0);
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                            break;
                        case Surface.ROTATION_90:
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION90);
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                            if (!isEngineReady) {
                                findViewById(R.id.flashLandscape).setVisibility(View.VISIBLE);
                                findViewById(R.id.flashNormal).setVisibility(View.INVISIBLE);
                            }
                            break;
                        case Surface.ROTATION_180:
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION180);
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT);
                            break;
                        case Surface.ROTATION_270:
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION270);
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

    private Handler popWindowHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Bundle bundle = msg.getData();
            final String popupText = bundle.getString("Tip");
            int alivetime = bundle.getInt("aliveTime");
            int liveTime = alivetime;
            if (liveTime < 0) {
                liveTime = 5000;
            }
            stopShowPopupTimer();
            startShowPopupTimer(liveTime);

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    textPop.setText(popupText);
                    textPop.setVisibility(View.VISIBLE);
                }
            });
        }
    };

    /**
     * Add for stopping cloud phone if background running timeout end.
     */
    private CommonDialog dialog;
    private ProgressDialog waitingDialog = null;

    private void startShowPopupTimer(long aliveTime) {
        if (timer == null) {
            timer = new Timer();
        }

        if (timerTask == null) {
            timerTask = new TimerTask() {
                @Override
                public void run() {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            LogUtils.info(TAG , "textPop set INVISIBLE");
                            textPop.setVisibility(View.INVISIBLE);
                        }
                    });
                }
            };
        }

        if (timer != null && timerTask != null) {
            timer.schedule(timerTask, aliveTime);
        }
    }

    private void stopShowPopupTimer() {
        if (timer != null) {
            timer.cancel();
            timer = null;
        }

        if (timerTask  != null) {
            timerTask.cancel();
            timerTask = null;
        }
    }

    private void startWorkThread() {
        workThread = new HandlerThread("workThread");
        workThread.start();
        sdkHandler = new Handler(workThread.getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MSG_RECONNECT:
                        reconnectCloudPhone();
                        break;
                    case MSG_ENGINE_READY:
                        isEngineReady = true;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                findViewById(R.id.flashNormal).setVisibility(View.INVISIBLE);
                                findViewById(R.id.flashLandscape).setVisibility(View.INVISIBLE);
                            }
                        });
                        break;
                    default:
                        break;
                }
            }
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        checkSimulator();
        FileUtil.deleteFile("client_stat_0_fps_data.csv");
        settings = getSettingsFromIntent();
        setupViews();
        setupSurfaceView();
        initDataPipe();
        loggingSetup();
        setupJniConf();
        checkPermissions();
        setupCallback();
        createWaitDialog();
        startWorkThread();
        createRotateDialog();
        LogUtils.info(TAG, "endOfOnCreate");
    }

    private void createRotateDialog() {
        LayoutInflater inflater = getLayoutInflater();
        inflateView = inflater.inflate(R.layout.rotate_dialog, null);
        rotateDialog = new RotateDialog(FullscreenActivity.this, inflateView);
    }

    private void setupJniConf() {
        if (!NetConfig.initialize()) {
            LogUtils.error(TAG, "NetConfig initialize failed");
            showPopup(getStringFromRes(R.string.failed_instruction), -1);
            finish();
            return;
        }

        if (!NetConfig.setNetConfig(settings.getProxyIp(), settings.getProxyPort(), intent.getIntExtra(SPUtil.INS_SETTING, 0))) {
            LogUtils.error(TAG, "NetConfig setNetConfig failed," +
                "port:" + settings.getProxyPort() + ", connect type:" + intent.getIntExtra(SPUtil.INS_SETTING, 0));
            showPopup(getStringFromRes(R.string.failed_instruction), -1);
            finish();
        }

        int initResult = engine.initialize();
        if (initResult != InstructionEngine.VMI_SUCCESS) {
            LogUtils.error(TAG, "InstructionEngine initialize failed, result:" + initResult);
            showPopup(getStringFromRes(R.string.failed_instruction), -1);
            finish();
            return;
        }
    }

    private void setupViews() {
        setFullScreen();
        keepScreenLongLight(this, true);
        textViewHint = InstanceofUtils.toTextView(findViewById(R.id.textViewInfo));
        imageButtonRunPhone = InstanceofUtils.toImageButton(findViewById(R.id.imageButtonRunPhone));
        iconImage = InstanceofUtils.toImageButton(findViewById(R.id.iconImage));
        textViewRate = InstanceofUtils.toTextView(findViewById(R.id.textRate));
        textPop = InstanceofUtils.toTextView(findViewById(R.id.textPop));
        if (textPop != null) {
            textPop.setText("");
        }
    }

    private void setupSurfaceView() {
        if (findViewById(R.id.vmi_surfaceView) instanceof VmiSurfaceView) {
            surfaceView = (VmiSurfaceView) findViewById(R.id.vmi_surfaceView);
        } else {
            return;
        }
        jniCallback.setNativeCallback();
        jniCallback.setNativeListener(this);
        setSurfaceViewListeners();
        initSurfaceHolder();
    }

    private void initSurfaceHolder() {
        final SurfaceHolder surfaceHolder = surfaceView.getHolder();
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        surfaceView.getHolder().setFixedSize(guestWidth, guestHeight);
        surfaceView.initialize(guestWidth, guestHeight);
        initSurfaceHolderCallback(surfaceHolder);
        initLayoutChangeListener();
    }

    private void initSurfaceHolderCallback(SurfaceHolder surfaceHolder) {
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
                LogUtils.info(TAG, "surfaceCreated");
                newlyCreated = true;
            }

            @Override
            public void surfaceChanged(SurfaceHolder inSurfaceHolder, int format, int width, int height) {
                LogUtils.info(TAG, "surfaceChanged width:" + width + ", height:" + height);
                if (!newlyCreated) {
                    return;
                }
                newlyCreated = false;
                threadPool.submit(new ConnectRunable());
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
                LogUtils.info(TAG, "surfaceDestroyed");
            }
        });
    }

    private void initLayoutChangeListener() {
        // do it later: move to class VmiSurfaceView
        surfaceView.addOnLayoutChangeListener(new View.OnLayoutChangeListener() {
            @Override
            public void onLayoutChange(View view, int left, int top, int right, int bottom,
                                       int oldLeft, int oldTop, int oldRight, int oldBottom) {
                LogUtils.error("[layoutchange]:", String.format(Locale.ENGLISH, "{%d %d %d %d}, {%d %d %d %d}",
                        left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom));
                int width = right - left;
                int height = bottom - top;
                int orient = FullscreenActivity.this.getRequestedOrientation();
                screenInputFitting(width, height, orient);
            }
        });
    }

    /**
     * screenInputFitting.
     *
     * @param width width.
     * @param height height.
     * @param orient orient.
     */
    private void screenInputFitting(int width, int height, int orient) {
        if ((orient == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE)
                || (orient == ActivityInfo .SCREEN_ORIENTATION_REVERSE_LANDSCAPE)) {
            // swap the width and height
            displayHeight = width;
            displayWidth = height;
        } else {
            displayWidth = width;
            displayHeight = height;
        }
        inputXScale = displayWidth / vmWidth; // vmWidth
        inputYScale = displayHeight / vmHeight; // vmHeight
        LogUtils.info(TAG, String.format(Locale.ROOT, "Input fitting: surfaceView vm (%d x %d)," +
                        "surfaceView device (%f x %f), surfaceView input (%f x %f)",
                vmWidth, vmHeight, displayWidth, displayHeight, inputXScale, inputYScale));
    }

    private void setSurfaceViewListeners() {
        surfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                if (!isEngineReady) {
                    return true;
                }
                return VmiTouch.getInstance().onTouch(view, event);
            }
        });
    }

    private void initDataPipe() {
        DataPipe.setInstructionEngine(engine);
        DataPipe.registerHookToTouch();
        DataPipe.registerAudioSendHook();
    }

    private void createWaitDialog() {
        waitingDialog = new ProgressDialog(this);
        waitingDialog.setMessage(getStringFromRes(R.string.retry_connection));
        waitingDialog.setIndeterminate(true);
        waitingDialog.setCancelable(false);
        LogUtils.info(TAG, "Create waitingDialog =" + waitingDialog + ", fullact=" + FullscreenActivity.this);
    }

    private void setupCallback() {
        jniCallback.setNativeCallback();
        jniCallback.setNativeListener(this);
        textViewHint.setVisibility(View.INVISIBLE);
        imageButtonRunPhone.setVisibility(View.INVISIBLE);
        iconImage.setVisibility(View.INVISIBLE);
    }

    /**
     * stop monitor and wait until it is really stopped.
     */
    public void stopMonitorBlocked() {
        stopFlag = true;
        while (!stopped) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                LogUtils.info(TAG, "sleep interrupted");
            }
        }
    }

    /**
     * update send and recv bytes
     */
    private void updateSendAndRecvBytes() {
        if (!nativeStatString.isEmpty()) {
            String txBytesString = nativeStatString.substring(
                nativeStatString.indexOf("BytesSend:") + 10, nativeStatString.indexOf("SendEnd"));
            lastTxBytes = Long.parseLong(txBytesString.trim());
            String rxBytesString = nativeStatString.substring(
                nativeStatString.indexOf("BytesRecv:") + 10, nativeStatString.indexOf("RecvEnd"));
            lastRxBytes = Long.parseLong(rxBytesString.trim());
        }
    }

    /**
     * start a monitor thread.
     */
    public void startMonitorThread() {
        if (!stopped) {
            return;
        }
        stopFlag = false;
        stopped = false;
        threadPool.submit(new StartPhoneRunable());
    }

    class StartPhoneRunable implements Runnable {
        @Override
        public void run() {
            while (!stopFlag) {
                if (!stopFlag) {
                    sleep(1000);
                }

                if (!stopFlag && isEngineReady) {
                    dismissWaitingDialog();
                    setGlReady();
                } else {
                    LogUtils.info(TAG, "glready get thread: false");
                }
                nativeStatString = engine.getStat();
                updateSendAndRecvBytes();
                long bandwidthBytes = lastRxBytes + lastTxBytes;
                totalBandwidthBytes += bandwidthBytes;
                if (!nativeStatString.isEmpty()) {
                    String lagString = nativeStatString.substring(
                            nativeStatString.indexOf("LAG:") + 4, nativeStatString.indexOf("ms"));
                    String subStartString = nativeStatString.substring(nativeStatString.indexOf("LAG:"), nativeStatString.indexOf("LAG:") + 5);
                    String subEndString = nativeStatString.substring(nativeStatString.indexOf("ms"));
                    final int lagFromNative = Integer.parseInt(lagString.trim());
                    int milliToMicro = 1000;
                    int lag = lagFromNative / milliToMicro;
                    final String nativeString = subStartString + " " + lag + subEndString;
                    final String statString = nativeString + "带宽：" + Long.toString(bandwidthBytes / 1024) +
                            "KB\n流量：" + Long.toString(totalBandwidthBytes / 1024 / 1024) + "MB\n时间：" +
                            simpleDateFormat.format(new Date());
                    stringTofile(bandwidthBytes, lag, nativeString, lastTxBytes, lastRxBytes);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            updateFloater(statString, lagFromNative);
                        }
                    });
                }
            }
            stopped = true;
        }

        private void stringTofile(long bandwidthBytes, int lag, String nativeString, long txBytes, long rxBytes) {
            String fps = "0";
            if (nativeString.contains("fps")) {
                String[] strings = nativeString.split("fps");
                fps = strings[1].substring(1, strings[1].indexOf("frameCount") - 2);
            }
            String bandWidth = Long.toString(bandwidthBytes / 1024) + "Kbit/s";
            String totalBandwidth = Long.toString(totalBandwidthBytes / 1024 / 1024) + "MByte";
            String send_bytes = Long.toString(txBytes) + "byte";
            String recv_bytes = Long.toString(rxBytes) + "byte";
            String date = simpleDateFormat.format(new Date());
            FileUtil.stringToFile("client_stat_0_fps_data.csv", date + "\tfps: " + fps + "\tLAG: " + lag +
                "ms\tbandwidth: " + bandWidth + "\ttotalBandwidth: " + totalBandwidth +
                "\tsend_byte: " + send_bytes + "\trecv_byte: " + recv_bytes);
        }
    }

    private void updateFloater(String statString, int lag) {
        textViewRate.setText(statString);
        if (isShowState) {
            textViewRate.setVisibility(View.VISIBLE);
        } else {
            textViewRate.setVisibility(View.INVISIBLE);
        }
        int netStatus;
        if (lag <= INITIAL_VALUE_ZERO) {
            netStatus = RED_WIFI_SIGN;
        } else if (lag < GREEN_WIFI_MAX_DELAY) {
            netStatus = GREEN_WIFI_SIGN;
        } else if (lag < YELLOW_WIFI_MAX_DELAY) {
            netStatus = YELLOW_WIFI_SIGN;
        } else {
            netStatus = RED_WIFI_SIGN;
        }

        if (FloatingViewManager.getInstance().getFloatingView() != null) {
            switch (netStatus) {
                case 1: {
                    FloatingViewManager.getInstance().getFloatingView().setbg(
                            R.mipmap.vmi_greenwifi_gimp4);
                    break;
                }
                case 2: {
                    FloatingViewManager.getInstance().getFloatingView().setbg(
                            R.mipmap.vmi_yellowifi_gimp4);
                    break;
                }
                case 3: {
                    FloatingViewManager.getInstance().getFloatingView().setbg(
                            R.mipmap.vmi_redwifi_gimp4);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    /**
     * set gl read.
     */
    public void setGlReady() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (settings.getFloaterVisible()) {
                    FloatingViewManager.getInstance().add(getApplicationContext());
                    FloatingViewManager.getInstance().getFloatingView().setMagnetViewListener(new MagnetViewListener() {
                        @Override
                        public void onRemove(DragView magnetView) {
                        }

                        @Override
                        public void onClick(DragView magnetView) {
                            if (!isSimulator) {
                                createRotateDialog();
                                if ((engineOrientation == Surface.ROTATION_0 || engineOrientation == Surface.ROTATION_180)) {
                                    rotateDialog.show(getFragmentManager(), Surface.ROTATION_0);
                                } else {
                                    rotateDialog.show(getFragmentManager(), Surface.ROTATION_90);
                                }
                                CheckBox checkBox = inflateView.findViewById(R.id.rotate_frameRate);
                                checkBox.setChecked(isShowState);
                            } else {
                                createRotateDialog();
                                rotateDialog.show(getFragmentManager(), Surface.ROTATION_0);
                                CheckBox checkBox = inflateView.findViewById(R.id.rotate_frameRate);
                                checkBox.setChecked(isShowState);
                            }
                        }
                    });
                }
            }
        });
    }

    /**
     * show reconnect dialog.
     */
    public void showReconnectDialog() {
        LogUtils.info(TAG, "showReconnectDialog");

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage(getStringFromRes(R.string.net_error));
        builder.setTitle("Warning");

        builder.setNegativeButton(getStringFromRes(R.string.reconnect_phone), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                LogUtils.info("reconnectCloudPhone", "reconnect dialog, user select reconnect Phone");
                reconnectDialog.dismiss();
                onNetDisconnect();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!waitingDialog.isShowing()) {
                            waitingDialog.show();
                        }
                        LogUtils.info(TAG, "Show1 waitingDialog=" + waitingDialog + ", fullact="
                                + FullscreenActivity.this + ", isShowing:" + waitingDialog.isShowing());
                    }
                });
            }
        });
        builder.setPositiveButton(getStringFromRes(R.string.exit), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                LogUtils.info("reconnectCloudPhone", "close phone due to user select exit phone in reconnect dialog");
                stopCloudPhone();
            }
        });
        builder.setCancelable(false);
        reconnectDialog = builder.show();
    }

    private String getStringFromRes(int resId) {
        return getResources().getString(resId);
    }

    /**
     * handle disconnection event.
     */
    private void onDisconnection() {
        LogUtils.info(TAG, "connection has been disconnected");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                // if disconnected when reconnecting, will not received glRestoreEnd opcode.
                // need to dismiss this dialog
                if (waitingDialog != null && waitingDialog.isShowing()) {
                    waitingDialog.dismiss();
                }
                stopMonitorBlocked();
                if (upstreamReceiveDispatcher != null) {
                    // make sure these streams are nonblock
                    upstreamReceiveDispatcher.stopBlocked();
                    upstreamReceiveDispatcher = null;
                }
                // Destroy objs first, after popup dialog
                showReconnectDialog();
            }
        });
    }

    /**
     * VMI Instruction Engine event notice.
     *
     * @param event VMI Instruction Engine event.
     */
    public void onVmiInstructionEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3) {
        switch (event) {
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN:
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        findViewById(R.id.flashNormal).setVisibility(View.VISIBLE);
                        findViewById(R.id.flashLandscape).setVisibility(View.VISIBLE);
                    }
                });
                onDisconnection();
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN:
                LogUtils.info(TAG, "recv VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN event");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(FullscreenActivity.this,
                            "网络连接不稳定", Toast.LENGTH_SHORT).show();
                    }
                });
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_VERSION_ERROR:
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
                        builder.setMessage(getStringFromRes(R.string.server_and_client_not_match));
                        builder.setTitle(getStringFromRes(R.string.warning));

                        builder.setNegativeButton(getStringFromRes(R.string.exit), new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                stopCloudPhone();
                            }
                        });
                        builder.setCancelable(false);
                        builder.show();
                    }
                });
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_READY:
                // when reconnect ok to show ui, it will cause bad render.
                // so delay 1.5 seconds can show normal ui.
                final int delayTime = 1500;
                if (sdkHandler != null) {
                    sdkHandler.sendEmptyMessageDelayed(MSG_ENGINE_READY, delayTime);
                }
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_ORIENTATION_CHANGED:
                engineOrientation = reserved0;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!isSimulator) {
                            setRotation(Surface.ROTATION_0);
                        } else {
                            setRotation(engineOrientation);
                        }
                    }
                });
                break;
            default :
        }
    }

    private Setting getSettingsFromIntent() {
        intent = getIntent();
        Setting setting = new Setting();
        String address = intent.getStringExtra("agentAddress");
        setting.setScreenResolution("720");
        setting.parseProxyAddress(address);
        return setting;
    }

    /**
     * stop cloud phone.
     */
    public void stopCloudPhone() {
        LogUtils.info(TAG, "Security Audit: stop cloud phone");
        threadPool.submit(new StopPhoneRunnable());
    }

    class StopPhoneRunnable implements Runnable {
        @Override
        public void run() {
            LogUtils.info(TAG, "before UpstramRunable");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (upstreamReceiveDispatcher != null) {
                        // make sure these streams are nonblock
                        upstreamReceiveDispatcher.stopBlocked();
                        upstreamReceiveDispatcher = null;
                        LogUtils.debug(TAG, "stopCloudPhone: upstreamReceiverDispatcher stopped");
                    }
                }
            });

            engine.stop();
            LogUtils.info(TAG, "after engine.stop");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    finish();
                    LogUtils.info("Kill", "kill current activity process: " + android.os.Process.myPid());
                    android.os.Process.killProcess(android.os.Process.myPid());
                }
            });
            LogUtils.info(TAG, "after killProcess");
        }
    }

    /**
     * dissmiss waiting dialog.
     */
    public void dismissWaitingDialog() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (waitingDialog != null && waitingDialog.isShowing()) {
                    waitingDialog.dismiss();
                    LogUtils.info(TAG, "Dismiss waitingDialog=" + waitingDialog
                            + ", fullact=" + FullscreenActivity.this);
                }
            }
        });
    }

    private void loggingSetup() {
        LOGGING_LEVEL_MAP.put("debug", android.util.Log.DEBUG);
        LOGGING_LEVEL_MAP.put("info", android.util.Log.INFO);
        LOGGING_LEVEL_MAP.put("error", android.util.Log.ERROR);
        LOGGING_LEVEL_MAP.put("none", android.util.Log.ASSERT);
        String logging = settings.getLoggingLevel();
        try {
            computedLogLevel = Integer.parseInt(logging);
        } catch (NumberFormatException e) {
            computedLogLevel = LOGGING_LEVEL_MAP.get(settings.getLoggingLevel());
        } finally {
            LogUtils.info(TAG, "longging setup end");
        }

        LogUtils.info(TAG, "DEBUG Log level is " + android.util.Log.DEBUG);
        LogUtils.info(TAG, "INFO Log level is " + android.util.Log.INFO);
        LogUtils.info(TAG, "ERROR Log level is " + android.util.Log.ERROR);
        LogUtils.info(TAG, "ASSERT is " + android.util.Log.ASSERT);
        LogUtils.info(TAG, "CurrentLoggingLeveL is " + computedLogLevel);
    }

    /**
     * need to run on ui thread.
     */
    public void onNetDisconnect() {
        LogUtils.info("reconnectCloudPhone", "onNetDisconnect");
        isEngineReady = false;
        stopMonitorBlocked();
        if (upstreamReceiveDispatcher != null) {
            // make sure these streams are nonblock
            upstreamReceiveDispatcher.stopBlocked();
            upstreamReceiveDispatcher = null;
        }
        sdkHandler.sendEmptyMessage(MSG_RECONNECT);
    }

    /**
     * reconnect phone.
     */
    public void reconnectCloudPhone() {
        LogUtils.info(TAG, "reconnectCloudPhone");
        reconnectCloudPhoneSetupJniConf();
        threadPool.submit(new ReconnectRunable());
    }

    class ReconnectRunable implements Runnable {
        @Override
        public void run() {
            engine.stop();
            final int initResult = engine.initialize();
            if (initResult != InstructionEngine.VMI_SUCCESS) {
                LogUtils.error(TAG, "surfaceView initialize failed, result:" + initResult);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        showPopup(getStringFromRes(R.string.failed_instruction), -1);
                        finish();
                        return;
                    }
                });
                return;
            }

            final int startResult = engine.start(surfaceView.getHolder().getSurface(),
                guestWidth, guestHeight, metric.densityDpi);
            if (startResult == InstructionEngine.VMI_INSTRUCTION_CLIENT_START_FAIL) {
                LogUtils.error(TAG, "Security Audit: InstructionEngine start failed, result:" + startResult);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        waitingDialog.dismiss();
                        showReconnectDialog();
                    }
                });
            } else {
                buildPacketCallback();
            }
        }
    }

    private void buildPacketCallback() {
        upstreamReceiveDispatcher = UpstreamReceiveDispatcher.getInstance();
        audioPlayerCallback = new AudioPlayerCallback();
        upstreamReceiveDispatcher.addNewPacketCallback((byte) InstructionWrapper.AUDIO,
                audioPlayerCallback);
        upstreamReceiveDispatcher.setInstructionEngine(engine);
        startMonitorThread();
        upstreamReceiveDispatcher.start();
    }

    private void reconnectCloudPhoneSetupJniConf() {
        if (!NetConfig.setNetConfig(settings.getProxyIp(), settings.getProxyPort(), intent.getIntExtra(SPUtil.INS_SETTING, 0))) {
            LogUtils.error(TAG, "NetConfig setNetConfig failed," +
                ", port:" + settings.getProxyPort() +  ", connect type:" + intent.getIntExtra(SPUtil.INS_SETTING, 0));
            showPopup(getStringFromRes(R.string.failed_instruction), -1);
            finish();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK)) {
            if (!isSimulator) {
                if (!rotateDialog.isVisible()) {
                    createRotateDialog();
                    if ((engineOrientation == Surface.ROTATION_0 || engineOrientation == Surface.ROTATION_180)) {
                        rotateDialog.show(getFragmentManager(), Surface.ROTATION_0);
                    } else {
                        rotateDialog.show(getFragmentManager(), Surface.ROTATION_90);
                    }
                    CheckBox checkBox = inflateView.findViewById(R.id.rotate_frameRate);
                    checkBox.setChecked(isShowState);
                } else {
                    dismissRotateDialog();
                }
            } else {
                if (!rotateDialog.isVisible()) {
                    createRotateDialog();
                    rotateDialog.show(getFragmentManager(), Surface.ROTATION_0);
                    CheckBox checkBox = inflateView.findViewById(R.id.rotate_frameRate);
                    checkBox.setChecked(isShowState);
                } else {
                    dismissRotateDialog();
                }

            }
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_APP_SWITCH) {
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_HOME) {
            return true;
        } else {
            return false;
        }
    }

    private void showUiLevelDialog() {
        if (dialog == null || !dialog.isShowing()) {
            final View contentView = View.inflate(this, R.layout.vmi_set_ui_level, null);
            contentView.setAlpha(0.7f);
            if (contentView.findViewById(R.id.textViewStatDetail) instanceof TextView) {
                final TextView textViewStatDetail = (TextView) contentView.findViewById(R.id.textViewStatDetail);
                textViewStatDetail.setText(statDetail);
            }
            if (contentView.findViewById(R.id.checkbox) instanceof CheckBox) {
                CheckBox checkBox = (CheckBox) contentView.findViewById(R.id.checkbox);
                checkBox.setChecked(isShowState);
                checkBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                    @Override
                    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                        isShowState = isChecked;
                    }
                });
            }
            showQuitPhoneDialog(contentView);
        }
    }


    private void showQuitPhoneDialog(View contentView) {
        instantiateQuitPhoneDialog(contentView);
        ColorDrawable dialogColor = new ColorDrawable(Color.WHITE);
        dialogColor.setAlpha(60);
        dialog.getWindow().setBackgroundDrawable(dialogColor);
        dialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialog) {
                LogUtils.error("lldialog", "onDismiss");
            }
        });
        dialog.setCancelable(true);
        dialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
                    if (!isShow) {
                        FullscreenActivity.this.dismissClosePhoneDialog();
                    } else {
                        isShow = false;
                        FullscreenActivity.this.showClosePhoneDialog();
                    }
                    return true;
                }
                return false;
            }
        });
        showClosePhoneDialog();
    }

    private void showClosePhoneDialog() {
        LogUtils.error(TAG, "Activity isFinishing = " + FullscreenActivity.this.isFinishing());
        if (!FullscreenActivity.this.isFinishing()) {
            dialog.show();
        } else {
            LogUtils.error(TAG, Log.getStackTraceString(new Throwable("showClosePhoneDialog")));
        }
    }

    /**
     * 关闭并销毁RotateDialog视图。
     */
    public void dismissRotateDialog() {
        rotateDialog.dismiss();
        rotateDialog.onStop();
        rotateDialog.onDestroyView();
        rotateDialog.onDestroy();
        rotateDialog.onDetach();
    }

    private void dismissClosePhoneDialog() {
        LogUtils.error(TAG, "Activity isFinishing = " + FullscreenActivity.this.isFinishing());
        if (!FullscreenActivity.this.isFinishing()) {
            dialog.dismiss();
        } else {
            LogUtils.error(TAG, Log.getStackTraceString(new Throwable("dismissClosePhoneDialog")));
        }
    }

    private void instantiateQuitPhoneDialog(View contentView) {
        CommonDialog.Builder builder = new CommonDialog.Builder(FullscreenActivity.this)
                .setTitle(getStringFromRes(R.string.phone))
                .setContentView(contentView)
                .setPositiveButton(getStringFromRes(R.string.sure), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        if (isShowState) {
                            textViewRate.setVisibility(View.VISIBLE);
                        } else {
                            textViewRate.setVisibility(View.GONE);
                        }
                        dismissClosePhoneDialog();
                    }
                })
                .setNegativeButton(getStringFromRes(R.string.exit), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // save as global variants
                        LogUtils.info(TAG, "stop Phone due to user select exit Phone in common dialog");
                        stopCloudPhone();
                    }
                });
        dialog = builder.create();
    }

    private void showRetryDialog() {
        LogUtils.info(TAG, "showRetryDialog");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder dlgAlert = new AlertDialog.Builder(FullscreenActivity.this);
                dlgAlert.setMessage(getStringFromRes(R.string.server_error));
                dlgAlert.setTitle("alert");
                dlgAlert.setPositiveButton("ok", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        engine.stop();
                        LogUtils.error(TAG, "sdk stopped, will exit fullscreen atctivity");
                        FullscreenActivity.this.finish();
                    }
                });
                dlgAlert.setCancelable(true);
                dlgAlert.create().show();
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        keepScreenLongLight(this, false);
        workThread.quit();
        LogUtils.debug(TAG, "onDestroy");
    }

    /**
     * 是否使屏幕常亮.
     *
     * @param activity this activity
     * @param isOpenLight isOpenLight
     */
    public static void keepScreenLongLight(Activity activity, boolean isOpenLight) {
        Window window = activity.getWindow();
        if (isOpenLight) {
            window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        } else {
            window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
    }

    /**
     * 设置全屏显示.
     */
    private void setFullScreen() {
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        Window window = getWindow();
        WindowManager.LayoutParams params = window.getAttributes();
        params.systemUiVisibility = View.SYSTEM_UI_FLAG_LOW_PROFILE;
        window.setAttributes(params);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        int vis = getWindow().getDecorView().getSystemUiVisibility();
        getWindow().getDecorView()
                .setSystemUiVisibility(vis | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE);

        setContentView(R.layout.vmi_activity_fullscreen);
        WindowManager wm = (WindowManager) getApplicationContext().getSystemService(WINDOW_SERVICE);
        wm.getDefaultDisplay().getRealMetrics(metric);
        LogUtils.info(TAG, "width pixels:" + metric.widthPixels + ", height pixels:" + metric.heightPixels
            + ",densityDpi:" + metric.densityDpi);
        guestWidth = metric.widthPixels;
        guestHeight = metric.heightPixels;
    }

    @Override
    public void onStart() {
        super.onStart();
        LogUtils.info(TAG, "onStart");
        FloatingViewManager.getInstance().attach(this);
        int ret = AudioTrackPlayer.startAudio();
        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            LogUtils.info(AUDIO_TAG, "VMI_SUCCESS start Audio success ");
        }

        if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_START_FAIL) {
            LogUtils.error(AUDIO_TAG, "VMI_AUDIO_ENGINE_CLIENT_START_FAIL start Audio error ");
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        LogUtils.info(TAG, "onStop");
        FloatingViewManager.getInstance().detach(this);
        int ret = AudioTrackPlayer.stopAudio();
        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            LogUtils.info(AUDIO_TAG, "VMI_SUCCESS stop Audio success ");
        }
        if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL) {
            LogUtils.error(AUDIO_TAG, "VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL, stop Audio error ");
        }

    }

    @Override
    public void onPause() {
        super.onPause();
        LogUtils.info(TAG, "onPause");
        if (upstreamReceiveDispatcher != null) {
            upstreamReceiveDispatcher.silentAudio(true);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        LogUtils.info(TAG, "onResume");
        if (upstreamReceiveDispatcher != null) {
            upstreamReceiveDispatcher.silentAudio(false);
        }
    }

    void checkPermissions() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) !=
                PackageManager.PERMISSION_GRANTED) {
            checkingPermission = true;
            ActivityCompat.requestPermissions(this, new String[]{"Manifest.permission.RECORD_AUDIO"},
                    CODE_AUDIO_RECORD);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[]permissions, int[] grantResults) {
        switch (requestCode) {
            case CODE_AUDIO_RECORD: {
                // If request is cancelled, the result arrays are empty.
                if (!(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)) {
                    showPopup(getStringFromRes(R.string.msg_audio), -1);
                }
                return;
            }
            default:
                break;
        }
        checkingPermission = false;
    }

    /**
     * show pop dialog.
     *
     * @param msg msg to display
     * @param aliveTime how long will it last on screen
     */
    public void showPopup(String msg, int aliveTime) {
        Message message = Message.obtain();
        Bundle bundle = new Bundle();
        bundle.putString("Tip", msg);
        bundle.putInt("aliveTime", aliveTime);
        message.setData(bundle);
        popWindowHandler.sendMessage(message);
    }

    /**
     * sleep milliseconds.
     *
     * @param seconds number of milliseconds
     */
    private void sleep(int seconds) {
        try {
            Thread.sleep(seconds);
        } catch (InterruptedException e) {
            LogUtils.error(TAG, "sleep with InterruptedException.");
        }
    }

    /**
     * 表示帧率视图是否可见。
     *
     * @return true or false
     */
    public boolean isShowState() {
        return isShowState;
    }

    /**
     * 设置帧率视图是否可见
     *
     * @param showState true or false
     */
    public void setShowState(boolean showState) {
        isShowState = showState;
    }
}
