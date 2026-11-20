/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
package com.huawei.cloudphonesdk.maincontrol;

import com.huawei.cloudphonesdk.maincontrol.config.EncodeParams;

/**
 * Native Listener.
 *
 * @since 2022-08-12
 */
public interface NativeListener {

    /**
     * video engine event notice.
     *
     * @param event video engine event.
     */
    void onVmiVideoEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3, String additionInfo);

    void onVmiFpsDataReceive(int fps, int jank, int bjank);

    void onVmiEncodeParamsReceive(EncodeParams encodeParams);
}
