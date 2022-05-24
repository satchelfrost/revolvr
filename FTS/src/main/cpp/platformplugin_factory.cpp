// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include "include/pch.h"
#include "include/platformplugin.h"
#include "include/common.h"

#define UNUSED_PARM(x) \
    { (void)(x); }

std::shared_ptr<IPlatformPlugin> CreatePlatformPlugin_Android(const std::shared_ptr<Options>& /*unused*/,
                                                              const std::shared_ptr<PlatformData>& /*unused*/);

std::shared_ptr<IPlatformPlugin> CreatePlatformPlugin(const std::shared_ptr<Options>& options,
                                                      const std::shared_ptr<PlatformData>& data) {
    return CreatePlatformPlugin_Android(options, data);
}
