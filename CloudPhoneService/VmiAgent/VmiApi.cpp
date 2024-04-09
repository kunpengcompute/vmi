#include "VmiApi.h"

const char *GetVersion()
{
    return nullptr;
}

VmiErrCode InitVmiEngine(VmiConfigEngine * /*config*/)
{
    return VmiErrCode::OK;
}

VmiErrCode DeInitVmiEngine()
{
    return VmiErrCode::OK;
}

VmiModuleStatus GetStatus(VmiDataType /*module*/)
{
    return VmiModuleStatus::MODULE_NOT_INIT;
}

VmiErrCode StartModule(VmiDataType /*module*/, uint8_t* /*config*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}

VmiErrCode StopModule(VmiDataType /*module*/)
{
    return VmiErrCode::OK;
}

VmiErrCode InjectData(VmiDataType /*module*/, VmiCmd /*cmd*/, uint8_t */*data*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}

VmiErrCode SetParam(VmiDataType /*module*/, VmiCmd /*cmd*/, uint8_t */*param*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}

VmiErrCode GetParam(VmiDataType /*module*/, VmiCmd /*cmd*/, uint8_t */*param*/, uint32_t /*size*/)
{
    return VmiErrCode::OK;
}