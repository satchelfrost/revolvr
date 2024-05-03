/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

enum class RVRExtensions {
    PassThrough,
    HandTracking, // TODO: currently enabled by default
};

namespace rvr {
class ExtensionManager {
public:
    ExtensionManager(std::vector<RVRExtensions> exts);
    ~ExtensionManager();
    void EnsureExtensionsAvailable();
    std::vector<const char*> GetExtensions();
    std::vector<RVRExtensions> rvrExts_;
    bool usingPassthrough_;
    bool usingHandtracking_;

private:
    void ConvertToCharPointers();
    void PushBackExtensions(const std::vector<std::string>& extensionsToPush);
    std::vector<std::string> extensions_;
    std::vector<const char*> extensionsAsCharPointers_;
};
}