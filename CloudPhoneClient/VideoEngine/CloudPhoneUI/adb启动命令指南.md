1.启动app并开始连接
adb connect 0.0.0.0:8501
adb -s  0.0.0.0:8501 shell am start  -n  com.huawei.cloudphone/com.huawei.cloudphone.ui.activities.TestActivity --es ip 10.44.176.132 --es port 8002

2.使用cmd命令发送视频编码参数
adb  -s  0.0.0.0:8501  shell am broadcast  -n com.huawei.cloudphone/com.huawei.cloudphone.broadcast.ADBBroadcastReceiver -a com.example.broadcast --es cmd  video 
--ei videoBitrate 3000000 --ei videoProfile 1 --ei videoGopSize 30 --ei videoRcMode 2 --ei videoForceKeyFrame 0 --ei videoInterpolation 0

3.使用cmd命令发送音频编码参数
adb  -s  0.0.0.0:8501  shell am broadcast  -n com.huawei.cloudphone/com.huawei.cloudphone.broadcast.ADBBroadcastReceiver -a com.example.broadcast --es cmd  audio
--ei audioBitrate 192000 --ei audioSampleInterval 10

4.关闭app,释放内存
adb  -s  0.0.0.0:8501 shell am force-stop com.huawei.cloudphone


