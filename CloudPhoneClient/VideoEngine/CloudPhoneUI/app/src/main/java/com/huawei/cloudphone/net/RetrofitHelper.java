/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.net;

import com.huawei.cloudphone.BuildConfig;
import com.huawei.cloudphone.MyApplication;

import java.util.concurrent.TimeUnit;

import okhttp3.OkHttpClient;
import okhttp3.logging.HttpLoggingInterceptor;
import retrofit2.Retrofit;
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.gson.GsonConverterFactory;
import retrofit2.converter.scalars.ScalarsConverterFactory;

/**
 * 请求信息
 *
 * @author zhh
 * @since 2019-09-24
 */
public class RetrofitHelper {
    /**
     * Retrofit instance
     */
    public static Retrofit sRetrofit;

    /**
     * 获取接口函数
     *
     * @return Api
     */
    public static Api getApi() {
        if (sRetrofit == null) {
            synchronized (Retrofit.class) {
                // 创建日志拦截器
                HttpLoggingInterceptor loggingInterceptor = new HttpLoggingInterceptor();
                loggingInterceptor.setLevel(HttpLoggingInterceptor.Level.BODY);
                // 初始化OKHttp
                OkHttpClient.Builder builder = new OkHttpClient.Builder();
                if (BuildConfig.DEBUG) {
                    builder.addInterceptor(loggingInterceptor); // 添加日志拦截器
                }
                builder.readTimeout(10, TimeUnit.SECONDS) // 设置读取超时时间
                    .writeTimeout(10, TimeUnit.SECONDS) // 设置写的超时时间
                    .connectTimeout(10, TimeUnit.SECONDS); // 设置连接超时时间

                sRetrofit = new Retrofit.Builder()
                    .addConverterFactory(ScalarsConverterFactory.create()) // 支持返回字符串
                    .addConverterFactory(GsonConverterFactory.create()) // 支持返回值JavaBean对象
                    .addCallAdapterFactory(RxJava2CallAdapterFactory.create()) // 支持返回值Observable<T>
                    .client(builder.build())
                    .baseUrl(MyApplication.instance.getBaseUrl())
                    .build();
            }
        }
        return sRetrofit.create(Api.class);
    }
}
