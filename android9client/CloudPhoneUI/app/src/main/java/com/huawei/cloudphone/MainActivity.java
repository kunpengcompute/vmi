package com.huawei.cloudphone;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.cloudphoneui.R;

public class MainActivity extends AppCompatActivity {

    private EditText serverIpEditText;
    private EditText serverPortEditText;
    private EditText serverAudioPortEditText;
    private Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        serverIpEditText = findViewById(R.id.server_ip);
        serverPortEditText = findViewById(R.id.server_port);
        serverAudioPortEditText =findViewById(R.id.server_port_audio);
        String ip = getString("IP","");
        String port = getString("PORT","");
        String audioPort = getString("PORT_AUDIO","");
        serverIpEditText.setText(ip);
        serverPortEditText.setText(port);
        serverAudioPortEditText.setText(audioPort);
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
                String audioPort = serverAudioPortEditText.getText().toString();
                if (Utils.isValidIp(ip) && Utils.isValidPort(port)&&Utils.isValidPort(audioPort)) {
                    login(ip, port,audioPort);
                } else {
                    Toast.makeText(MainActivity.this, "输入的ip或port端口不合法", Toast.LENGTH_LONG).show();
                }
            }
        });
    }

    private void login(String ip, String port, String audioPort) {
        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(port) ||  TextUtils.isEmpty(audioPort)) {
            Toast.makeText(MainActivity.this, "ip或port端口为空", Toast.LENGTH_LONG).show();
            return;
        }
        Log.d("wsh0", "login:ip: "+ip+",port:"+port+",vsport:"+audioPort);
        saveIpAndPort(ip, port,audioPort);
        VideoConf videoConf = new VideoConf();
        videoConf.setIp(ip);
        videoConf.setPort(Integer.parseInt(port));
        videoConf.setAudioPort(Integer.parseInt(audioPort));
        Log.d("wsh1", "login:ip: "+ip+",port:"+port+",vsport:"+audioPort);
        Bundle bundle = new Bundle();
        bundle.putParcelable("video_conf", videoConf);
        Intent intent = new Intent(this, FullscreenActivity.class);
        intent.putExtras(bundle);
        startActivity(intent);
    }

    private void saveIpAndPort(String ip, String port, String audioPort) {
        putString("IP", ip);
        putString("PORT", port);
        putString("PORT_AUDIO", audioPort);
    }

    private void putString(String key, String value) {
        SharedPreferences preferences = getSharedPreferences("sp", Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(key, value);
        editor.apply();
    }

    private String getString(String key, String value) {
        SharedPreferences preferences = getSharedPreferences("sp", Context.MODE_PRIVATE);
        return preferences.getString(key, value);
    }

    private void confirmPermission() {
        boolean permission = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
        if (!permission) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE}, 88);
        }
    }

}