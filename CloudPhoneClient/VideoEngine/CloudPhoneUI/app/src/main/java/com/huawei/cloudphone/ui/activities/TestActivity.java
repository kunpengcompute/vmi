/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import android.Manifest;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.RadioGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import com.huawei.cloudphonesdk.maincontrol.VideoConf;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.CommonUtil;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphone.util.ToActivityUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.io.File;

/**
 * apk测试页
 *
 * @author gj
 * @since 2019-09-24
 */
public class TestActivity extends BaseActivity implements RadioGroup.OnCheckedChangeListener {
    private static final String TAG = TestActivity.class.getSimpleName();
    private static final long LOG_FILE_SIZE_LIMIT = 20 * 1024 * 1024;
    private EditText mServerIpEditText;
    private EditText mVmiPortEditText;
    private RadioGroup mDecodeRadioGroup;
    private Button mButton;
    private String ip;
    private String port;
    private ImageButton settingButting;

    @Override
    protected int getLayoutRes() {
        return R.layout.activity_test;
    }

    @Override
    protected void initView() {
        mServerIpEditText = findViewById(R.id.server_ip);
        mVmiPortEditText = findViewById(R.id.server_port);
        mDecodeRadioGroup = findViewById(R.id.rg_decode);
        mButton = findViewById(R.id.btn_startGame);
        settingButting = findViewById(R.id.imageBtSetting);
        confirmPermission();
    }

    @Override
    protected void onStart() {
        super.onStart();
        File logFile = new File(LogUtil.PATH + LogUtil.CLIENT_LOG);
        boolean stopFlag = SPUtil.getBoolean(SPUtil.STOP_FLAG, true);
        if (!logFile.exists()) {
            return;
        }
        if (!stopFlag && logFile.length() > 0) {
            LogUtil.writeLog(LogUtil.PATH, LogUtil.CLIENT_LOG, LogUtil.EXIT_INFO);
            SPUtil.putBoolean(SPUtil.STOP_FLAG, true);
        }
        processingLargeFiles(logFile);
    }

    public void processingLargeFiles(File file) {
        if (file.length() > LOG_FILE_SIZE_LIMIT) {
            File oldLogFile = new File(LogUtil.PATH + "old_client.log");
            if (oldLogFile.exists()) {
                oldLogFile.delete();
            }
            file.renameTo(oldLogFile);
        }
    }

    private void confirmPermission() {
        boolean permission = ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
        if (!permission) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                            Manifest.permission.READ_EXTERNAL_STORAGE}, 88);
        }
    }

    /**
     * 初始化默认ip和端口号
     */
    private void initDefaultIp() {
        Intent intent = getIntent();
        if (intent == null) {
            return;
        }
        Bundle extras = intent.getExtras();
        if (extras == null) {
	        inputIpAndPort();
            return;
        }
        if (extras.containsKey("ip") && extras.containsKey("vmi_port")) {
            ip = intent.getStringExtra("ip");
            port = intent.getStringExtra("vmi_port");
            inputIpAndPort();
        } else if (extras.containsKey("ip") && extras.containsKey("port")) {
            ip = intent.getStringExtra("ip");
            port = intent.getStringExtra("port");
            inputIpAndPort();
            mButton.performClick();
        } else {
            inputIpAndPort();
        }
    }

    private void inputIpAndPort() {
        // 传过来的IP或者端口号为空就取上一次记住的
        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(port)) {
            ip = SPUtil.getString(SPUtil.KEY_SERVER_IP, null);
            port = SPUtil.getString(SPUtil.KEY_SERVER_PORT, null);
        }
        if (!TextUtils.isEmpty(ip) && !TextUtils.isEmpty(port)) {
            mServerIpEditText.setText(ip);
            mVmiPortEditText.setText(port);
            LogUtil.info(TAG, "ip: " + ip + ";vmiPort: " + port);
        }
    }

    @Override
    protected void setListener() {
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (ClickUtil.isFastClick()) {
                    return;
                }
                String ip = mServerIpEditText.getText().toString();
                String vmiPort = mVmiPortEditText.getText().toString();
                if (CommonUtil.isValidIp(ip) && CommonUtil.isValidPort(vmiPort)) {
                    login(ip, vmiPort);
                } else {
                    ToastUtil.showToast(R.string.invalid_ip_or_port);
                }
            }
        });
        settingButting.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(TestActivity.this, SettingsActivity.class);
                startActivity(intent);
            }
        });
        // 初始化默认ip和端口号
        initDefaultIp();
    }

    @Override
    public void onCheckedChanged(RadioGroup radioGroup, int id) {
        if (radioGroup == mDecodeRadioGroup && id == R.id.rb_decode_software) {
            ToastUtil.showToast("暂不支持软解");
            radioGroup.check(R.id.rb_decode_hardware);
        }
    }

    // 判断是否为wifi
    private boolean isWifi(Context context) {
        if (context == null) {
            return false;
        }
        if (context.getSystemService(CONNECTIVITY_SERVICE) instanceof ConnectivityManager) {
            ConnectivityManager connectivityManager = (ConnectivityManager)
                    context.getSystemService(CONNECTIVITY_SERVICE);
            NetworkInfo activeNetInfo = connectivityManager.getActiveNetworkInfo();
            if (activeNetInfo != null
                    && activeNetInfo.getType() == ConnectivityManager.TYPE_WIFI) {
                return true;
            }
        }
        return false;
    }

    private void login(final String ip, final String vmiPort) {
        if (isWifi(this)) {
            startCloudPhone(ip, vmiPort);
        } else {
            AlertDialog.Builder builder = new AlertDialog.Builder(this)
                    .setTitle("提示")
                    .setMessage("当前您处于非wifi状态，试玩游戏一分钟，大约需要10M流量")
                    .setPositiveButton("继续游戏",
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    startCloudPhone(ip, vmiPort);
                                }
                            })
                    .setNegativeButton("退出",
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    dialog.dismiss();
                                }
                            });
            builder.show();
        }
    }

    /**
     * 启动视频流
     *
     * @param ip      serverIp
     * @param vmiPort VimPort
     */
    private void startCloudPhone(String ip, String vmiPort) {
        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(vmiPort)) {
            ToastUtil.showToast("ip or port is empty");
            return;
        }
        LogUtil.writeLog(LogUtil.PATH, LogUtil.CLIENT_LOG, LogUtil.ENTER_INFO);
        SPUtil.putBoolean(SPUtil.STOP_FLAG, false);
        saveIpAndPort(ip, vmiPort);
        // Init Config
        VideoConf videoConf = new VideoConf();
        videoConf.setIp(ip);
        videoConf.setVmiAgentPort(Integer.parseInt(vmiPort));
        // 关闭悬浮按钮
        SPUtil.putBoolean(SPUtil.IS_SHOW_FLOAT_VIEW, true);
        ToActivityUtil.toFullScreenActivity(mActivity, videoConf);
    }

    private void saveIpAndPort(String ip, String vmiPort) {
        // 持久化数据
        SPUtil.putString(SPUtil.KEY_SERVER_IP, ip);
        SPUtil.putString(SPUtil.KEY_SERVER_PORT, vmiPort);
    }
}
