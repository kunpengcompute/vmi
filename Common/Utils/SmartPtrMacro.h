/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2024. All rights reserved.
 * Description: singleton template
 */
#ifndef SMART_PTR_MACRO_H
#define SMART_PTR_MACRO_H
#include <memory>
#include "logging.h"

#define MAKE_UNIQUE_NOTHROW(ptr, type, ...) do {                                \
        try {                                                                   \
            ptr = std::make_unique<type>(__VA_ARGS__);                          \
        } catch (const std::bad_alloc &) {                                      \
            ERR("bad alloc execption catched");                                 \
            ptr = nullptr;                                                      \
        } catch (const std::exception &e) {                                     \
            ERR("exception caught: %s !", e.what());                            \
            ptr = nullptr;                                                      \
        }                                                                       \
    } while (0)

#define MAKE_SHARED_NOTHROW(ptr, type, ...) do {                                \
        try {                                                                   \
            ptr = std::make_shared<type>(__VA_ARGS__);                          \
        } catch (const std::bad_alloc &) {                                      \
            ERR("bad alloc execption catched");                                 \
            ptr = nullptr;                                                      \
        } catch (const std::exception &e) {                                     \
            ERR("exception caught: %s !", e.what());                            \
            ptr = nullptr;                                                      \
        }                                                                       \
    } while (0)

#endif
