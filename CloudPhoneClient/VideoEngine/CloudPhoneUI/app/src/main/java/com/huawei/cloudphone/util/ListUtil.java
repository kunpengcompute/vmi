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

package com.huawei.cloudphone.util;

import java.util.List;

/**
 * ListUtil
 *
 * @author gj
 * @since 2019-09-24
 */
public class ListUtil {
    /**
     * 获取List的数据量
     *
     * @param list List
     * @return list.size
     */
    public static int getSize(List list) {
        return list == null ? 0 : list.size();
    }

    /**
     * 判断List是否为空
     *
     * @param list List
     * @return list isEmpty
     */
    public static boolean isEmpty(List list) {
        return list == null || list.size() == 0;
    }
}
