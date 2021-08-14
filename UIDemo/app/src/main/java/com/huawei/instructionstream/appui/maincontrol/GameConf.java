/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol;

import android.util.Log;
import com.huawei.instructionstream.appui.R;

import java.io.Serializable;

/**
 * GameConf info.
 *
 * @since 2018-07-05
 */
public class GameConf implements Serializable {
    private static final long serialVersionUID = 224L;
    boolean testVmiTouchJava = false;
    boolean testAudioClient = false;

    boolean reconnectLastVm = false;
    boolean frameBufferDepth24 = false;
    boolean showStat = true;
    boolean saveTexture = true;
    String agentAddress = "192.168.2.1:88888";
    String redisIp = "192.168.2.94";
    int redisPort = 10000;
    int gameId = 0;
    int remoteScreenResolution = 720; // 默认720p，保留可修改能力
    int remoteTouchResolution = 720; // 默认720p，保留可修改能力
    boolean screenLandscape = false; // 是否横屏
    int loggingLevel = Log.INFO;
    Boolean startGame = true; // 是否发送启动游戏命令，正常为true，如果有其他启动游戏方式设为false
    int clarityLevel = 0;
    String name = "";

    Boolean fixedLandscape = false;
    Boolean floaterVisible = true;
    int numberOfFramesSkipped = 0;
    int splashScreenImageResourceId = R.drawable.vmi_splash_screen_landscape;
    int videoWaitingImageResourceId;

    String configServerToken = "123456"; // 默认是一个无效的token
    String callbackClassName = "com.huawei.cloudgame.decoder.FullscreenActivity";
    Boolean esGL = false;

    public void setTestVmiTouchJava(boolean testVmiTouchJava) {
        this.testVmiTouchJava = testVmiTouchJava;
    }

    public boolean getTestVmiTouchJava() {
        return this.testVmiTouchJava;
    }

    public void setTestAudioClient(boolean testAudioClient) {
        this.testAudioClient = testAudioClient;
    }

    public boolean getTestAudioClient() {
        return this.testAudioClient;
    }

    public boolean getReconnectLastVm() {
        return reconnectLastVm;
    }

    public void setReconnectLastVm(boolean reconnectLastVm) {
        this.reconnectLastVm = reconnectLastVm;
    }

    public String getCallbackClassName() {
        return callbackClassName;
    }

    public void setCallbackClassName(String callbackClassName) {
        this.callbackClassName = callbackClassName;
    }

    public String getConfigServerToken() {
        return configServerToken;
    }

    public void setConfigServerToken(String configServerToken) {
        this.configServerToken = configServerToken;
    }

    public Boolean getFloaterVisible() {
        return floaterVisible;
    }

    public void setFloaterVisible(Boolean floaterVisible) {
        this.floaterVisible = floaterVisible;
    }

    public int getNumberOfFramesSkipped() {
        return numberOfFramesSkipped;
    }

    public void setNumberOfFramesSkipped(int numberOfFramesSkipped) {
        this.numberOfFramesSkipped = numberOfFramesSkipped;
    }

    public Boolean getFixedLandscape() {
        return fixedLandscape;
    }

    public void setFixedLandscape(Boolean fixedLandscape) {
        this.fixedLandscape = fixedLandscape;
    }

    public int getSplashScreenImageResourceId() {
        return splashScreenImageResourceId;
    }

    public void setSplashScreenImageResourceId(int splashScreenImageResourceId) {
        this.splashScreenImageResourceId = splashScreenImageResourceId;
    }

    public int getVideoWaitingImageResourceId() {
        return videoWaitingImageResourceId;
    }

    public void setVideoWaitingImageResourceId(int videoWaitingImageResourceId) {
        this.videoWaitingImageResourceId = videoWaitingImageResourceId;
    }

    public Boolean getEsGL() {
        return esGL;
    }

    public void setEsGL(Boolean esGL) {
        this.esGL = esGL;
    }

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getClarityLevel() {
        return clarityLevel;
    }

    public void setClarityLevel(int clarityLevel) {
        this.clarityLevel = clarityLevel;
    }

    public boolean isFrameBufferDepth24() {
        return frameBufferDepth24;
    }

    public void setFrameBufferDepth24(boolean frameBufferDepth24) {
        this.frameBufferDepth24 = frameBufferDepth24;
    }

    public boolean isShowStat() {
        return showStat;
    }

    public void setShowStat(boolean showStat) {
        this.showStat = showStat;
    }

    public String getAgentAddress() {
        return agentAddress;
    }

    public void setAgentAddress(String agentAddress) {
        this.agentAddress = agentAddress;
    }

    public int getGameId() {
        return gameId;
    }

    public void setGameId(int gameId) {
        this.gameId = gameId;
    }

    public int getRemoteScreenResolution() {
        return remoteScreenResolution;
    }

    public void setRemoteScreenResolution(int remoteScreenResolution) {
        this.remoteScreenResolution = remoteScreenResolution;
    }

    public int getRemoteTouchResolution() {
        return remoteTouchResolution;
    }

    public void setRemoteTouchResolution(int remoteTouchResolution) {
        this.remoteTouchResolution = remoteTouchResolution;
    }

    public boolean isScreenLandscape() {
        return screenLandscape;
    }

    public void setScreenLandscape(boolean screenLandscape) {
        this.screenLandscape = screenLandscape;
    }

    public int getLoggingLevel() {
        return loggingLevel;
    }

    public void setLoggingLevel(int loggingLevel) {
        this.loggingLevel = loggingLevel;
    }

    public Boolean getStartGame() {
        return startGame;
    }

    public void setStartGame(Boolean startGame) {
        this.startGame = startGame;
    }

    public boolean isSaveTexture() {
        return saveTexture;
    }

    public void setSaveTexture(boolean saveTexture) {
        this.saveTexture = saveTexture;
    }

    public String getRedisIp() {
        return redisIp;
    }

    public void setRedisIp(String redisIp) {
        this.redisIp = redisIp;
    }

    public int getRedisPort() {
        return redisPort;
    }

    public void setRedisPort(int redisPort) {
        this.redisPort = redisPort;
    }

    @Override
    public String toString() {
        return "GameConf{" +
                "frameBufferDepth24=" + frameBufferDepth24 +
                ", showStat=" + showStat +
                ", saveTexture=" + saveTexture +
                ", agentAddress='" + agentAddress + '\'' +
                ", redisIp='" + redisIp + '\'' +
                ", redisPort=" + redisPort +
                ", gameId=" + gameId +
                ", remoteScreenResolution=" + remoteScreenResolution +
                ", remoteTouchResolution=" + remoteTouchResolution +
                ", screenLandscape=" + screenLandscape +
                ", loggingLevel=" + loggingLevel +
                ", startGame=" + startGame +
                ", clarityLevel=" + clarityLevel +
                ", name='" + name + '\'' +
                ", fixedLandscape=" + fixedLandscape +
                ", floaterVisible=" + floaterVisible +
                ", numberOfFramesSkipped=" + numberOfFramesSkipped +
                ", splashScreenImageResourceId=" + splashScreenImageResourceId +
                ", videoWaitingImageResourceId=" + videoWaitingImageResourceId +
                ", esGL=" + esGL +
                ", reconnectLastVm=" + reconnectLastVm +
                '}';
    }
}
