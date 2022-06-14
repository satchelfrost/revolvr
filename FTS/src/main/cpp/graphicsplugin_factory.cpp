// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include <utility>

#include "include/pch.h"
#include "include/common.h"
#include "include/options.h"
#include "include/graphicsplugin.h"
#include "include/rvr_android_platform.h"

std::shared_ptr<IGraphicsPlugin> CreateGraphicsPlugin_Vulkan(const std::shared_ptr<Options>& options,
                                                             RVRAndroidPlatform& android_platform);

namespace {
using GraphicsPluginFactory = std::function<std::shared_ptr<IGraphicsPlugin>(const std::shared_ptr<Options>& options,
                                                                             RVRAndroidPlatform& android_platform)>;

std::map<std::string, GraphicsPluginFactory, IgnoreCaseStringLess> graphicsPluginMap = {
    {"Vulkan",
     [](const std::shared_ptr<Options>& options, RVRAndroidPlatform& android_platform) {
         return CreateGraphicsPlugin_Vulkan(options, android_platform);
     }},
};
}  // namespace

std::shared_ptr<IGraphicsPlugin> CreateGraphicsPlugin(const std::shared_ptr<Options>& options,
                                                      RVRAndroidPlatform& android_platform) {
    if (options->GraphicsPlugin.empty()) {
        ALOGE("No graphics API specified");
        exit(1);
    }

    const auto apiIt = graphicsPluginMap.find(options->GraphicsPlugin);
    if (apiIt == graphicsPluginMap.end()) {
        ALOGE("Unsupported graphics API '%s'", options->GraphicsPlugin.c_str());
        exit(1);
    }

    return CreateGraphicsPlugin_Vulkan(options, android_platform);
}
