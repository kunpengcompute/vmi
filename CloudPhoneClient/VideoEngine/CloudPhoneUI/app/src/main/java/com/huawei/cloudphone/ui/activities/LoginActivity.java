/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import android.content.DialogInterface;
import android.content.Intent;
import androidx.appcompat.app.AlertDialog;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import com.huawei.cloudphone.BuildConfig;
import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.model.UserBean;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.CommonUtil;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphone.util.ToActivityUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphone.util.ViewUtil;
import com.huawei.cloudphone.widget.TopBar;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.util.Locale;

/**
 * apk登录页
 *
 * @author gj
 * @since 2019-09-24
 */
public class LoginActivity extends BaseActivity implements View.OnClickListener {
    private static final String TAG = LoginActivity.class.getSimpleName();

    // TopBar
    private TopBar mTopBar;

    // 账号
    private EditText mAccountEditText;

    // 密码
    private EditText mPasswordEditText;

    // 清空账号密码
    private ImageView mClearInputImageView;

    // 新用户注册
    private TextView mRegisterTextView;

    // 登录
    private Button mLoginButton;

    // test
    private Button mTestButton;

    // 版本号
    private TextView mVersionTextView;

    // Ip
    private String[] mIpArray;

    // 配服IP
    private String mServerIp;

    @Override
    public int getLayoutRes() {
        if (!isTaskRoot()) {
            finish();
        }
        return R.layout.activity_login;
    }

    @Override
    public void initView() {
        mTopBar = CastUtil.toTopBar(findViewById(R.id.tb_header));
        mAccountEditText = CastUtil.toEditText(findViewById(R.id.et_account));
        mPasswordEditText = CastUtil.toEditText(findViewById(R.id.et_password));
        mClearInputImageView = CastUtil.toImageView(findViewById(R.id.iv_clear));
        mRegisterTextView = CastUtil.toTextView(findViewById(R.id.tv_register));
        mLoginButton = CastUtil.toButton(findViewById(R.id.btn_login));
        mTestButton = CastUtil.toButton(findViewById(R.id.btn_test));
        mVersionTextView = CastUtil.toTextView(findViewById(R.id.tv_version));
        mIpArray = getResources().getStringArray(R.array.ip);
        // 设置测试按钮是否可见，debug显示，release隐藏
        ViewUtil.setVisibility(mTestButton, BuildConfig.DEBUG ? View.VISIBLE : View.GONE);
        // 设置版本号
        mVersionTextView.setText(String.format(Locale.ENGLISH, getString(R.string.version_name),
            BuildConfig.VERSION_NAME));
        // 初始化默认配服IP
        initDefaultIp();
        // 设置记住的账号密码
        mAccountEditText.setText(SPUtil.getString(SPUtil.KEY_ACCOUNT, ""));
        mPasswordEditText.setText(SPUtil.getString(SPUtil.KEY_PASSWORD, ""));
        // 提示是否允许截屏
        checkScreenShot();
    }

    /**
     * 检测是否允许截屏
     */
    private void checkScreenShot() {
        if ((SPUtil.getBoolean(SPUtil.SCREEN_LIMIT, true)) && !BuildConfig.DEBUG) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("提示");
            builder.setMessage("是否允许录（截）屏");
            builder.setIcon(R.mipmap.ic_launcher);
            builder.setCancelable(false);
            builder.setPositiveButton("允许", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    dialogInterface.dismiss();
                    SPUtil.putBoolean(SPUtil.SCREEN_LIMIT, false);
                    SPUtil.putBoolean(SPUtil.IS_SCREENSHOT, true);
                }
            });
            builder.setNegativeButton("不允许", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    dialogInterface.dismiss();
                    SPUtil.putBoolean(SPUtil.SCREEN_LIMIT, false);
                    SPUtil.putBoolean(SPUtil.IS_SCREENSHOT, false);
                    ToActivityUtil.toLoginActivity(mActivity);
                }
            });
            builder.create().show();
        }
    }

    /**
     * 初始化默认配服IP
     */
    private void initDefaultIp() {
        Intent intent = getIntent();
        // 读取启动时传过来的配置服IP
        if (intent != null) {
            mServerIp = intent.getStringExtra("server_ip");
        }
        // 启动时未传IP则取上次记住的IP，若上次未记住IP则取列表第一个IP
        if (TextUtils.isEmpty(mServerIp)) {
            mServerIp = SPUtil.getString(SPUtil.KEY_BASE_URL, mIpArray[0]);
        }
        MyApplication.instance.setBaseUrl(mServerIp);
    }

    @Override
    public void setListener() {
        // 点击事件
        mClearInputImageView.setOnClickListener(this);
        mLoginButton.setOnClickListener(this);
        mRegisterTextView.setOnClickListener(this);
        mTestButton.setOnClickListener(this);
        mTopBar.setOnTopBarClickListener(new TopBar.OnTopBarClickListener() {
            @Override
            public void onBackClicked(View view) {
            }

            @Override
            public void onMenuClicked(View view) {
                showChooseIpDialog();
            }
        });
        mLoginButton.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View view) {
                if (ClickUtil.isFastClick()) {
                    return true;
                }
                showInputIpDialog();
                return true;
            }
        });
    }

    @Override
    public void onClick(View view) {
        if (ClickUtil.isFastClick()) {
            return;
        }
        switch (view.getId()) {
            case R.id.iv_clear: { // 清空账号密码
                mAccountEditText.setText("");
                mPasswordEditText.setText("");
                break;
            }
            case R.id.btn_login: { // 登录
                break;
            }
            case R.id.tv_register: { // 新用户注册
                ToActivityUtil.toRegisterActivity(mActivity);
                break;
            }
            case R.id.btn_test: {
                ToActivityUtil.toTestActivity(mActivity);
                break;
            }
            default: {
                break;
            }
        }
    }

    /**
     * 弹出自定义IP输入框
     */
    private void showInputIpDialog() {
        View contentView = LayoutInflater.from(mActivity)
            .inflate(R.layout.layout_input_ip, new FrameLayout(mActivity));
        final EditText editText = CastUtil.toEditText(contentView.findViewById(R.id.et_dialog_ip));
        editText.setText(mServerIp);
        editText.setSelection(mServerIp.length());
        // Init dialog
        AlertDialog alertDialog = new AlertDialog.Builder(mActivity)
            .setTitle(R.string.input_ip)
            .setView(contentView)
            .setNegativeButton(R.string.cancel, null)
            .setPositiveButton(R.string.ensure, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    String ip = editText.getText().toString();
                    if (CommonUtil.isValidIp(ip)) {
                        mServerIp = ip;
                        MyApplication.instance.setBaseUrl(ip);
                    } else {
                        LogUtil.error(TAG, "input ip: " + getString(R.string.invalid_ip));
                        ToastUtil.showToast(R.string.invalid_ip);
                    }
                }
            })
            .create();
        alertDialog.show();
    }

    /**
     * 校验账号密码是否合法
     *
     * @param account  账号
     * @param password 密码
     * @return true:合法 false:非法
     */
    private boolean checkParamsValid(String account, String password) {
        if (TextUtils.isEmpty(account) || TextUtils.isEmpty(password)) {
            ToastUtil.showToast("账号或密码不能为空");
            return false;
        }
        return true;
    }

    /**
     * 保存个人信息
     *
     * @param userBean 用户信息
     * @param account  账号
     * @param password 密码
     */
    private void saveUserInfo(UserBean userBean, String account, String password) {
        // 缓存一份用户信息到内存
        MyApplication.instance.setUserBean(userBean);
        // 保存用户信息和账号密码到本地
        SPUtil.putObject(SPUtil.KEY_USER_INFO, userBean);
        SPUtil.putString(SPUtil.KEY_ACCOUNT, account);
        SPUtil.putString(SPUtil.KEY_PASSWORD, password);
    }

    /**
     * 选择ip弹窗
     */
    private void showChooseIpDialog() {
        int selectedIndex = -1;
        for (int index = 0; index < mIpArray.length; index++) {
            if (mIpArray[index].equals(mServerIp)) {
                selectedIndex = index;
                break;
            }
        }
        AlertDialog alertDialog = new AlertDialog.Builder(mActivity)
            .setSingleChoiceItems(mIpArray, selectedIndex, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    mServerIp = mIpArray[which];
                    MyApplication.instance.setBaseUrl(mServerIp);
                    dialog.dismiss();
                }
            }).create();
        alertDialog.show();
    }
}
