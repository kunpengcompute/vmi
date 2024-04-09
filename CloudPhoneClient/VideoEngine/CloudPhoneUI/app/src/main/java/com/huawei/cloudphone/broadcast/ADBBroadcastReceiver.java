package com.huawei.cloudphone.broadcast;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

public class ADBBroadcastReceiver extends BroadcastReceiver {
    public static final String TAG = "com.example.broadcast";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent.getAction().equals(TAG)) {
            Bundle bundle = intent.getExtras();
            Intent adbIntent = new Intent("receive_abb_data");
            adbIntent.putExtras(bundle);
            context.sendBroadcast(adbIntent);
        }
    }
}
