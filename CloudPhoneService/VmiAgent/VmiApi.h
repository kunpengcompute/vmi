/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 视频流云手机对外接口
 */

#ifndef VMI_API_H
#define VMI_API_H

#include <cstdint>
#include "VmiDef.h"

/**
 * @功能描述：获取产品版本号
 * @返回值：产品版本号字符串
*/
const char *GetVersion();

/**
 * @功能描述：初始化云手机服务端
 * @参数 [in] config：启动模块时使用的配置，参考VmiConfigEngine
 * @返回值：成功返回0(VmiErrCode::OK)，失败返回非0值，指示具体错误码
*/
VmiErrCode InitVmiEngine(VmiConfigEngine *config);

/**
 * @功能描述：解初始化云手机服务端
 * @返回值：成功返回0(VmiErrCode::OK)，失败返回非0值，指示具体错误码
*/
VmiErrCode DeInitVmiEngine();

/**
 * @功能描述：获取某个模块的状态
 * @参数 [in] module：对应模块的枚举类型
 * @返回值：对应模块的状态
*/
VmiModuleStatus GetStatus(VmiDataType module);

/**
 * @功能描述：启动某个模块
 * @参数 [in] module：对应模块的枚举类型
 * @参数 [in] config：启动模块时使用的配置，继承自VmiConfig
 *                   视频：参考VmiConfigVideo
 *                   音频播放：参考VmiConfigAudio
 *                   触控：参考VmiConfigTouch
 *                   麦克风：参考VmiConfigMic
 *                   传感器：暂未实现
 * @参数 [in] size：config指向内存的长度
 * @返回值：成功返回0(VmiErrCode::OK)，失败返回非0值，指示具体错误码
*/
VmiErrCode StartModule(VmiDataType module, uint8_t* config, uint32_t size);

/**
 * @功能描述：停止某个模块
 * @参数 [in] module：对应模块的枚举类型
 * @返回值：成功返回0(VmiErrCode::OK)，失败返回非0值，指示具体错误码
*/
VmiErrCode StopModule(VmiDataType module);

/**
 * @功能描述：向对应模块注入数据或命令
 * @参数 [in] module：对应模块的枚举类型
 * @参数 [in] cmd：要对模块进行的操作指令
 * @参数 [in] data：需要发送的数据
 *                 触控：参考VmiDef.h，支持注入触控和按键数据
 *                 麦克风：参考参考VmiDef.h，支持注入麦克风数据
 * @参数 [in] size：输入数据的长度
 * @返回值：成功返回0(VmiErrCode::OK)，失败返回非0值，指示具体错误码
*/
VmiErrCode InjectData(VmiDataType module, VmiCmd cmd, uint8_t *data, uint32_t size);

/**
 * @功能描述：向对应模块设置参数
 * @参数 [in] module：对应模块的枚举类型
 * @参数 [in] cmd：设置参数对应的指令
 * @参数 [in] param：需要设置的参数
 *                 视频：参考VmiDef.h，支持设置编码参数，需要传入EncodeParams
 *                 音频播放：参考VmiDef.h，支持设置音频播放参数，需要传入AudioPlayParams
                                        支持音频播放速度负反馈调节，需要传入客户端待播放的音频包数量，uint32_t类型
 * @参数 [in] size：输入数据的长度
 * @返回值：成功返回0(VmiErrCode::OK)，失败返回非0值，指示具体错误码
*/
VmiErrCode SetParam(VmiDataType module, VmiCmd cmd, uint8_t *param, uint32_t size);

/**
 * @功能描述：获取对应模块的参数
 * @参数 [in] module：对应模块的枚举类型
 * @参数 [in] cmd: 获取对应模块的指令，不同cmd所需存放输出参数指针的长度不一致，需参考VmiDef.h的定义
 * @参数 [in] param：存放输出参数的指针
 * @参数 [in] size：输出数据的长度
 * @返回值：成功返回0(VmiErrCode::OK)，失败返回非0值，指示具体错误码
 */
VmiErrCode GetParam(VmiDataType module, VmiCmd cmd, uint8_t *param, uint32_t size);

#endif
