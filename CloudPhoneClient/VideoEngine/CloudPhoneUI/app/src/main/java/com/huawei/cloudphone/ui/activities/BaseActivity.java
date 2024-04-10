/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.ui.activities;

import android.annotation.SuppressLint;
import android.os.Bundle;
import androidx.annotation.LayoutRes;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.huawei.cloudphone.dialog.LoadingDialog;
import com.huawei.cloudphone.util.SPUtil;

import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.disposables.Disposable;

/**
 * 基础activity
 *
 * @author gj
 * @since 2019-09-24
 */
@SuppressLint("Registered")
public abstract class BaseActivity extends AppCompatActivity {
    private static final String TAG = BaseActivity.class.getSimpleName();

    BaseActivity mActivity;
    private boolean mIsTv;
    private float mRatio;
    private DisplayMetrics mMetrics = new DisplayMetrics();

    // 保存当前页面的请求，便于退出页面时取消请求
    private CompositeDisposable mCompositeDisposable;

    // 加载框
    private LoadingDialog mLoadingDialog;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!(SPUtil.getBoolean(SPUtil.IS_SCREENSHOT, true))) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_SECURE);
        }
        mActivity = this;
        setContentView(getLayoutRes());
        // 初始化View
        initView();
        // 绑定监听事件
        setListener();
    }

    /**
     * LayoutRes
     *
     * @return int
     */
    @LayoutRes
    protected abstract int getLayoutRes();

    /**
     * 初始化view
     */
    protected abstract void initView();

    /**
     * 设置监听
     */
    protected abstract void setListener();

    @Override
    protected void onDestroy() {
        // 取消所有请求
        removeAllDisposable();
        dismissLoadingDialog();
        super.onDestroy();
    }

    /**
     * 添加请求
     *
     * @param disposable Disposable
     */
    public synchronized void addDisposable(Disposable disposable) {
        if (mCompositeDisposable == null) {
            mCompositeDisposable = new CompositeDisposable();
        }
        mCompositeDisposable.add(disposable);
    }

    /**
     * 移除所有请求
     */
    private void removeAllDisposable() {
        if (mCompositeDisposable != null) {
            mCompositeDisposable.clear();
            mCompositeDisposable = null;
        }
    }

    /**
     * 显示加载框
     *
     * @param message 加载框message
     */
    public void showLoadingDialog(String message) {
        if (mLoadingDialog == null) {
            mLoadingDialog = new LoadingDialog(this);
        }
        mLoadingDialog.setMessage(message);
        if (!mLoadingDialog.isShowing()) {
            mLoadingDialog.show();
        }
    }

    /**
     * 隐藏加载框
     */
    public void dismissLoadingDialog() {
        if (mLoadingDialog == null) {
            return;
        }
        if (mLoadingDialog.isShowing()) {
            mLoadingDialog.dismiss();
        }
    }
}