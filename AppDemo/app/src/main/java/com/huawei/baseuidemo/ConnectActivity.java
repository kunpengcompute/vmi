package com.huawei.baseuidemo;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.regex.Pattern;

public class ConnectActivity extends AppCompatActivity {

    private static final String TAG = "VmiConnectPage";
    private EditText serverIp; // IP
    private EditText serverPort; // 端口号
    public static final int MIN_PORT = 1; // 最小端口号
    public static final int MAX_PORT = 65535; // 最大端口号

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_connect);
        if (!isTaskRoot()) {
            Intent intent = getIntent();
            String action = intent.getAction();
            if (intent.hasCategory(Intent.CATEGORY_LAUNCHER) && Intent.ACTION_MAIN.equals(action)) {
                finish();
                return;
            }
        }

        Object startButton = findViewById(R.id.btn_startGame);
        if (startButton instanceof Button) {
            addListener((Button)startButton);
        }

        if (findViewById(R.id.server_ip) instanceof EditText) {
            serverIp = (EditText)findViewById(R.id.server_ip);
        }

        if (findViewById(R.id.server_port) instanceof EditText) {
            serverPort = (EditText)findViewById(R.id.server_port);
        }
    }

    private void addListener(Button button) {
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (checkIp(serverIp.getText().toString()) &&
                    checkPort(serverPort.getText().toString())) {
                    Intent intent = new Intent(ConnectActivity.this, BaseUiActivity.class);
                    intent.putExtra("agentAddress", setUrlAddress());
                    startActivity(intent);
                } else {
                    Toast.makeText(ConnectActivity.this,
                            "enter IP address or port is invalid.", Toast.LENGTH_LONG).show();
                }
            }
        });
    }

    /**
     * 校验IP是否合法.
     *
     * @param text, ipaddr
     * @return ip是否合法
     */
    private boolean checkIp(String text) {
        if (text != null && !text.isEmpty()) {
            String regex = "^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$";
            if (text.matches(regex)) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    /**
     * 校验port是否合法.
     *
     * @param port
     * @return port是否合法
     */
    private boolean checkPort(String port) {
        Pattern pattern = Pattern.compile("[1-9][0-9]{0,5}");
        if (pattern.matcher(port).matches()) {
            int portNum = Integer.parseInt(port);
            Log.i(TAG, "port num: " + portNum);
            if (portNum >= MIN_PORT && portNum <= MAX_PORT) {
                return true;
            }
        }
        return false;
    }

    /**
     * 合并ip和port字符串
     *
     * @return 将ip和port合并返回
     */
    private String setUrlAddress() {
        String ip = serverIp.getText().toString().trim();
        String port = serverPort.getText().toString().trim();
        String urlAddr = ip + ":" + port;
        return urlAddr;
    }
}