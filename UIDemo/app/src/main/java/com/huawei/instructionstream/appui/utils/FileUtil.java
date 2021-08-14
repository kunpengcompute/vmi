/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.utils;

import android.os.Environment;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;

/**
 * 文件读写操作
 *
 * @since 2021-03-04
 */
public class FileUtil {
    private static final String PATH = Environment.getExternalStorageDirectory() + "/vmifps/";
    private static final String TAG = "CGP-fileUtil";
    private static ThreadPool threadPool = new ThreadPool(1, 20);

    /**
     * 将字符串写入文件
     *
     * @param filename 文件名
     * @param text 需要写入的字符串
     */
    public static void stringToFile(final String filename, final String text) {
        threadPool.submit(new Runnable() {
            @Override
            public void run() {
                File filepath = new File(PATH);
                if (!filepath.exists()) {
                    filepath.mkdirs();
                }
                File file = new File(PATH + filename);
                FileOutputStream os = null;
                PrintStream ps = null;
                try {
                    os = new FileOutputStream(file, true);
                    ps = new PrintStream(os);
                    ps.append(text + System.getProperty("line.separator"));
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } finally {
                    if (os != null) {
                        try {
                            os.close();
                        } catch (IOException e) {
                            LogUtils.error(TAG, e.getMessage());
                        }
                    }
                    if (ps != null) {
                        ps.close();
                    }
                }
            }
        });
    }

    /**
     * 删除文件
     *
     * @param filename 文件名
     */
    public static void deleteFile(String filename) {
        File file = new File(PATH + filename);
        if (file.exists() && file.isFile()) {
            file.delete();
        }
    }

}
