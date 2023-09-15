package com.huawei.baseuidemo;

import static java.lang.Thread.sleep;

import android.Manifest;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.huawei.baseuidemo.audioplay.AudioPlayerCallback;
import com.huawei.baseuidemo.engine.InstructionEngine;
import com.huawei.baseuidemo.engine.InstructionWrapper;
import com.huawei.baseuidemo.engine.JniCallBack;
import com.huawei.baseuidemo.engine.NativeListener;
import com.huawei.baseuidemo.maincontrol.DataPipe;
import com.huawei.baseuidemo.maincontrol.NetConfig;
import com.huawei.baseuidemo.maincontrol.UpStreamReceiveDispatcher;
import com.huawei.baseuidemo.utils.ThreadPool;
import com.huawei.baseuidemo.widget.VmiSurfaceView;
import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudgame.touch.VmiTouch;

import java.text.SimpleDateFormat;
import java.util.Date;

public class BaseUiActivity extends AppCompatActivity implements NativeListener {

    private static final String TAG = "VmiBasePage";
    private String proxyIp;
    private int proxyPort;
    private VmiSurfaceView surfaceView;
    public volatile boolean stopFlag = true;
    public volatile boolean stopped = true;
    public int orientation = -1;
    private Boolean newlyCreated = true; // 渲染线程标志位
    boolean checkingPermission = false;
    private int engineOrientation;
    private int guestWidth = 1080;
    private int guestHeight = 1920;
    private final long lastTxBytes = 0;
    private final long lastRxBytes = 0;
    private long totalBandWidthBytes = 0;
    DisplayMetrics metric = new DisplayMetrics();
    private final ThreadPool threadPool = new ThreadPool(4, 20);
    private final SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss.SSS");

    private final InstructionEngine engine = new InstructionEngine(); // 新建一个指令流引擎对象
    private final JniCallBack jniCallback = new JniCallBack(); // 新建一个指令流引擎Jni回调对象
    UpStreamReceiveDispatcher upStreamReceiveDispatcher = null;
    AudioPlayerCallback audioPlayerCallback = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_base_ui);

        // 设置Activity全屏
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        int vis = getWindow().getDecorView().getSystemUiVisibility();
        getWindow().getDecorView().setSystemUiVisibility(vis| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_IMMERSIVE);

        // 设置Activity方向
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        Intent intent = getIntent();
        String address = intent.getStringExtra("agentAddress");
        parseProxyAddress(address); // 解析ip和端口号
        setUpInsCallback(); // 设置指令流引擎回调对象
        initDataPipe(); // 初始化注册音频和触控接口
        setUpIPAndPort(); // 原型设置IP和端口
        setUpInitialize(); // 初始化指令流引擎，传入IP、端口以及服务端云手机分辨率信息
        setUpSurfaceView(); // 设置surface view 并启动引擎
        checkPermission(); // 检查权限
    }

    /**
     * 解析proxy地址
     *
     * @param address
     */
    private void parseProxyAddress(String address) {
        proxyIp = address.split(":")[0];
        proxyPort = Integer.parseInt(address.split(":")[1]);
        Log.i(TAG, "login in ip: " + proxyIp + "port: " + proxyPort);
    }

    /**
     * 设置指令流引擎回调对象
     */
    private void setUpInsCallback() {
        jniCallback.setNativeCallback();
        jniCallback.setNativeListener(this);
    }

    /**
     * 初始化注册音频和触控接口
     */
    private void initDataPipe() {
        // 设置指令流引擎对象
        DataPipe.setInstructionEngine(engine);
        // 注册触控引擎
        DataPipe.registerHookToTouch();
        // 注册音频引擎
        DataPipe.registerAudioSendHook();
    }

    /**
     * 原型设置IP和端口
     */
    private void setUpIPAndPort() {
        // 通信库初始化失败，退出activity
        if (!NetConfig.initialize()) {
            Log.e(TAG, "NetConfig initialize failed.");
            Toast.makeText(getApplicationContext(), "通信库初始化失败，请检查流程",
                    Toast.LENGTH_LONG).show();
        }
        // 通信库设置IP和端口，第三个参数为通信协议类型，0是tcp
        if (!NetConfig.setNetConfig(proxyIp, proxyPort, 0)) {
            Log.e(TAG, "Set netconfig failed, " + "ip: " + proxyIp + ", port: " + proxyPort);
            Toast.makeText(getApplicationContext(), "通信库配置IP和端口失败，请检查流程",
                    Toast.LENGTH_LONG).show();
        }
    }

    /**
     * 初始化指令流引擎，传入IP、端口以及服务端云手机分辨率信息
     */
    private void setUpInitialize() {
        int initResult = engine.initialize();
        if (initResult != InstructionEngine.VMI_SUCCESS) {
            // 初始化失败，退出处理
            Log.e(TAG, "InstructionEngine initialize failed, result: " + initResult);
            Toast.makeText(getApplicationContext(), "引擎初始化失败，请检查流程",
                    Toast.LENGTH_LONG).show();
            finish();
        }
    }

    /**
     * 预先启动接收的逻辑线程，并注册音频和录音的回调对象
     */
    private void setUpRecvData() {
        upStreamReceiveDispatcher = UpStreamReceiveDispatcher.getInstance();
        upStreamReceiveDispatcher.setInstructionEngine(engine);
        // 音频数据回调
        audioPlayerCallback = new AudioPlayerCallback();
        // 注册音频回调
        upStreamReceiveDispatcher.addNewPacketCallback((byte) InstructionWrapper.AUDIO,
                audioPlayerCallback);
        // 获取引擎统计信息
        setUpGetStat();
        // 启动逻辑线程接收音频数据
        upStreamReceiveDispatcher.start();
    }

    /**
     * 设置surface view并启动引擎
     */
    private void setUpSurfaceView() {
        if (findViewById(R.id.vmi_surfaceView) instanceof VmiSurfaceView) {
            surfaceView = (VmiSurfaceView) findViewById(R.id.vmi_surfaceView);
        } else {
            Log.e(TAG, "surface view is start fail.");
            return;
        }
        setUpSurfaceViewListeners();
        initSurfaceHolder();
    }

    /**
     * 设置触控引擎关联View
     */
    private void setUpSurfaceViewListeners() {
        surfaceView.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View view, MotionEvent event) {
                return VmiTouch.getInstance().onTouch(view, event);
            }
        });
    }

    private void initSurfaceHolder() {
        WindowManager wm = (WindowManager) getApplicationContext().getSystemService(WINDOW_SERVICE);
        wm.getDefaultDisplay().getRealMetrics(metric);
        Log.i(TAG, "width pixels: " + metric.widthPixels + ", height pixels: " +
                metric.heightPixels + ", densityDpi: " + metric.densityDpi);
        guestWidth = metric.widthPixels;
        guestHeight = metric.heightPixels;
        surfaceView.initialize(guestWidth, guestHeight);
        initSurfaceHolderCallback(surfaceView.getHolder());
    }

    private void initSurfaceHolderCallback(SurfaceHolder surfaceHolder) {
        // final SurfaceHolder surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
                // 首次创建
                Log.i(TAG, "surfaceCreated is first create.");
                newlyCreated = true;
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder inSurfaceHolder,
                                       int format, int width, int height) {
                if (!newlyCreated) {
                    return;
                }
                newlyCreated = false;
                // 由于启动接口涉及socket连接，建议放到线程中调用
                Log.i(TAG, "surfaceChanged is change.");
                threadPool.submit(new ConnectRunnable());
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
                // Surface销毁时触发，渲染线程标志置false，以便结束线程
                Log.i(TAG, "surfaceDestroyed is destroyed.");
                newlyCreated = false;
            }
        });
    }

    class ConnectRunnable implements Runnable {

        @Override
        public void run() {
            // 调用指令流引擎启动接口
            final int startResult = engine.start(surfaceView.getHolder().getSurface(),
                    guestWidth, guestHeight, metric.densityDpi);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (startResult == InstructionEngine.VMI_INSTRUCTION_CLIENT_START_FAIL) {
                        // 连接失败
                        Log.e(TAG, "instruction start failed, result: " + startResult);
                        Toast.makeText(getApplicationContext(), "引擎连接失败",
                                Toast.LENGTH_LONG).show();
                    } else {
                        // 连接成功或者已经连接
                        Log.d(TAG, "instruction start success, result: " + startResult);
                        setUpRecvData();
                    }
                }
            });
        }
    }

    /**
     * 获取指令流引擎的运行时统计信息
     */
    private void setUpGetStat() {
        stopFlag = false;
        stopped = false;
        threadPool.submit(new StartPhoneRunnable());
    }

    class StartPhoneRunnable implements Runnable {

        @Override
        public void run() {
            while (!stopFlag) {
                if (!stopFlag) {
                    try {
                        sleep(1000);
                    } catch (InterruptedException e) {
                        Log.e(TAG, "sleep interrupted.");
                    }
                }
                // 定时查询引擎统计信息
                String nativeStatString = engine.getStat();
                long bandWidthBytes = lastRxBytes + lastTxBytes;
                totalBandWidthBytes += bandWidthBytes;
                if (!nativeStatString.isEmpty()) {
                    String lagString = nativeStatString.substring(nativeStatString.indexOf("LAG: ") + 4,
                            nativeStatString.indexOf("ms"));
                    String subStartString = nativeStatString.substring(nativeStatString.indexOf("LAG: "),
                            nativeStatString.indexOf("LAG: ") + 5);
                    String subEndString = nativeStatString.substring(nativeStatString.indexOf("ms"));
                    final int lagFromNative = Integer.parseInt(lagString.trim());
                    int milliToMicro = 1000;
                    int lag = lagFromNative / milliToMicro;
                    final String nativeString = subStartString + " " + lag + subEndString;
                    final String statString = nativeString + "带宽： " + Long.toString(bandWidthBytes / 1024) + "KB\n流量： " +
                            Long.toString(totalBandWidthBytes / 1024 / 1024) + "MB\n时间： " +
                            simpleDateFormat.format((new Date()));
                    Log.i(TAG, "接收的数据： \n" + statString);
                }
            }
            stopped = true;
        }
    }

    /**
     * 停止指令流引擎
     */
    private void setUpStopPhone() {
        Log.i(TAG, "stop cloud phone.");
        threadPool.submit(new StopPhoneRunnable());
    }

    class StopPhoneRunnable implements Runnable {

        @Override
        public void run() {
            Log.i(TAG, "stop phone start.");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    // 停止接收等逻辑线程
                    upStreamReceiveDispatcher.stopRunnable();
                    upStreamReceiveDispatcher = null;
                    Log.d(TAG, "stop phone end.");
                }
            });
            engine.stop();
            Log.i(TAG, "stop engine.");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    finish();
                    android.os.Process.killProcess(android.os.Process.myPid());
                }
            });
            Log.i(TAG, "kill process.");
        }
    }

    /**
     * 重连指令流云手机流程
     */
    private void ReConnectPhone() {
        if (!NetConfig.setNetConfig(proxyIp, proxyPort, 0)) {
            Log.e(TAG, "set netconfig failed, " + "ip: " + proxyIp + ", port: " + proxyPort);
            Toast.makeText(getApplicationContext(), "重连：设置ip和port失败，请检查流程",
                    Toast.LENGTH_LONG).show();
            finish();
        }
        threadPool.submit(new ReConnectRunnable());
    }

    class ReConnectRunnable implements Runnable {

        @Override
        public void run() {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    // 停止指令流引擎
                    engine.stop();
                    // 初始化指令流引擎
                    int initResult = engine.initialize();
                    if (initResult != InstructionEngine.VMI_SUCCESS) {
                        // 初始化指令流引擎失败
                        Log.e(TAG, "initialize engine failed, result: " + initResult);
                        Toast.makeText(getApplicationContext(), "重连：初始化引擎失败，请检查流程",
                                Toast.LENGTH_LONG).show();
                        finish();
                    }
                    // 启动指令流引擎
                    int startResult = engine.start(surfaceView.getHolder().getSurface(), guestWidth,
                            guestHeight, metric.densityDpi);
                    if (startResult == InstructionEngine.VMI_INSTRUCTION_CLIENT_START_FAIL) {
                        // 启动指令流引擎失败
                        Log.e(TAG, "start engine failed, result: " + startResult);
                        Toast.makeText(getApplicationContext(), "重连：启动引擎失败，请检查流程",
                                Toast.LENGTH_LONG).show();
                    } else {
                        // 启动指令流引擎成功，启动接收线程
                        Log.i(TAG, "start engine success, result: " + startResult);
                        setUpRecvData();
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
                if (orientation != rota) {
                    Log.i(TAG, "client begin rotate: " + orientation + " ---> " + rota);
                    orientation = rota;
                    switch (orientation) {
                        case Surface.ROTATION_0:
                            // surfaceView ui旋转
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION0);
                            // 设置屏幕方向
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                            break;
                        case Surface.ROTATION_90:
                            // surfaceView ui旋转
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION90);
                            // 设置屏幕方向
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                            break;
                        case Surface.ROTATION_180:
                            // surfaceView ui旋转
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION180);
                            // 设置屏幕方向
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT);
                            break;
                        case Surface.ROTATION_270:
                            // surfaceView ui旋转
                            surfaceView.setScreenRotation(InstructionEngine.Rotation.ROTATION270);
                            // 设置屏幕方向
                            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
                            break;
                        default:
                            break;
                    }
                }
            }
        });
    }

    /**
     * 检查权限
     */
    private void checkPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) !=
                PackageManager.PERMISSION_GRANTED) {
            checkingPermission = true;
            ActivityCompat.requestPermissions(this,
                    new String[]{"Manifest.permission.RECORD_AUDIO"}, 1);
        }
    }

    /**
     * 开启声音
     */
    public void onStart() {
        super.onStart();
        Log.i(TAG, "on start.");
        int ret = AudioTrackPlayer.startAudio();
        // 音频开启成功
        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            Log.i(TAG, "start audio success.");
        }
        // 音频开启失败
        if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_START_FAIL) {
            Log.e(TAG, "start audio failed.");
        }
    }

    /**
     * 开启静音
     */
    public void onStop() {
        super.onStop();
        Log.i(TAG, "on stop.");
        int ret = AudioTrackPlayer.stopAudio();
        // 音频静音成功
        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            Log.i(TAG, "stop audio success.");
        }
        // 音频静音失败
        if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL) {
            Log.e(TAG, "stop audio failed.");
        }
    }

    /**
     * 实现NativeListener接口
     *
     * @param event
     * @param reserved0
     * @param reserved1
     * @param reserved2
     * @param reserved3
     */
    public void onVmiInstructionEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3){
        // 处理上报事件
        switch (event) {
            // 网络断开
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN:
                Log.i(TAG, "network is disconnect.");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        // 停止逻辑线程
                        stopFlag = true;
                        if (upStreamReceiveDispatcher != null) {
                            upStreamReceiveDispatcher.stopRunnable();
                            upStreamReceiveDispatcher = null;
                        }
                        // 执行指令流重连流程
                        Log.i(TAG, "network is disconnect after, reconnecting.");
                        Toast.makeText(getApplicationContext(), "网络连接失败，重连中",
                                Toast.LENGTH_LONG).show();
                        ReConnectPhone();
                        // 或者退出云手机
                        //Log.i(TAG, "network is disconnect after, stop.");
                        //Toast.makeText(getApplicationContext(), "网络连接失败，重连中",
                        //        Toast.LENGTH_LONG).show();
                        //setUpStopPhone();
                    }
                });
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN:
                Log.i(TAG, "network connection is unstable.");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "网络连接不稳定", 
                                Toast.LENGTH_LONG).show();
                    }
                });
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_VERSION_ERROR:
                Log.i(TAG, "The server and client version of the instruction flow engine do not match.");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "服务端和客户端的版本不匹配", 
                                Toast.LENGTH_LONG).show();
                        setUpStopPhone();
                    }
                });
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_READY:
                Log.i(TAG, "instruction render success.");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "引擎渲染第一帧画面成功", 
                                Toast.LENGTH_LONG).show();
                    }
                });
                break;
            case InstructionEngine.VMI_INSTRUCTION_ENGINE_EVENT_ORIENTATION_CHANGED:
                Log.i(TAG, "instruction orientation.");
                engineOrientation = reserved0;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        setRotation(engineOrientation);
                    }
                });
                break;
            default:
                break;
        }
    }
}