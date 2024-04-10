/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;

import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.model.Response;
import com.huawei.cloudphone.net.MyObserver;
import com.huawei.cloudphone.net.RequestParam;
import com.huawei.cloudphone.net.RetrofitHelper;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;
import com.huawei.cloudphone.util.CommonUtil;
import com.huawei.cloudphone.util.ToastUtil;
import com.huawei.cloudphone.widget.TopBar;
import com.huawei.cloudphonesdk.utils.LogUtil;

import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

/**
 * apk登录页
 *
 * @author gj
 * @since 2019-09-24
 */
public class RegisterActivity extends BaseActivity implements View.OnClickListener {
    private static final String TAG = RegisterActivity.class.getSimpleName();
    // 导航栏
    private TopBar mTopBar;

    // 账号
    private EditText mAccountEditText;

    // 密码
    private EditText mPasswordEditText;

    // 清空账号密码
    private ImageView mClearInputImageView;

    // 注册
    private Button mRegisterButton;

    @Override
    protected int getLayoutRes() {
        return R.layout.activity_register;
    }

    @Override
    public void initView() {
        mTopBar = CastUtil.toTopBar(findViewById(R.id.tb_header));
        mAccountEditText = CastUtil.toEditText(findViewById(R.id.et_account));
        mPasswordEditText = CastUtil.toEditText(findViewById(R.id.et_password));
        mClearInputImageView = CastUtil.toImageView(findViewById(R.id.iv_clear));
        mRegisterButton = CastUtil.toButton(findViewById(R.id.btn_register));
    }

    @Override
    public void setListener() {
        // 点击事件
        mClearInputImageView.setOnClickListener(this);
        mRegisterButton.setOnClickListener(this);
        // 导航栏返回
        mTopBar.setOnTopBarClickListener(new TopBar.OnTopBarClickListener() {
            @Override
            public void onBackClicked(View view) {
                finish();
            }

            @Override
            public void onMenuClicked(View view) {
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
            case R.id.btn_register: { // 注册
                break;
            }
            default: {
                break;
            }
        }
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
        if (!CommonUtil.isRegisterValidAccount(account)) {
            ToastUtil.showToast("用户名必须以字母开头,只能包含数字和英文字母,用户名长度在3到20位");
            return false;
        }
        if (!CommonUtil.isRegisterValidPassword(password)) {
            ToastUtil.showToast("密码必须同时包含数字和英文字母, 特殊字符只可选@.!_，长度在8到20之间，且不可包含两个以上连续字符。");
            return false;
        }
        return true;
    }
}
