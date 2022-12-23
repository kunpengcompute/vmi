package com.huawei.utils;

import android.os.Environment;
import android.util.Log;

import com.huawei.cloudphone.ThreadPool;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;

public class LogUtil {
    public static final String PATH = Environment.getExternalStorageDirectory() + "/log/";
    public static final String CLIENT_LOG = "client.log";
    public static final String EXIT_INFO = "===========exit cloud phone============";
    public static final String ENTER_INFO = "============enter cloud phone==========";
    public static final String TAG = "LogUtil";
    private static final String[] running = new String[]{"logcat"};
    private static final String[] clear = new String[]{"logcat", "-c"};
    private static Process exec;
    private static final ThreadPool threadPool = new ThreadPool(1, 20);

    public static void startLogs(final String fileName) {
        stopLog();
        try {
            Runtime.getRuntime().exec(clear);
            createFile(fileName);
            exec = Runtime.getRuntime().exec(running);
        } catch (IOException e) {
            Log.e(TAG, "startLogs: " + e.getMessage());
        }
        threadPool.submit(new Runnable() {
            @Override
            public void run() {
                final InputStream is = exec.getInputStream();
                FileOutputStream os = null;
                try {
                    os = new FileOutputStream(PATH + fileName, true);
                    int len = 0;
                    byte[] buf = new byte[1024];
                    len = is.read(buf);
                    while (len != -1) {
                        os.write(buf, 0, len);
                        os.flush();
                        len = is.read(buf);
                    }
                } catch (FileNotFoundException e) {
                    Log.e(TAG, "FileNotFoundException: " + e.getMessage());
                } catch (IOException e) {
                    Log.e(TAG, "IOException: " + e.getMessage());
                } finally {
                    if (os != null) {
                        try {
                            os.close();
                        } catch (IOException e) {
                            Log.e(TAG, "final os IOException: " + e.getMessage());
                        }
                    }

                    if (is != null) {
                        try {
                            is.close();
                        } catch (IOException e) {
                            Log.e(TAG, "final is IOException: " + e.getMessage());
                        }
                    }
                }
            }
        });
    }

    private static void createFile(String fileName) {
        File logDir = new File(PATH);
        if (!logDir.exists()) {
            boolean mkdirs = logDir.mkdirs();
        }
        File logFile = new File(PATH + fileName);
        if (!logFile.exists()) {
            try {
                logFile.createNewFile();
            } catch (IOException e) {
                Log.e(TAG, "createFile: " + e.getMessage());
            }
        }
    }

    private static void stopLog() {
        if (exec != null) {
            exec.destroy();
        }
    }

    public static void writeLog(String fileDirName, String fileName, String info) {
        BufferedWriter bw = null;
        info = info + System.getProperty("line.separator");
        try {
            bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileDirName + fileName, true)));
            bw.write(info);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (bw != null) {
                try {
                    bw.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
