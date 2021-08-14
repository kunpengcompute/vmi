/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui;

import android.widget.Button;
import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;
import com.huawei.instructionstream.appui.model.SettingsBean;
import com.huawei.instructionstream.appui.utils.ClickUtil;
import com.huawei.instructionstream.appui.utils.LogUtils;
import com.huawei.instructionstream.appui.utils.SPUtil;
import com.huawei.instructionstream.appui.utils.Utils;
import com.huawei.instructionstream.appui.maincontrol.GameConf;
import java.io.IOException;
import android.widget.Toast;

/**
 * 登陆UI界面.
 *
 * @since 2019-07-22
 */
public class DirectConnectActivity extends BaseActivity {
    private static final String TAG = "CGP-DirectConnActivity";
    private static final String SERVER_IP = "serverIP";
    private static final String SERVER_PORT = "serverPort";
    private static final String STARTGAME = "start";
    private static final int DETAIL_REQUEST_FULLSCREEN = 3;
    private EditText etServerPort;
    private EditText etServerIP;
    private String fileNames = "client.log";
    private int requestExternalStrongeNew = 88;
    private GameConf gameConf;
    private String serverIp;
    private String serverPort;

    private void confirmPermission() {
        boolean permission = ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED;
        LogUtils.info(TAG, "write external storage permission granted = " + permission);
        Boolean firstStart = true;
        if (Utils.getValueByKey(getApplicationContext(), Utils.SP_NAME, "firststart", true) instanceof Boolean) {
            firstStart = (Boolean) Utils.getValueByKey(getApplicationContext(), Utils.SP_NAME, "firststart", true);
        }
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                && firstStart) {
            Utils.saveKeyValue(getApplicationContext(), Utils.SP_NAME, "firststart", false);
            LogUtils.info(TAG, "start permission" + permission);
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                            Manifest.permission.READ_EXTERNAL_STORAGE}, requestExternalStrongeNew);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!isTaskRoot()) {
            Intent intent = getIntent();
            String action = intent.getAction();
            if (intent.hasCategory(Intent.CATEGORY_LAUNCHER) && Intent.ACTION_MAIN.equals(action)) {
                finish();
                return;
            }
        }
        confirmPermission();
    }

    @Override
    protected int getLayoutId() {
        return R.layout.activity_direct_connect;
    }

    @Override
    protected void loadData() {
        toolBar.setTitleText(getResources().getString(R.string.phone));
        // 获取默认设置
        getDataFromSp();
        // 获取Bundle对象，通过adb传入。
        Bundle extras = getIntent().getExtras();
        // 通过adb命令传入ip 端口。
        getServerFromAdb(extras);
        etServerIP.setText(serverIp);
        etServerPort.setText(serverPort);
        // 通过adb命令启动云手机。
        startGameByAdb(extras);
    }

    private void startGameByAdb(Bundle extras) {
        if (extras != null && extras.containsKey(STARTGAME)) {
            if (Utils.checkIp(etServerIP.getText().toString()) && Utils.checkPort(etServerPort.getText().toString())
                    && Utils.isFastClick()) {
                startGame();
            } else {
                Toast.makeText(DirectConnectActivity.this, "error ip or port", Toast.LENGTH_SHORT).show();
            }
        }
    }

    private Bundle getServerFromAdb(Bundle extras) {
        if (extras != null) {
            if (extras.containsKey(SERVER_IP)) {
                serverIp = extras.getString(SERVER_IP);
                LogUtils.info(TAG, serverIp);
            }
            if (extras.containsKey(SERVER_PORT)) {
                serverPort = extras.getString(SERVER_PORT);
                LogUtils.info(TAG, serverPort);
            }
        }
        return extras;
    }

    private void getDataFromSp() {
        if (Utils.getValueByKey(this, Utils.SP_NAME, SERVER_IP, "") instanceof String) {
            serverIp = (String) Utils.getValueByKey(this, Utils.SP_NAME, SERVER_IP, "");
        }
        if (Utils.getValueByKey(this, Utils.SP_NAME, SERVER_PORT, "") instanceof String) {
            serverPort = (String) Utils.getValueByKey(this, Utils.SP_NAME, SERVER_PORT, "");
        }
    }


    private void addListener(Button button) {
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (Utils.checkIp(etServerIP.getText().toString())
                        && Utils.checkPort(etServerPort.getText().toString())) {
                    if (ClickUtil.isFastClick()) {
                        startGame();
                    }
                } else {
                    Toast.makeText(DirectConnectActivity.this,
                            getResources().getString(R.string.error_ip_port), Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    @Override
    protected void initViews() {
        Object startBT = findViewById(R.id.btn_startGame);
        if (startBT instanceof Button) {
            addListener((Button)startBT);
        }
        if (findViewById(R.id.server_port) instanceof EditText) {
            etServerPort = (EditText) findViewById(R.id.server_port);
        }
        if (findViewById(R.id.server_ip) instanceof EditText) {
            etServerIP = (EditText)findViewById(R.id.server_ip);
        }
        if (findViewById(R.id.imageBtSetting) instanceof ImageButton) {
            ImageButton imageBtSetting = (ImageButton) findViewById(R.id.imageBtSetting);
            imageBtSetting.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(DirectConnectActivity.this, SettingsActivity.class);
                    startActivity(intent);
                }
            });
        }
    }

    private void startGame() {
        // 开始游戏
        gameConf = new GameConf();
        gameConf.setAgentAddress(setUrlAdress());
        try {
            LogUtils.startLogs(fileNames);
        } catch (IOException e) {
            LogUtils.error(TAG, e.getMessage());
        }
        saveInfo();
        SettingsBean bean = SPUtil.getObject(SPUtil.INS_SETTING,SettingsBean.class,new SettingsBean());
        Intent intent = new Intent(DirectConnectActivity.this, FullscreenActivity.class);
        intent.putExtra("agentAddress", setUrlAdress());
        intent.putExtra(SPUtil.INS_SETTING, bean.getNetType());
        DirectConnectActivity.this.startActivityForResult(intent, DETAIL_REQUEST_FULLSCREEN);
    }

    private String setUrlAdress() {
        String ip = etServerIP.getText().toString().trim();
        String port = etServerPort.getText().toString().trim();
        return ip + ":" + port;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        LogUtils.error(TAG, "reqCode:" + requestCode + " resCode:" + resultCode);

        if (requestCode == DETAIL_REQUEST_FULLSCREEN) {
            LogUtils.stopLog();
            saveInfo();
        }
    }

    private void saveInfo() {
        Utils.saveKeyValue(this, Utils.SP_NAME, "serverIP", etServerIP.getText().toString().trim());
        Utils.saveKeyValue(this, Utils.SP_NAME, "serverPort", etServerPort.getText().toString().trim());
    }
}