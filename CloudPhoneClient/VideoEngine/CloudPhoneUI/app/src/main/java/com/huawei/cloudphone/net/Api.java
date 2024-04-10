/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.net;

import com.huawei.cloudphone.model.ApplyDeviceBean;
import com.huawei.cloudphone.model.CheckUpdateBean;
import com.huawei.cloudphone.model.DeviceListBean;
import com.huawei.cloudphone.model.RegionListBean;
import com.huawei.cloudphone.model.Response;
import com.huawei.cloudphone.model.TemplateBean;
import com.huawei.cloudphone.model.UserBean;

import java.util.ArrayList;

import io.reactivex.Observable;
import okhttp3.RequestBody;
import retrofit2.http.Body;
import retrofit2.http.Field;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.HTTP;
import retrofit2.http.POST;
import retrofit2.http.Query;
import retrofit2.http.Url;

/**
 * 所有的请求声明在这里，接口信息
 *
 * @author zhh
 * @since 2019-09-24
 */
public interface Api {
    /**
     * 登录
     *
     * @param url      请求路径
     * @param action   登录Action
     * @param userName 用户名
     * @param pwd      密码
     * @return 用户信息
     */
    @FormUrlEncoded
    @POST
    Observable<UserBean> login(
        @Url String url,
        @Field("action") String action,
        @Field("username") String userName,
        @Field("pwd") String pwd);

    /**
     * 退出登录
     *
     * @param url      请求路径
     * @param action   登出Action
     * @param userName 用户名
     * @param token    登录Token
     * @return 登出结果
     */
    @FormUrlEncoded
    @POST
    Observable<Response> logout(
        @Url String url,
        @Field("action") String action,
        @Field("username") String userName,
        @Field("token") String token);

    /**
     * 注册
     *
     * @param url      请求路径
     * @param action   注册Action
     * @param userName 用户名
     * @param pwd      密码
     * @return 注册结果
     */
    @FormUrlEncoded
    @POST
    Observable<Response> register(
        @Url String url,
        @Field("action") String action,
        @Field("username") String userName,
        @Field("pwd") String pwd);

    /**
     * 获取设备列表
     *
     * @param url         请求路径
     * @param userId      用户id
     * @param userName    用户名
     * @param token       登录Token
     * @param returnStyle 返回数据格式
     * @return 设备列表数据
     */
    @GET
    Observable<DeviceListBean> getDevices(
        @Url String url,
        @Query("userId") long userId,
        @Query("username") String userName,
        @Query("token") String token,
        @Query("returnStyle") String returnStyle);

    /**
     * 重启设备
     *
     * @param url      请求路径
     * @param userId   用户id
     * @param userName 用户名
     * @param token    登录Token
     * @param alias    设备别名
     * @return 返回结果
     */
    @GET
    Observable<Response> reStartDevices(
        @Url String url,
        @Query("userId") long userId,
        @Query("username") String userName,
        @Query("token") String token,
        @Query("alias") String alias);

    /**
     * 删除设备
     *
     * @param url      请求路径
     * @param userName 用户名
     * @param token    登录Token
     * @param body     放在body中的json格式数据
     * @return 删除结果
     */
    @HTTP(method = "DELETE", hasBody = true)
    Observable<Response> deleteDevice(
        @Url String url,
        @Query("username") String userName,
        @Query("token") String token,
        @Body RequestBody body);

    /**
     * 获取设备区域信息
     *
     * @param url      请求路径
     * @param userId   用户id
     * @param userName 用户名
     * @param token    登录Token
     * @param devType  type
     * @return 区域信息
     */
    @GET
    Observable<RegionListBean> getRegions(
        @Url String url,
        @Query("userId") long userId,
        @Query("username") String userName,
        @Query("token") String token,
        @Query("devType") int devType);

    /**
     * 获取设备模板信息
     *
     * @param url           请求路径
     * @param userName      用户名
     * @param token         登录Token
     * @param template_type 模板类型
     * @return 模板信息
     */
    @GET
    Observable<ArrayList<TemplateBean>> getTemplates(
        @Url String url,
        @Query("username") String userName,
        @Query("token") String token,
        @Query("template_type") int template_type);

    /**
     * 申请设备
     *
     * @param url      请求路径
     * @param userName 用户名
     * @param token    登录Token
     * @param body     放在body中的json格式数据
     * @return 设备信息
     */
    @POST
    Observable<ApplyDeviceBean> applyDevice(
        @Url String url,
        @Query("username") String userName,
        @Query("token") String token,
        @Body RequestBody body);

    /**
     * 检查客户端更新
     *
     * @param url     请求路径
     * @param apkName 应用名称
     * @param reqObj  请求对象
     * @return 用户信息
     */
    @GET
    Observable<CheckUpdateBean> checkUpdate(
        @Url String url,
        @Query("apkName") String apkName,
        @Query("reqObj") String reqObj);


    /**
     * 日志上传
     *
     * @param url              请求路径
     * @param paramRequestBody 参数体
     * @return 上传结果
     */
    @POST
    Observable<Response> uploadLog(
        @Url String url,
        @Body RequestBody paramRequestBody);
}
