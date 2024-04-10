/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.download;

import android.net.Uri;

/**
 * 下载进度监听
 *
 * @author zhh
 * @since 2020/5/11
 */
public interface DownloadListener {
    /**
     * 下载进度回调
     *
     * @param uri      文件uri
     * @param progress 已下载的百分比 0-1.0f
     */
    void onProgress(Uri uri, float progress);
}
