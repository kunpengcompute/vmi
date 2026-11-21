/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * vmi is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

package com.huawei.cloudphonesdk.maincontrol.config;

public class VmiConfigAudio extends VmiConfig{
    private int audioType = 0;
    private AudioPlayParams audioPlayParams;
    public int getAudioType() {
        return audioType;
    }

    public void setAudioType(int audioType) {
        this.audioType = audioType;
    }

    public AudioPlayParams getAudioPlayParams() {
        return audioPlayParams;
    }

    public void setAudioPlayParams(AudioPlayParams audioPlayParams) {
        this.audioPlayParams = audioPlayParams;
    }
}
