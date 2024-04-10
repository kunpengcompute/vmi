/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.huawei.cloudphonesdk.maincontrol.VideoConf;
import com.huawei.cloudphone.BuildConfig;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.CommonUtil;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphone.util.ToActivityUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.util.Locale;

/**
 * apk测试页
 *
 * @author gj
 * @since 2020-0-24
 */
public class DevTestActivity extends BaseActivity implements RadioGroup.OnCheckedChangeListener {
    private static final String TAG = DevTestActivity.class.getSimpleName();

    private EditText mServerIpEditText;
    private EditText mVmiPortEditText;
    private EditText mVssPortEditText;
    private EditText mGameIdEditText;
    private RadioGroup mDecodeRadioGroup;
    private RadioGroup mDefinitionRadioGroup;
    private Button mButton;
    private TextView mVersionTextView;

    @Override
    protected int getLayoutRes() {
        return R.layout.activity_dev_test;
    }

    @Override
    protected void initView() {
        mServerIpEditText = CastUtil.toEditText(findViewById(R.id.server_ip));
        mVmiPortEditText = CastUtil.toEditText(findViewById(R.id.server_port));
        mVssPortEditText = CastUtil.toEditText(findViewById(R.id.video_port));
        mGameIdEditText = CastUtil.toEditText(findViewById(R.id.et_game_id));
        mDecodeRadioGroup = CastUtil.toRadioGroup(findViewById(R.id.rg_decode));
        mDefinitionRadioGroup = CastUtil.toRadioGroup(findViewById(R.id.rg_definition));
        mButton = CastUtil.toButton(findViewById(R.id.btn_startGame));
        mVersionTextView = CastUtil.toTextView(findViewById(R.id.tv_version));
        // 设置版本号
        mVersionTextView.setText(String.format(Locale.ENGLISH, getString(R.string.version_name),
            BuildConfig.VERSION_NAME));
        // 初始化默认ip和端口号
        initDefaultIp();
        // last game id
        int gameId = SPUtil.getInt(SPUtil.KEY_GAME_ID, 0);
        if (gameId > 0) {
            mGameIdEditText.setText(String.valueOf(gameId));
        }
    }

    /**
     * 初始化默认ip和端口号
     */
    private void initDefaultIp() {
        Intent intent = getIntent();
        if (intent != null) {
            String ip = intent.getStringExtra("ip");
            String vmiPort = intent.getStringExtra("vmi_port");
            String vssPort = intent.getStringExtra("vss_port");
            // 传过来的IP或者端口号为空就取上一次记住的
            if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(vmiPort) || TextUtils.isEmpty(vssPort)) {
                ip = SPUtil.getString(SPUtil.KEY_SERVER_IP, null);
                vmiPort = SPUtil.getString(SPUtil.KEY_SERVER_PORT, null);
                vssPort = SPUtil.getString(SPUtil.KEY_VIDEO_PORT, null);
            }
            if (!TextUtils.isEmpty(ip) && !TextUtils.isEmpty(vmiPort) && !TextUtils.isEmpty(vssPort)) {
                mServerIpEditText.setText(ip);
                mVmiPortEditText.setText(vmiPort);
                mVssPortEditText.setText(vssPort);
                LogUtil.info(TAG, "ip: " + ip + ";vmiPort: " + vmiPort + ";vssPort: " + vssPort);
            }
        }
    }

    @Override
    protected void setListener() {
        mDecodeRadioGroup.setOnCheckedChangeListener(this);
        mDefinitionRadioGroup.setOnCheckedChangeListener(this);
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (ClickUtil.isFastClick()) {
                    return;
                }
                String ip = mServerIpEditText.getText().toString();
                String vmiPort = mVmiPortEditText.getText().toString();
                String vssPort = mVssPortEditText.getText().toString();
                if (CommonUtil.isValidIp(ip) && CommonUtil.isValidPort(vmiPort)
                    && CommonUtil.isValidPort(vssPort)) {
                    login(ip, vmiPort, vssPort);
                } else {
                    ToastUtil.showToast(R.string.invalid_ip_or_port);
                }
            }
        });
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

    private void login(final String ip, final String vmiPort, final String vssPort) {
        if (isWifi(this)) {
            startCloudPhone(ip, vmiPort, vssPort);
        } else {
            AlertDialog.Builder builder = new AlertDialog.Builder(this)
                .setTitle("提示")
                .setMessage("当前您处于非wifi状态，试玩游戏一分钟，大约需要10M流量")
                .setPositiveButton("继续游戏",
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            startCloudPhone(ip, vmiPort, vssPort);
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
     * @param ip      Ip
     * @param vmiPort vmiPort
     * @param vssPort vssPort
     */
    private void startCloudPhone(String ip, String vmiPort, String vssPort) {
        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(vmiPort) || TextUtils.isEmpty(vssPort)) {
            ToastUtil.showToast("ip or port is empty");
            return;
        }
        saveIpAndPort(ip, vmiPort, vssPort);
        // Init Config
        VideoConf videoConf = new VideoConf();
        videoConf.setIp(ip);
        videoConf.setVmiAgentPort(Integer.parseInt(vmiPort));
        videoConf.setVideoAgentPort(Integer.parseInt(vssPort));
        int gameID = CommonUtil.stringToInt(mGameIdEditText.getText().toString(), 0);
        if (gameID > 0) {
            videoConf.setTestTouch(gameID == 99);
            videoConf.setTestAudioClient(gameID == 100);
            videoConf.setGameId(gameID);
            SPUtil.putInt(SPUtil.KEY_GAME_ID, gameID);
        } else {
            SPUtil.removeKey(SPUtil.KEY_GAME_ID);
        }
        // 开启悬浮按钮（仅测试使用）
        SPUtil.putBoolean(SPUtil.IS_SHOW_FLOAT_VIEW, true);
        ToActivityUtil.toFullScreenActivity(mActivity, videoConf);
    }

    private void saveIpAndPort(String ip, String vmiPort, String vssPort) {
        // 持久化数据
        SPUtil.putString(SPUtil.KEY_SERVER_IP, ip);
        SPUtil.putString(SPUtil.KEY_SERVER_PORT, vmiPort);
        SPUtil.putString(SPUtil.KEY_VIDEO_PORT, vssPort);
    }
}
