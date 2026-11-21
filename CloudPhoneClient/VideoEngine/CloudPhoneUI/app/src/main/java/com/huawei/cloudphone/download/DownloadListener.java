/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * vmi is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
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
