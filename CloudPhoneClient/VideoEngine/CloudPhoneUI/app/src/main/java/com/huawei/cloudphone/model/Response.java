/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.model;

/**
 * 返回信息
 *
 * @author zhh
 * @since 2019-09-24
 */
public class Response {
    // 错误码
    private int errno;

    // 错误信息
    private String errmsg;

    public int getErrno() {
        return errno;
    }

    public void setErrno(int errno) {
        this.errno = errno;
    }

    public String getErrmsg() {
        return errmsg;
    }

    public void setErrmsg(String errmsg) {
        this.errmsg = errmsg;
    }

    @Override
    public String toString() {
        return "Response{" +
            "errno=" + errno +
            ", errmsg='" + errmsg + '\'' +
            '}';
    }
}
