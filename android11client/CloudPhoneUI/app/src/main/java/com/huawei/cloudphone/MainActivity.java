package com.huawei.cloudphone;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.cloudphoneui.R;
import com.huawei.utils.LogUtil;
import com.huawei.utils.Utils;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    public static final int LOG_FILE_SIZE_LIMIT = 20 * 1024 * 1024;
    private EditText serverIpEditText;
    private EditText serverPortEditText;
    private Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        serverIpEditText = findViewById(R.id.server_ip);
        serverPortEditText = findViewById(R.id.server_port);
        initDefaultIP();
        button = findViewById(R.id.btn_startGame);
        confirmPermission();
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (Utils.isFastClick()) {
                    return;
                }
                String ip = serverIpEditText.getText().toString();
                String port = serverPortEditText.getText().toString();
                if (Utils.isValidIp(ip) && Utils.isValidPort(port)) {
                    login(ip, port);
                } else {
                    Toast.makeText(MainActivity.this, "输入的ip或port端口不合法", Toast.LENGTH_LONG).show();
                }
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
        File logFile = new File(LogUtil.PATH + LogUtil.CLIENT_LOG);
        boolean stopFlag = Utils.getBoolean(Utils.STOP_FLAG, true);
        if (!logFile.exists()) {
            return;
        }
        if (!stopFlag && logFile.length() > 0) {
            LogUtil.writeLog(LogUtil.PATH, LogUtil.CLIENT_LOG, LogUtil.EXIT_INFO);
            Utils.putBoolean(Utils.STOP_FLAG, true);
        }
        processingLargeFiles(logFile);
    }

    private void processingLargeFiles(File logFile) {
        if (logFile.length() > LOG_FILE_SIZE_LIMIT) {
            File oldLogFile = new File(LogUtil.PATH + "old_client.log");
            if (oldLogFile.exists()) {
                oldLogFile.delete();
            }
            logFile.renameTo(oldLogFile);
        }
    }

    private void initDefaultIP() {
        Intent intent = getIntent();
        if (intent != null) {
            String ip = intent.getStringExtra("serverIP");
            String port = intent.getStringExtra("serverPort");
            if (!TextUtils.isEmpty(ip) && !TextUtils.isEmpty(port)) {
                login(ip, port);
            } else {
                ip = Utils.getString("IP", "");
                port = Utils.getString("PORT", "");
            }
            serverIpEditText.setText(ip);
            serverPortEditText.setText(port);
        }
    }

    private void login(String ip, String port) {
        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(port)) {
            Toast.makeText(MainActivity.this, "ip或port端口为空", Toast.LENGTH_LONG).show();
            return;
        }
        LogUtil.writeLog(LogUtil.PATH, LogUtil.CLIENT_LOG, LogUtil.ENTER_INFO);
        Utils.putBoolean(Utils.STOP_FLAG, false);
        saveIpAndPort(ip, port);
        VideoConf videoConf = new VideoConf();
        videoConf.setIp(ip);
        videoConf.setPort(Integer.parseInt(port));
        Bundle bundle = new Bundle();
        bundle.putParcelable("video_conf", videoConf);
        Intent intent = new Intent(this, FullscreenActivity.class);
        intent.putExtras(bundle);
        startActivity(intent);
    }

    private void saveIpAndPort(String ip, String port) {
        Utils.putString("IP", ip);
        Utils.putString("PORT", port);
    }

    private void confirmPermission() {
        boolean permission = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
        if (!permission) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE}, 88);
        }
    }

}