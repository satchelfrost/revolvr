// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include <utility>

#include "pch.h"
#include "common.h"
#include "options.h"
#include "platformdata.h"
#include "graphicsplugin.h"

// Graphics API factories are forward declared here.
#ifdef XR_USE_GRAPHICS_API_VULKAN
std::shared_ptr<IGraphicsPlugin> CreateGraphicsPlugin_VulkanLegacy(const std::shared_ptr<Options>& options,
                                                                   std::shared_ptr<IPlatformPlugin> platformPlugin);

std::shared_ptr<IGraphicsPlugin> CreateGraphicsPlugin_Vulkan(const std::shared_ptr<Options>& options,
                                                             std::shared_ptr<IPlatformPlugin> platformPlugin);
#endif

namespace {
using GraphicsPluginFactory = std::function<std::shared_ptr<IGraphicsPlugin>(const std::shared_ptr<Options>& options,
                                                                             std::shared_ptr<IPlatformPlugin> platformPlugin)>;

std::map<std::string, GraphicsPluginFactory, IgnoreCaseStringLess> graphicsPluginMap = {
#ifdef XR_USE_GRPHICS_API_VULKAN
    {"Vulkan",
     [](const std::shared_ptr<Options>& options, std::shared_ptr<IPlatformPlugin> platformPlugin) {
         return CreateGraphicsPlugin_VulkanLegacy(options, std::move(platformPlugin));
     }},
    {"Vulkan2",
     [](const std::shared_ptr<Options>& options, std::shared_ptr<IPlatformPlugin> platformPlugin) {
         return CreateGraphicsPlugin_Vulkan(options, std::move(platformPlugin));
     }},
#endif
};
}  // namespace

std::shared_ptr<IGraphicsPlugin> CreateGraphicsPlugin(const std::shared_ptr<Options>& options,
                                                      std::shared_ptr<IPlatformPlugin> platformPlugin) {
    if (options->GraphicsPlugin.empty()) {
        throw std::invalid_argument("No graphics API specified");
    }

    const auto apiIt = graphicsPluginMap.find(options->GraphicsPlugin);
    if (apiIt == graphicsPluginMap.end()) {
        throw std::invalid_argument(Fmt("Unsupported graphics API '%s'", options->GraphicsPlugin.c_str()));
    }

    return apiIt->second(options, std::move(platformPlugin));
}
