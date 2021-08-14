/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol;

import com.huawei.instructionstream.appui.utils.LogUtils;
import com.huawei.instructionstream.appui.R;

import java.io.Serializable;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Setting.
 *
 * @since 2018-07-05
 */
public class Setting implements Serializable {
    private static final long serialVersionUID = 226L;
    private static final String RESOLUTION_720P = "720";
    private static final String RESOLUTION_1280P = "1280";
    private static final String RESOLUTION_1920P = "1920";

    boolean testVmiTouchJava = false;
    boolean testAudioClient = false;
    boolean reconnectLastVm = false;
    boolean glEnabled = true;
    boolean inputEnabled = false;
    boolean audioEnabled = true;
    boolean vncEnabled = false;
    boolean opusEnabled = true;
    boolean saveTexture = false;

    // default TCP
    boolean frameBufferDepth24 = false;
    boolean glCompressionEnabled = false;
    int clarityLevel = 0;
    boolean esGL = false;
    boolean isShowRateAndCompressRatio = false;
    String proxyAddress;
    String proxyIp;
    int proxyPort;
    String redisIp;
    int redisPort;
    String name;
    String vncAddress = "192.168.2.1:8888";
    String vmId = "0";
    String gameId = "0";
    boolean stereoEnabled = true;
    boolean localAudioFileOnly = false;
    int sampleRate = 48000;
    int bufferSize = 32000;
    String screenResolution = "720";
    int screenResolutionPosition = 1;
    String screenHeight = null;
    String inputResolution = "720";
    int inputResolutionPosition = 1;
    String encoding = "zrle";
    int encodingPosition = 0;
    boolean screenLandscape = false;
    String loggingLevel = "info";
    int loggingLevelPosition = 1;
    Boolean startGame = true;
    String gameIcon = null;
    Boolean floaterVisible = true;
    int numberOfFramesSkipped = 0;
    int splashScreenImageResourceId = R.drawable.vmi_splash_screen_landscape;
    int videoWaitingImageResourceId;
    Boolean fixedLandscape = false;

    // 默认是一个无效的token
    String configServerToken = "123456";

    // 默认是一个无效的ID
    String defaultCallbackClassName = "com.huawei.cloudgame.decoder.FullscreenActivity";
    String callbackClassName = defaultCallbackClassName;
    int runningMode;
    int vssPort;

    /**
     * new default setting
     *
     * @param vncAddress vnc Address
     * @param proxyAddress proxy Address
     * @param vmId vm Id
     * @param gameId game Id
     * @param screenHorizontal screen Horizontal
     * @return Setting
     */
    public static Setting newDefaultSetting(String vncAddress, String proxyAddress, String vmId, String gameId,
                                            boolean screenHorizontal) {
        Setting setting = new Setting();
        setting.parseProxyAddress(proxyAddress);
        setting.setVncAddress(vncAddress);
        setting.vmId = vmId;
        setting.gameId = gameId;
        setting.screenLandscape = screenHorizontal;
        return setting;
    }

    /**
     * parse proxy address
     *
     * @param address address
     */
    public void parseProxyAddress(String address) {
        proxyAddress = address;
        Pattern pattern = Pattern.compile("^\\s*(.*?):(\\d+)\\s*$");
        Matcher matcher = pattern.matcher(proxyAddress);
        if (!matcher.matches()) {
            LogUtils.error("Settings", "could not parse proxyAddress");
        }
        proxyIp = proxyAddress.split(":")[0];
        proxyPort = Integer.parseInt(proxyAddress.split(":")[1]);
    }

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

    public boolean isEsGL() {
        return esGL;
    }

    public void setEsGL(boolean esGL) {
        this.esGL = esGL;
    }

    public boolean isShowRateAndCompressRatio() {
        return isShowRateAndCompressRatio;
    }

    public void setShowRateAndCompressRatio(boolean showRateAndCompressRatio) {
        isShowRateAndCompressRatio = showRateAndCompressRatio;
    }

    public boolean isDefaultCallbackClassName() {
        return defaultCallbackClassName.equals(callbackClassName);
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

    /**
     * get number of frames to skip
     *
     * @return numberOfFramesSkipped
     */
    public int getNumberOfFramesToSkip() {
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

    public String getGameIcon() {
        return gameIcon;
    }

    public void setGameIcon(String gameIcon) {
        this.gameIcon = gameIcon;
    }

    public boolean isSaveTexture() {
        return saveTexture;
    }

    public void setSaveTexture(boolean saveTexture) {
        this.saveTexture = saveTexture;
    }

    public boolean isGlEnabled() {
        return glEnabled;
    }

    public void setGlEnabled(boolean glEnabled) {
        this.glEnabled = glEnabled;
    }

    public boolean isInputEnabled() {
        return inputEnabled;
    }

    public void setInputEnabled(boolean inputEnabled) {
        this.inputEnabled = inputEnabled;
    }

    public boolean isAudioEnabled() {
        return audioEnabled;
    }

    public void setAudioEnabled(boolean audioEnabled) {
        this.audioEnabled = audioEnabled;
    }

    public boolean isVncEnabled() {
        return vncEnabled;
    }

    public void setVncEnabled(boolean vncEnabled) {
        this.vncEnabled = vncEnabled;
    }

    public String getProxyAddress() {
        return proxyAddress;
    }

    public void setProxyAddress(String proxyAddress) {
        this.proxyAddress = proxyAddress;
    }

    public String getProxyIp() {
        return proxyIp;
    }

    public void setProxyIp(String proxyIp) {
        this.proxyIp = proxyIp;
    }

    public int getProxyPort() {
        return proxyPort;
    }

    public void setProxyPort(int proxyPort) {
        this.proxyPort = proxyPort;
    }

    public String getVncAddress() {
        return vncAddress;
    }

    public void setVncAddress(String vncAddress) {
        this.vncAddress = vncAddress;
    }

    public String getVmId() {
        return vmId;
    }

    public void setVmId(String vmId) {
        this.vmId = vmId;
    }

    public String getGameId() {
        return gameId;
    }

    public void setGameId(String gameId) {
        this.gameId = gameId;
    }

    public boolean isStereoEnabled() {
        return stereoEnabled;
    }

    public void setStereoEnabled(boolean stereoEnabled) {
        this.stereoEnabled = stereoEnabled;
    }

    public boolean isLocalAudioFileOnly() {
        return localAudioFileOnly;
    }

    public void setLocalAudioFileOnly(boolean localAudioFileOnly) {
        this.localAudioFileOnly = localAudioFileOnly;
    }

    public int getSampleRate() {
        return sampleRate;
    }

    public void setSampleRate(int sampleRate) {
        this.sampleRate = sampleRate;
    }

    public int getBufferSize() {
        return bufferSize;
    }

    public void setBufferSize(int bufferSize) {
        this.bufferSize = bufferSize;
    }

    /**
     * get Vm screen width
     *
     * @return screenResolution
     */
    public String getVmScreenWidth() {
        return screenResolution;
    }

    public String getInputResolution() {
        return inputResolution;
    }

    public void setInputResolution(String inputResolution) {
        this.inputResolution = inputResolution;
    }

    public String getEncoding() {
        return encoding;
    }

    public void setEncoding(String encoding) {
        this.encoding = encoding;
    }

    public int getEncodingPosition() {
        return encodingPosition;
    }

    public void setEncodingPosition(int encodingPosition) {
        this.encodingPosition = encodingPosition;
    }

    public boolean isScreenLandscape() {
        return screenLandscape;
    }

    public void setScreenLandscape(boolean screenLandscape) {
        this.screenLandscape = screenLandscape;
    }

    public int getScreenResolutionPosition() {
        return screenResolutionPosition;
    }

    public void setScreenResolutionPosition(int screenResolutionPosition) {
        this.screenResolutionPosition = screenResolutionPosition;
    }

    public int getInputResolutionPosition() {
        return inputResolutionPosition;
    }

    public void setInputResolutionPosition(int inputResolutionPosition) {
        this.inputResolutionPosition = inputResolutionPosition;
    }

    public boolean isOpusEnabled() {
        return opusEnabled;
    }

    public void setOpusEnabled(boolean opusEnabled) {
        this.opusEnabled = opusEnabled;
    }

    public String getLoggingLevel() {
        return loggingLevel;
    }

    public void setLoggingLevel(String loggingLevel) {
        this.loggingLevel = loggingLevel;
    }

    public int getLoggingLevelPosition() {
        return loggingLevelPosition;
    }

    public void setLoggingLevelPosition(int loggingLevelPosition) {
        this.loggingLevelPosition = loggingLevelPosition;
    }

    public boolean isGlCompressionEnabled() {
        return glCompressionEnabled;
    }

    public void setGlCompressionEnabled(boolean glCompressionEnabled) {
        this.glCompressionEnabled = glCompressionEnabled;
    }

    /**
     * get Vm screen height
     *
     * @return screenHeight
     */
    public String getVmScreenHeight() {
        return screenHeight;
    }

    public String getScreenResolution() {
        return screenResolution;
    }

    public void setScreenResolution(String screenResolution) {
        this.screenResolution = screenResolution;
        this.screenHeight = RESOLUTION_720P.equals(this.screenResolution) ? RESOLUTION_1280P : RESOLUTION_1920P;
    }

    public String getScreenHeight() {
        return screenHeight;
    }

    public void setScreenHeight(String screenHeight) {
        this.screenHeight = screenHeight;
    }

    public Boolean getStartGame() {
        return startGame;
    }

    public void setStartGame(Boolean startGame) {
        this.startGame = startGame;
    }

    public boolean isFrameBufferDepth24() {
        return frameBufferDepth24;
    }

    public void setFrameBufferDepth24(boolean frameBufferDepth24) {
        this.frameBufferDepth24 = frameBufferDepth24;
    }

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
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

    public int getClarityLevel() {
        return clarityLevel;
    }

    public void setClarityLevel(int clarityLevel) {
        this.clarityLevel = clarityLevel;
    }

    public int getRunningMode() {
        return runningMode;
    }

    public void setRunningMode(int runningMode) {
        this.runningMode = runningMode;
    }

    public int getVssPort() {
        return vssPort;
    }

    public void setVssPort(int vssPort) {
        this.vssPort = vssPort;
    }

    @Override
    public String toString() {
        return "Setting{" +
                "glEnabled=" + glEnabled +
                ", inputEnabled=" + inputEnabled +
                ", audioEnabled=" + audioEnabled +
                ", vncEnabled=" + vncEnabled +
                ", opusEnabled=" + opusEnabled +
                ", saveTexture=" + saveTexture +
                ", frameBufferDepth24=" + frameBufferDepth24 +
                ", glCompressionEnabled=" + glCompressionEnabled +
                ", clarityLevel=" + clarityLevel +
                ", esGL=" + esGL +
                ", isShowRateAndCompressRatio=" + isShowRateAndCompressRatio +
                ", proxyAddress='" + proxyAddress + '\'' +
                ", proxyIp='" + proxyIp + '\'' +
                ", proxyPort=" + proxyPort +
                ", redisIp='" + redisIp + '\'' +
                ", redisPort=" + redisPort +
                ", name='" + name + '\'' +
                ", vncAddress='" + vncAddress + '\'' +
                ", vmId='" + vmId + '\'' +
                ", gameId='" + gameId + '\'' +
                ", stereoEnabled=" + stereoEnabled +
                ", localAudioFileOnly=" + localAudioFileOnly +
                ", sampleRate=" + sampleRate +
                ", bufferSize=" + bufferSize +
                ", screenResolution='" + screenResolution + '\'' +
                ", screenResolutionPosition=" + screenResolutionPosition +
                ", screenHeight='" + screenHeight + '\'' +
                ", inputResolution='" + inputResolution + '\'' +
                ", inputResolutionPosition=" + inputResolutionPosition +
                ", encoding='" + encoding + '\'' +
                ", encodingPosition=" + encodingPosition +
                ", screenLandscape=" + screenLandscape +
                ", loggingLevel='" + loggingLevel + '\'' +
                ", loggingLevelPosition=" + loggingLevelPosition +
                ", startGame=" + startGame +
                ", gameIcon='" + gameIcon + '\'' +
                ", floaterVisible=" + floaterVisible +
                ", numberOfFramesSkipped=" + numberOfFramesSkipped +
                ", splashScreenImageResourceId=" + splashScreenImageResourceId +
                ", videoWaitingImageResourceId=" + videoWaitingImageResourceId +
                ", fixedLandscape=" + fixedLandscape +
                ", reconnectLastVm=" + reconnectLastVm +
                '}';
    }
}
