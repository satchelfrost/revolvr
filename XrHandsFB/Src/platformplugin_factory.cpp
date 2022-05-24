// Copyright (c) 2017-2022, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include "pch.h"
#include "platformplugin.h"

#define UNUSED_PARM(x) \
    { (void)(x); }

std::shared_ptr<IPlatformPlugin> CreatePlatformPlugin_Android(const std::shared_ptr<Options>& /*unused*/,
                                                              const std::shared_ptr<PlatformData>& /*unused*/);

std::shared_ptr<IPlatformPlugin> CreatePlatformPlugin(const std::shared_ptr<Options>& options,
                                                      const std::shared_ptr<PlatformData>& data) {
#if !defined(XR_USE_PLATFORM_ANDROID)
    UNUSED_PARM(data);
#endif

#if defined(XR_USE_PLATFORM_WIN32)
    return CreatePlatformPlugin_Win32(options);
#elif defined(XR_USE_PLATFORM_ANDROID)
    return CreatePlatformPlugin_Android(options, data);
#elif defined(XR_USE_PLATFORM_XLIB)
    return CreatePlatformPlugin_Xlib(options);
#elif defined(XR_USE_PLATFORM_XCB)
    return CreatePlatformPlugin_Xcb(options);
#elif defined(XR_USE_PLATFORM_WAYLAND)
    return CreatePlatformPlugin_Wayland(options);
#else
#error Unsupported platform or no XR platform defined!
#endif
}
