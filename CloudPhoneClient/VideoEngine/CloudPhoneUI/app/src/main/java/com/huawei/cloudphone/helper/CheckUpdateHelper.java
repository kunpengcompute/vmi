/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.helper;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.view.View;

import com.huawei.cloudphone.BuildConfig;
import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.dialog.ProgressDialog;
import com.huawei.cloudphone.download.DownloadHelper;
import com.huawei.cloudphone.download.DownloadListener;
import com.huawei.cloudphone.model.CheckUpdateBean;
import com.huawei.cloudphone.model.Response;
import com.huawei.cloudphone.net.MyObserver;
import com.huawei.cloudphone.net.RequestParam;
import com.huawei.cloudphone.net.RetrofitHelper;
import com.huawei.cloudphone.ui.activities.BaseActivity;
import com.huawei.cloudphonesdk.utils.LogUtil;

import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

/**
 * 检测版本更新，下载APK，自动安装
 *
 * @author zWX809008
 * @since 2020/5/13
 */
public class CheckUpdateHelper {
    private static final String TAG = CheckUpdateHelper.class.getSimpleName();
    private BaseActivity mActivity;

    // 检测更新弹窗
    private AlertDialog mUpdateDialog;

    // 更新进度弹窗
    private ProgressDialog mProgressDialog;

    /**
     * 构造函数
     *
     * @param activity BaseActivity
     */
    public CheckUpdateHelper(BaseActivity activity) {
        mActivity = activity;
    }

    /**
     * 显示更新弹窗
     *
     * @param checkUpdateBean Update message info
     */
    private void showUpdateDialog(final CheckUpdateBean checkUpdateBean) {
        final boolean forceUpdate = checkUpdateBean.getForceUpdate() >= 1;
        AlertDialog.Builder builder = new AlertDialog.Builder(mActivity);
        builder.setTitle("升级提示");
        builder.setMessage("" + checkUpdateBean.getUpdateMsg());
        if (!forceUpdate) { // 强制更新时，不显示取消按钮
            builder.setNegativeButton("取消", null);
        }
        builder.setPositiveButton("确定", null);
        mUpdateDialog = builder.create();
        mUpdateDialog.setCanceledOnTouchOutside(false);
        mUpdateDialog.setCancelable(!forceUpdate);
        mUpdateDialog.show();
        // 强制更新时，点击确定键不消失弹窗
        mUpdateDialog.getButton(DialogInterface.BUTTON_POSITIVE)
            .setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    if (!forceUpdate) {
                        mUpdateDialog.dismiss();
                    }
                    startDownload(checkUpdateBean);
                }
            });
    }

    /**
     * 开始下载
     *
     * @param checkUpdateBean 新版本apk信息
     */
    private void startDownload(CheckUpdateBean checkUpdateBean) {
        // 显示下载进度弹窗
        showProgressDialog();
        // 保存的APK文件名称格式
        String fileName = "CloudPhone_" + checkUpdateBean.getVersionNo() + "_v"
            + checkUpdateBean.getVersionName() + ".apk";
        // 添加下载任务执行下载
        DownloadHelper.getInstance().addDownloadTask(checkUpdateBean.getUrl(), fileName, new DownloadListener() {
            @Override
            public void onProgress(Uri uri, float progress) {
                mProgressDialog.setProgress(progress);
                if (progress == 1) {
                    mProgressDialog.setProgress(0);
                    mProgressDialog.dismiss();
                    installApk(uri);
                }
            }
        });
    }

    /**
     * 显示下载进度弹窗
     */
    private void showProgressDialog() {
        if (mProgressDialog == null) {
            mProgressDialog = new ProgressDialog(mActivity);
        }
        mProgressDialog.show();
    }

    /**
     * 安装APK
     *
     * @param uri 文件uri
     */
    private void installApk(Uri uri) {
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        intent.setDataAndType(uri, "application/vnd.android.package-archive");
        mActivity.startActivity(intent);
    }
}
