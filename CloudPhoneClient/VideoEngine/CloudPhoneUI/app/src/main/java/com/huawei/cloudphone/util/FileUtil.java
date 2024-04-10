/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.util;

import android.content.Context;
import android.net.Uri;
import android.os.Build;
import androidx.core.content.FileProvider;

import com.huawei.cloudphone.BuildConfig;

import java.io.File;

/**
 * File的工具类
 *
 * @author zhh
 * @since 2020-05-13
 */
public class FileUtil {
    /**
     * 获取文件uri
     *
     * @param context Context
     * @param file    File
     * @return Uri
     */
    public static Uri getFileUri(Context context, File file) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            return Uri.fromFile(file);
        } else {
            return FileProvider.getUriForFile(context, BuildConfig.APPLICATION_ID + ".download", file);
        }
    }
}
