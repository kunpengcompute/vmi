package com.huawei.cloudphone;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.net.TrafficStats;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.cloudphoneui.R;
import com.huawei.cloudgame.touch.VmiTouch;
import com.huawei.cloudphone.dialog.ExitDialog;
import com.huawei.cloudphone.view.VmiSurfaceView;
import com.huawei.cloudphone.helper.FloatingHelper;
import com.huawei.cloudphonesdk.audio.play.AudioPlayerCallback;
import com.huawei.cloudphonesdk.maincontrol.DataPipe;
import com.huawei.cloudphonesdk.maincontrol.NativeListener;
import com.huawei.cloudphonesdk.maincontrol.NetConfig;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniCallback;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniWrapper;
import com.huawei.cloudphonesdk.maincontrol.UpstreamReceiveDispatcher;
import com.huawei.utils.LogUtil;
import com.huawei.utils.Utils;

import java.lang.ref.WeakReference;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
public class FullscreenActivity extends AppCompatActivity implements NativeListener {
    private static final int WHAT_CHECK_CONNECT_STATUS = 1;
    private static final int WHAT_RECONNECT = 2;
    private static final int WHAT_CONNECT_SUCCESS = 3;
    private static final int WHAT_CONNECT_FAILED = 4;
    private static final int WHAT_RECONNECT_FAILED = 5;
    private static final int SOCKET_DISCONNECTION = -2;
    private static final int ORIENTATION_CHANGE = -6;
    public static final String IS_SHOW_FLOAT_VIEW = "is_show_float_view";
    private VideoConf mVideoConf;
    private VmiSurfaceView mSurfaceView;
    private ImageView flashNormal;
    public static final String TAG = "FULLSCREEN_ACTIVITY";
    private boolean isSimulator = false;
    private int guestWidth = 1080;
    private int guestHeight = 1920;
    private float displayWidth = 1080.0f;
    DisplayMetrics metrics = new DisplayMetrics();
    private ThreadPool mThreadPool = new ThreadPool(4, 20);
    ThreadHandler mThreadHandler;
    private UpstreamReceiveDispatcher mUpstreamDispatcher;
    int engineOrientation;
    private int orientation = -1;
    private boolean stopHandlerFlag = false;
    private TextView frameRateTextView;
    private long lastRxBytes;
    private long lastTxBytes;
    private FloatingHelper floatingHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        LogUtil.startLogs(LogUtil.CLIENT_LOG);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        Utils.setNavigationBarVisible(this, true);
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_SECURE);
        setFullScreen();
        setContentView(R.layout.activity_fullscreen);
        mThreadHandler = new ThreadHandler(this);
        initView();
    }

    @Override
    protected void onStart() {
        super.onStart();
        Utils.setNavigationBarVisible(this, true);
    }

    private void setFullScreen() {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        Window window = getWindow();
        WindowManager.LayoutParams params = window.getAttributes();
        params.systemUiVisibility = View.SYSTEM_UI_FLAG_LOW_PROFILE;
        window.setAttributes(params);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        int vis = getWindow().getDecorView().getSystemUiVisibility();
        getWindow().getDecorView().setSystemUiVisibility(vis | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE);

        WindowManager wm = (WindowManager) getApplicationContext().getSystemService(WINDOW_SERVICE);
        wm.getDefaultDisplay().getRealMetrics(metrics);

        guestWidth = metrics.widthPixels;
        guestHeight = metrics.heightPixels;
    }

    private void setupFloatingButton() {
        floatingHelper = new FloatingHelper(this);
        floatingHelper.init();
    }

    private void setupSurfaceHolder() {
        SurfaceHolder surfaceHolder = mSurfaceView.getHolder();
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mSurfaceView.getHolder().setFixedSize(guestWidth, guestHeight);
        mSurfaceView.initialize(guestWidth, guestHeight);
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                startConnect();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                stopMonitor();
                OpenGLJniWrapper.stop();
            }
        });
    }

    private void stopMonitor() {
        mThreadHandler.removeCallbacksAndMessages(null);
        if (mUpstreamDispatcher != null) {
            mUpstreamDispatcher.stopBlocked();
            mUpstreamDispatcher = null;
        }
    }

    private void startConnect() {
        stopHandlerFlag = false;
        setupMediaCodec();
        setupJniConf();
        mThreadPool.submit(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if (mSurfaceView == null) {
                    return;
                }
                Log.i(TAG, "run: guestWidth:" + guestWidth + ", guestHeight" + guestHeight + ",densityDpi:" + metrics.densityDpi);
                int startResult = OpenGLJniWrapper.start(mSurfaceView.getHolder().getSurface(), guestWidth, guestHeight, metrics.densityDpi);
                if (startResult == 0) {
                    mThreadHandler.sendEmptyMessageDelayed(WHAT_CONNECT_SUCCESS, 2000);
                    mThreadHandler.sendEmptyMessage(WHAT_CHECK_CONNECT_STATUS);
                    Log.i(TAG, "run: 连接成功");
                } else {
                    Log.e(TAG, "run: 连接失败");
                    mThreadHandler.sendEmptyMessage(WHAT_CONNECT_FAILED);
                }
            }
        });
    }

    public View getFrameRateView() {
        return frameRateTextView;
    }

    static class ThreadHandler extends Handler {

        private WeakReference<FullscreenActivity> weakReference;

        ThreadHandler(FullscreenActivity activity) {
            this.weakReference = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            FullscreenActivity activity = weakReference.get();
            if (activity == null || activity.isFinishing()) {
                return;
            }

            switch (msg.what) {
                case WHAT_CHECK_CONNECT_STATUS: {
                    if (!activity.stopHandlerFlag) {
                        activity.checkConnectStatus();
                    }
                    break;
                }
                case WHAT_RECONNECT: {
                    break;
                }
                case WHAT_CONNECT_SUCCESS: {
                    if (!activity.stopHandlerFlag) {
                        activity.startMonitor();
                    }
                    break;
                }
                case WHAT_CONNECT_FAILED: {
                    activity.showConnectFailedDialog();
                    break;
                }
                case WHAT_RECONNECT_FAILED: {
                    activity.showConnotReconnectDialog();
                    break;
                }
                default:
                    break;
            }
        }
    }

    private void showConnotReconnectDialog() {

    }

    private void showConnectFailedDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
        builder.setMessage("连接云手机服务器失败，请稍后重试");
        builder.setTitle("alert");
        builder.setPositiveButton("ok", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                finish();
            }
        });
        builder.show();
    }

    @Override
    public void finish() {
        android.os.Process.killProcess(android.os.Process.myPid());
        super.finish();
    }

    private void startMonitor() {
        mUpstreamDispatcher = new UpstreamReceiveDispatcher();
        mUpstreamDispatcher.addNewPacketCallback(OpenGLJniWrapper.AUDIO, new AudioPlayerCallback());
        mUpstreamDispatcher.start();
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                flashNormal.setVisibility(View.INVISIBLE);
            }
        });
    }

    private void checkConnectStatus() {
        refreshFrameRate();
        mThreadHandler.sendEmptyMessageDelayed(WHAT_CHECK_CONNECT_STATUS, 1000);
    }

    private void refreshFrameRate() {
        int uid = getApplicationInfo().uid;
        long currentRxBytes = TrafficStats.getUidRxBytes(uid);
        long currentTxBytes = TrafficStats.getUidTxBytes(uid);
        long totalBytes = currentRxBytes + currentTxBytes;
        if (lastRxBytes == 0 && lastTxBytes == 0) {
            lastRxBytes = currentRxBytes;
            lastTxBytes = currentTxBytes;
        }
        long bandWidthBytes = currentRxBytes + currentRxBytes - lastTxBytes - lastRxBytes;
        int bandWidth = (int) (bandWidthBytes / 1024);
        int trafficSum = (int) (totalBytes / 1024 / 1024);
        String stat = OpenGLJniWrapper.getStatistics();
        int lag = 0;
        if (!stat.isEmpty()) {
            String receiveFps = stat.substring(stat.indexOf("接受帧率"));
            String lagString = stat.substring(stat.indexOf("LAG:") + 4, stat.indexOf("ms"));
            if (lagString.length() > 9) {
                lag = -1;

            } else {
                final int lagFromNative = Integer.parseInt(lagString.trim());
                lag = lagFromNative / 1000;
            }
            String statString = "LAG:" + lag + "ms" + System.lineSeparator() + receiveFps
                    + "带宽：" + bandWidth + "KB" + System.lineSeparator()
                    + "流量：" + totalBytes / 1024 / 1024 + "MB";
            frameRateTextView.setText(statString);
            updateFloater(lag);
        }
        lastRxBytes = currentRxBytes;
        lastTxBytes = currentTxBytes;

    }

    private void updateFloater(int lag) {
        if (lag < 0) {
            floatingHelper.setMainImage(R.drawable.image_menu_red);
        } else if (lag < 80) {
            floatingHelper.setMainImage(R.drawable.image_menu_green);
        } else if (lag < 180) {
            floatingHelper.setMainImage(R.drawable.image_menu_yellow);
        } else {
            floatingHelper.setMainImage(R.drawable.image_menu_red);
        }
    }

    private void setupJniConf() {
        if (!NetConfig.initialize()) {
            Log.e(TAG, "setupJniConf:net initialize failed ");
            finish();
            return;
        }
        Log.d(TAG, "setupJniConf: " + mVideoConf.getIp() + ":" + mVideoConf.getPort());
        if (!NetConfig.setNetConfig(mVideoConf.getIp(), mVideoConf.getPort(), 0)) {
            Log.d(TAG, "setupJniConf: " + mVideoConf.getIp() + ":" + mVideoConf.getPort());
            finish();
        }
        int initResult = OpenGLJniWrapper.initialize();
        if (initResult != 0) {
            Log.e(TAG, "setupJniConf:jni initialize failed ");
            finish();
            return;
        }
    }

    private void setupMediaCodec() {

    }

    private void checkSimulator() {
        if (Build.MODEL.contains("Monbox") || (Build.MODEL.contains("Kbox"))) {
            isSimulator = true;
        }
    }

    private void initDataPipe() {
        if (mVideoConf == null) {
            Log.e(TAG, "initDataPipe: ");
            finish();
            return;
        }
        DataPipe.registerHookToTouch();
        DataPipe.registerHookToAudio();
    }

    @SuppressLint("ClickableViewAccessibility")
    private void setupTouch() {
        mSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                VmiTouch.getInstance().onTouch(v, event);
                floatingHelper.closeFloatButton();
                return true;
            }
        });
    }

    private void setupView() {
        mSurfaceView = findViewById(R.id.vmi_surfaceView);
        flashNormal = findViewById(R.id.flashNormal);
        frameRateTextView = findViewById(R.id.frameRate);
        if (Utils.getBoolean("show_frameRate", false)) {
            frameRateTextView.setVisibility(View.VISIBLE);
        } else {
            frameRateTextView.setVisibility(View.GONE);
        }
    }

    private void initView() {
        setupConfig();
        setupView();
        setupTouch();
        initDataPipe();
        checkSimulator();
        setupFloatingButton();
        setupSurfaceHolder();
        setListener();
    }

    private void setListener() {
        OpenGLJniCallback callback = new OpenGLJniCallback();
        callback.setObj();
        callback.setNativeListener(this);
    }

    @Override
    public void onBackPressed() {
        showExitDialog();
    }

    public void showExitDialog() {
//        AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
//        builder.setMessage("是否退出云手机？");
//        builder.setTitle("warning");
//        builder.setNegativeButton("确定", new DialogInterface.OnClickListener() {
//            @Override
//            public void onClick(DialogInterface dialog, int which) {
//                finish();
//
//            }
//        });
//        builder.setPositiveButton("取消", new DialogInterface.OnClickListener() {
//            @Override
//            public void onClick(DialogInterface dialog, int which) {
//            }
//        });
//        builder.setCancelable(false);
//        builder.create().show();
        ExitDialog exitDialog = new ExitDialog(this);
        exitDialog.setPositiveButton(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                exitDialog.dismiss();
            }
        });
        exitDialog.setNegativeButton(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                finish();
            }
        });
        exitDialog.show();
    }

    private void setupConfig() {
        Intent intent = getIntent();
        if (intent != null) {
            mVideoConf = intent.getParcelableExtra("video_conf");
            DisplayMetrics metrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
            mVideoConf.setWidth(Math.min(metrics.widthPixels, metrics.heightPixels));
            mVideoConf.setHeight(Math.min(metrics.widthPixels, metrics.heightPixels));
            mVideoConf.setDensity(metrics.densityDpi);
        }
    }

    public void sendKeyEvent(int keycode, int action) {
        byte[] bytes = new byte[4];
        bytes[0] = (byte) keycode;
        bytes[1] = (byte) (keycode >> 8);
        bytes[2] = (byte) action;
        bytes[3] = (byte) (action >> 8);
        OpenGLJniWrapper.sendKeyEvent(bytes, 4);
    }

    @Override
    public void onVmiVideoEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3) {
        switch (event) {
            case SOCKET_DISCONNECTION:
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Log.d(TAG, "run: 网络断开回调");
                        stopHandlerFlag = true;
                        flashNormal.setVisibility(View.VISIBLE);
                        showReconnectDialog();
                    }
                });
                stopMonitor();
                OpenGLJniWrapper.stop();
                break;
            case ORIENTATION_CHANGE:
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
            default:
                break;
        }
    }

    private void setRotation(int rotation) {
        final int rota = rotation;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (orientation != rota) {
                    orientation = rota;
                    switch (orientation) {
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

    private void showReconnectDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
        builder.setMessage("网络异常断开，重连或退出云手机？");
        builder.setTitle("warning");
        builder.setNegativeButton("重连云手机", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                startConnect();
            }
        });
        builder.setPositiveButton("退出云手机", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                finish();
            }
        });
        builder.setCancelable(false);
        builder.create().show();
    }
}