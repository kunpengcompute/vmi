/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
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
