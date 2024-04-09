/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.net;

import android.content.Context;

import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.R;
import com.huawei.cloudphone.model.Response;
import com.huawei.cloudphone.ui.activities.BaseActivity;
import com.huawei.cloudphone.util.SPUtil;
import com.huawei.cloudphone.util.ToActivityUtil;
import com.huawei.cloudphone.util.ToastUtil;

import java.net.ConnectException;
import java.net.SocketTimeoutException;

import io.reactivex.observers.DisposableObserver;
import retrofit2.HttpException;

/**
 * 对返回的数据进一步封装
 *
 * @author zhh
 * @since 2019-09-24
 */
public abstract class MyObserver<T extends Response> extends DisposableObserver<T> {
    /**
     * 本地错误
     */
    public static final int HTTP_LOCAL = -1;

    /**
     * 未授权
     */
    public static final int HTTP_UNAUTHORIZED = 401;

    /**
     * 资源不存在
     */
    public static final int HTTP_RESOURCE_NOT_EXIST = 901;

    private Context mContext;

    // 是否显示加载弹窗，默认显示
    private boolean mShowDialog;

    // 弹窗文案
    private final String mAlertText;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public MyObserver(Context context) {
        this(context, true);
    }

    /**
     * 构造方法
     *
     * @param context    Context
     * @param showDialog boolean
     */
    public MyObserver(Context context, boolean showDialog) {
        this(context, showDialog, null);
    }

    /**
     * MyObserver
     *
     * @param context   Context
     * @param alertText String
     */
    public MyObserver(Context context, String alertText) {
        this(context, true, alertText);
    }

    private MyObserver(Context context, boolean showDialog, String alertText) {
        mContext = context;
        mShowDialog = showDialog;
        mAlertText = alertText;
    }

    /**
     * 请求开始
     */
    @Override
    protected void onStart() {
        super.onStart();
        if (mShowDialog && (mContext instanceof BaseActivity)) {
            ((BaseActivity) mContext).showLoadingDialog(mAlertText);
        }
    }

    /**
     * 请求成功，服务端能返回数据，但code不一定是200
     *
     * @param response 返回的数据
     */
    @Override
    public void onNext(T response) {
        if (response == null) {
            onFailed(buildResponse(HTTP_LOCAL, MyApplication.instance.getString(R.string.system_error)));
            return;
        }
        // 返回非200的code认为是失败
        if (response.getErrno() != 200) {
            onFailed(response);
            return;
        }
        onSuccess(response);
    }

    /**
     * 请求出错回调
     *
     * @param throwable 错误信息
     */
    @Override
    public final void onError(Throwable throwable) {
        // 处理错误
        if (throwable instanceof SocketTimeoutException) {
            onFailed(buildResponse(HTTP_LOCAL, MyApplication.instance.getString(R.string.connect_time_out)));
        } else if (throwable instanceof ConnectException) {
            onFailed(buildResponse(HTTP_LOCAL, MyApplication.instance.getString(R.string.connect_failed)));
        } else if (throwable instanceof HttpException) {
            handleHttpException((HttpException) throwable);
        } else {
            onFailed(buildResponse(HTTP_LOCAL, MyApplication.instance.getString(R.string.system_error)));
        }
        // 回调一个完成的函数，因为出错后不会回调
        onComplete();
    }

    /**
     * 处理Http错误
     *
     * @param httpException HttpException
     */
    private void handleHttpException(HttpException httpException) {
        int responseCode = httpException.code();
        if (responseCode == HTTP_UNAUTHORIZED) {
            clearUserInfo();
            ToActivityUtil.toLoginActivity(mContext);
            ToastUtil.showToast(R.string.another_user_login);
        } else {
            onFailed(buildResponse(responseCode, MyApplication.instance.getString(R.string.system_error)));
        }
    }

    /**
     * 请求执行完回调
     */
    @Override
    public void onComplete() {
        if (mContext instanceof BaseActivity) {
            ((BaseActivity) mContext).dismissLoadingDialog();
        }
    }

    /**
     * 请求成功，服务端能返回数据，code=200
     *
     * @param t 返回的数据
     */
    protected abstract void onSuccess(T t);

    /**
     * 请求失败，失败后默认弹出toast提示
     *
     * @param response 失败信息
     */
    public void onFailed(Response response) {
        if (response != null) {
            ToastUtil.showToast(response.getErrmsg());
        }
    }

    /**
     * 清空用户信息
     */
    private void clearUserInfo() {
        // 清空内存缓存
        MyApplication.instance.setUserBean(null);
        // 清空本地缓存
        SPUtil.removeKey(SPUtil.KEY_USER_INFO);
    }

    /**
     * 构建错误包装类
     *
     * @param errCode    错误码
     * @param errMessage 错误信息
     * @return Response
     */
    private Response buildResponse(int errCode, String errMessage) {
        Response response = new Response();
        response.setErrno(errCode);
        response.setErrmsg(errMessage);
        return response;
    }
}