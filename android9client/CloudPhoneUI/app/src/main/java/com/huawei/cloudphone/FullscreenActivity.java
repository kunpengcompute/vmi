package com.huawei.cloudphone;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Range;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.example.cloudphoneui.R;
import com.huawei.cloudphonesdk.audio.play.AudioPlayerCallback;
import com.huawei.cloudphonesdk.audio.record.AudioRecorderCallback;
import com.huawei.cloudphonesdk.maincontrol.Constant;
import com.huawei.cloudphonesdk.maincontrol.DataPipe;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniCallback;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniWrapper;
import com.huawei.cloudphonesdk.maincontrol.UpstreamReceiveDispatcher;
import com.huawei.cloudphonesdk.touch.EventHelper;

import java.io.IOException;
import java.lang.ref.WeakReference;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
public class FullscreenActivity extends AppCompatActivity {
    private static final int WHAT_CHECK_CONNECT_STATUS = 1;
    private static final int WHAT_RECONNECT = 2;
    private static final int WHAT_CONNECT_SUCCESS = 3;
    private static final int WHAT_CONNECT_FAILED = 4;
    private static final int WHAT_RECONNECT_FAILED = 5;
    private static final int SOCKET_DISCONNECTION = -2;
    private VideoConf mVideoConf;
    private VmiSurfaceView mSurfaceView;
    private ImageView flashNormal;
    private EventHelper eventHelper;
    public static final String TAG = "FULLSCREEN_ACTIVITY";

    DisplayMetrics metrics = new DisplayMetrics();
    private ThreadPool mThreadPool = new ThreadPool(4, 20);
    ThreadHandler mThreadHandler;
    private UpstreamReceiveDispatcher mUpstreamDispatcher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setFullScreen();
        setContentView(R.layout.activity_fullscreen);
        mThreadHandler = new ThreadHandler(this);
        initView();
    }

    private void setFullScreen() {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        Window window = getWindow();
        WindowManager.LayoutParams params = window.getAttributes();
        params.systemUiVisibility = View.SYSTEM_UI_FLAG_LOW_PROFILE;
        window.setAttributes(params);
        int vis = getWindow().getDecorView().getSystemUiVisibility();
        getWindow().getDecorView().setSystemUiVisibility(vis | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE);
    }

    private void setupFloatingButton() {
    }

    private void setupSurfaceHolder() {
        SurfaceHolder surfaceHolder = mSurfaceView.getHolder();
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                Log.i(TAG, "surfaceCreated: ");
                holder.setFixedSize(mVideoConf.getWidth(), mVideoConf.getHeight());
                startConnect();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                Log.i(TAG, "surfaceChanged: ");
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                Log.i(TAG, "surfaceDestroyed: ");
                stopMonitor();
                OpenGLJniWrapper.stop();
            }
        });
    }

    private void stopMonitor() {
        Log.i(TAG, "stopMonitor: ");
        mThreadHandler.removeCallbacksAndMessages(null);
        if (mUpstreamDispatcher != null) {
            mUpstreamDispatcher.stopBlocked();
            mUpstreamDispatcher = null;
        }
    }

    private void startConnect() {
        setupMediaCodec();
        setupJniConf();
        mThreadPool.submit(new Runnable() {
            @Override
            public void run() {
                int startResult = OpenGLJniWrapper.startc(mSurfaceView.getHolder().getSurface());
                int jniStatus = OpenGLJniWrapper.getJniStatus();
                if (startResult == 0 && jniStatus == Constant.JNI_STATUS_RUNNING) {
                    mThreadHandler.sendEmptyMessage(WHAT_CONNECT_SUCCESS);
                    Log.i(TAG, "run: 连接成功");
                } else {
                    Log.e(TAG, "run: 连接失败");
                    mThreadHandler.sendEmptyMessage(WHAT_CONNECT_FAILED);
                }
            }
        });
    }

    private void setupJniConf() {
        OpenGLJniWrapper.setDecodeMode(Constant.DECODE_METHOD_HARDWARE);
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_IP, mVideoConf.getIp());
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_PORT, String.valueOf(mVideoConf.getPort()));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_VSS_PORT, String.valueOf(mVideoConf.getAudioPort()));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_GAME_ID, String.valueOf(0));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_ENCODE_MODE, String.valueOf(1));

        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_ENCODE_FRAMERATE, String.valueOf(0));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_ENCODE_BITRATE, String.valueOf(0));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_ENCODE_PROFILE, String.valueOf(0));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_ENCODE_GOPSIZE, String.valueOf(0));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_VIDEO_WIDTH, String.valueOf(mVideoConf.getWidth()));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_VIDEO_HEIGHT, String.valueOf(mVideoConf.getHeight()));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_VIDEO_DENSITY, String.valueOf(mVideoConf.getDensity()));
        OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_LOOP_SIZE, String.valueOf(0));
        if (mWidthRange != null & mHeightRange != null) {
            OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_RANGE_WIDTH_MIN, Integer.toString(mWidthRange.getLower()));
            OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_RANGE_WIDTH_MAX, Integer.toString(mWidthRange.getUpper()));
            OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_RANGE_HEIGHT_MIN, Integer.toString(mHeightRange.getLower()));
            OpenGLJniWrapper.setJniConf(OpenGLJniWrapper.KEY_RANGE_HEIGHT_MAX, Integer.toString(mHeightRange.getUpper()));
        }
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
                    activity.checkConnectStatus();
                    break;
                }
                case WHAT_RECONNECT: {
                    break;
                }
                case WHAT_CONNECT_SUCCESS: {
                    activity.startMonitor();
                    break;
                }
                case WHAT_CONNECT_FAILED: {
                    activity.showConnectFailedDialog();
                    break;
                }
                case WHAT_RECONNECT_FAILED: {
//                    activity.showConnotReconnectDialog();
                    break;
                }
                default:
                    break;
            }
        }
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
        builder.setCancelable(false);
        builder.show();
    }

    @Override
    public void finish() {
        android.os.Process.killProcess(android.os.Process.myPid());
        super.finish();
    }

    private void startMonitor() {
        Log.i(TAG, "startMonitor: ");
        mUpstreamDispatcher = new UpstreamReceiveDispatcher();
        mUpstreamDispatcher.addNewPacketCallback(OpenGLJniWrapper.ORIENTATION,mSurfaceView);
        mUpstreamDispatcher.addNewPacketCallback(OpenGLJniWrapper.AUDIO, new AudioPlayerCallback());
        mUpstreamDispatcher.addNewPacketCallback(OpenGLJniWrapper.RECORDER,new AudioRecorderCallback(this));
        mUpstreamDispatcher.start();
        Log.d(TAG, "startMonitor:success ");
        mThreadHandler.sendEmptyMessageDelayed(WHAT_CHECK_CONNECT_STATUS, 1000);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                flashNormal.setVisibility(View.INVISIBLE);
            }
        });
    }

    private void checkConnectStatus() {
        if (!OpenGLJniWrapper.getConnectStatus()) {
            stopMonitor();
            OpenGLJniWrapper.stop();
            showReconnectDialog();
        } else {
            mThreadHandler.sendEmptyMessageDelayed(WHAT_CHECK_CONNECT_STATUS, 1000);
        }
    }

    private Range<Integer> mWidthRange;
    private Range<Integer> mHeightRange;

    private void setupMediaCodec() {
        if (mWidthRange == null || mHeightRange == null) {
            MediaCodec mediaCodec = null;
            try {
                mediaCodec = MediaCodec.createDecoderByType("video/avc");
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (mediaCodec == null) {
                Log.e(TAG, "setupMediaCodec: fail");
                return;
            }
            MediaCodecInfo codecInfo = mediaCodec.getCodecInfo();
            MediaCodecInfo.CodecCapabilities codecCapabilities = codecInfo.getCapabilitiesForType("video/avc");
            if (codecCapabilities == null) {
                Log.e(TAG, "setupMediaCodec:codecCapabilities == null ");
                return;
            }
            MediaCodecInfo.VideoCapabilities videoCapabilities = codecCapabilities.getVideoCapabilities();
            mWidthRange = videoCapabilities.getSupportedWidths();
            mHeightRange = videoCapabilities.getSupportedHeights();

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
        eventHelper = new EventHelper();
        Log.d(TAG, "setupTouch:"+ (mSurfaceView == null));
        mSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                Log.d(TAG, "onTouch:view: "+v);
                eventHelper.handleTouchEvent(v, event);
                return true;
            }
        });
    }

    public void setListener(){
        OpenGLJniCallback callback = new OpenGLJniCallback();
        callback.setObj();
        callback.setJniCallback(new OpenGLJniCallback.JniCallback() {
            @Override
            public void onResolutionUnsupported() {
                Log.d(TAG, "onResolutionUnsupported: ");
            }

            @Override
            public void onFrameChanged(int frameType, int frameSize, long timeout, long timestamp) {

            }
        });
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (eventHelper.handleKeyEvent(event)) {
            return true;
        }
        return super.dispatchKeyEvent(event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        if (eventHelper.handleMotionEvent(event)) ;
        return super.onGenericMotionEvent(event);
    }

    private void setupView() {
        mSurfaceView = findViewById(R.id.vmi_surfaceView);
        flashNormal = findViewById(R.id.flashNormal);
    }

    private void initView() {
        setupConfig();
        setupView();
        setupTouch();
        setListener();
        initDataPipe();
        setupSurfaceHolder();
        setupFloatingButton();

    }

    @Override
    public void onBackPressed() {
        showExitDialog();
    }

    private void showExitDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
        builder.setMessage("是否退出云手机？");
        builder.setTitle("warning");
        builder.setNegativeButton("确定", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                finish();

            }
        });
        builder.setPositiveButton("取消", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
            }
        });
        builder.setCancelable(false);
        builder.create().show();
    }

    private void setupConfig() {
        Intent intent = getIntent();
        if (intent != null) {
            mVideoConf = intent.getParcelableExtra("video_conf");
            DisplayMetrics metrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
            mVideoConf.setWidth(Math.min(metrics.widthPixels, metrics.heightPixels));
            mVideoConf.setHeight(Math.max(metrics.widthPixels, metrics.heightPixels));
            mVideoConf.setDensity(metrics.densityDpi);
        }
    }

    private void showReconnectDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(FullscreenActivity.this);
        builder.setMessage("网络异常断开，退出云手机？");
        builder.setTitle("warning");
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