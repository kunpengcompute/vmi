package com.huawei.cloudphone.ui.activities;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphone.util.ToActivityUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphonesdk.maincontrol.VideoConf;
import com.huawei.cloudphonesdk.utils.LogUtil;

public class ReconnectActivity extends BaseActivity {

    public static final String TAG = "FullscreenActivity";
    BaseActivity mActivity;

    @Override
    protected int getLayoutRes() {
        return R.layout.activity_reconnect;
    }

    @Override
    protected void initView() {
        mActivity = this;
        setFullScreen();
        showReconnectDialog();
    }

    @Override
    protected void setListener() {
    }

    private String getStringFromRes(int resId) {
        return getResources().getString(resId);
    }

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
    }

    private void showReconnectDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(ReconnectActivity.this);
        builder.setMessage(getStringFromRes(R.string.net_error));
        builder.setTitle("Warning");
        builder.setNegativeButton(getStringFromRes(R.string.reconnect_phone), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                LogUtil.info("reconnectCloudPhone", "reconnect dialog, user select reconnect Phone");
                dialog.dismiss();
                startConnect();
            }
        });
        builder.setPositiveButton(getStringFromRes(R.string.exit), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                LogUtil.info("reconnectCloudPhone", "close phone due to user select exit phone in reconnect dialog");
                finish();
            }
        });
        builder.setCancelable(false);
        AlertDialog reconnectDialog = builder.create();
        reconnectDialog.show();
        Log.d(TAG, "showReconnectDialog: ");
    }

    private void startConnect() {
        String ip = SPUtil.getString(SPUtil.KEY_SERVER_IP, "");
        String vmiPort = SPUtil.getString(SPUtil.KEY_SERVER_PORT, "");
        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(vmiPort)) {
            ToastUtil.showToast("ip or port is empty, reconnect cloudPhone fail");
            return;
        }
        // Init Config
        VideoConf videoConf = new VideoConf();
        videoConf.setIp(ip);
        videoConf.setVmiAgentPort(Integer.parseInt(vmiPort));
        // 关闭悬浮按钮
        SPUtil.putBoolean(SPUtil.IS_SHOW_FLOAT_VIEW, true);
        ToActivityUtil.toFullScreenActivity(mActivity, videoConf);
        finish();
    }
}