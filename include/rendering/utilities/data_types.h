/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once
#include <pch.h>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() {return graphicsFamily.has_value();}
};

enum class DataType {
    U8,
    U16,
    U32,
    F32
};

enum class MemoryType {
    DeviceLocal,
    HostVisible
};
