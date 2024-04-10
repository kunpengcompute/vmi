/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.model;

/**
 * 用户信息实体类
 *
 * @author zhh
 * @since 2019-09-24
 */
public class UserBean extends Response {
    // 用户id
    private long userid;

    // Token
    private String token;

    // 用户名
    private String username;

    public long getUserid() {
        return userid;
    }

    public void setUserid(long userid) {
        this.userid = userid;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    @Override
    public String toString() {
        return "UserBean{" +
                "userid=" + userid +
                ", token='" + token + '\'' +
                ", username='" + username + '\'' +
                '}';
    }
}
