package com.huawei.cloudphone.broadcast;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;

import com.huawei.cloudphone.MyApplication;
import com.huawei.cloudphone.R;
import com.huawei.cloudphonesdk.utils.LogUtil;

public class ADBBroadcastReceiver extends BroadcastReceiver {
    public static final String TAG = "com.example.broadcast";

    @Override
    public void onReceive(Context context, Intent intent) {
        String broadcastName = MyApplication.instance.getString(R.string.broadcast_name);
        if (TextUtils.equals(intent.getAction(), broadcastName)) {
            LogUtil.info(broadcastName, "broadcast->");
            Bundle bundle = intent.getExtras();
            if(bundle != null) {
                Intent adbIntent = new Intent("receive_adb_data");
                adbIntent.putExtras(bundle);
                context.sendBroadcast(adbIntent);
            }
        }
    }
}
