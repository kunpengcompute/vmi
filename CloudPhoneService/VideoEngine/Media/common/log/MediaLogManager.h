/*
 * 版权所有 (c) 华为技术有限公司 2021-2021
 * 功能说明: 媒体日志管理模块
 */
#ifndef MEDIA_LOG_MANAGER_H
#define MEDIA_LOG_MANAGER_H

#include <string>
#include "MediaLogDefs.h"

class MediaLogManager {
public:
    /**
     * @功能描述: 获取MediaLogManager单例对象
     * @返回值: 返回值MediaLogManager单例对象引用
     */
    static MediaLogManager& GetInstance();

    /**
     * @功能描述: 设置日志回调函数，同时如果回调函数不为空时修改默认日志级别为DEBUG级别
     * @参数 [in] logCallback: 日志回调函数
     */
    void SetLogCallback(MediaLogCallbackFunc logCallback);

    /**
     * @功能描述: 判断日志回调函数是否为空
     * @返回值: true 日志回调函数为空
     *          false 日志回调函数不为空
     */
    bool IsLogCallbackNull() const;

    /**
     * @功能描述: 调用回调函数
     * @参数 [in] level: 日志级别
     * @参数 [in] tag: 日志标签
     * @参数 [in] logData: 格式化输出数据
     */
    void Callback(int level, const std::string &tag, const std::string &logData) const;

    /**
     * @功能描述: 获取日志级别
     * @返回值: 日志级别，默认为INFO级别
     */
    MediaLogLevel GetLogLevel() const;

private:
    MediaLogManager();
    ~MediaLogManager() = default;
    MediaLogManager(const MediaLogManager&) = delete;
    MediaLogManager& operator=(const MediaLogManager&) = delete;
    MediaLogManager(MediaLogManager &&) = delete;
    MediaLogManager& operator=(MediaLogManager &&) = delete;

    MediaLogCallbackFunc m_logCallback = nullptr;
    MediaLogLevel m_logLevel = LOG_LEVEL_INFO;
};

#endif  // MEDIA_LOG_MANAGER_H
