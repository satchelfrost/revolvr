/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <extension_manager.h>
#include <global_context.h>

namespace rvr {
ExtensionManager::ExtensionManager(std::vector<RVRExtensions> exts) : rvrExts_(exts),
usingHandtracking_(false), usingPassthrough_(false) {
    auto androidExtensions = AndroidContext::GetInstanceExtensions();
    auto vulkanExtensions = VulkanContext::GetInstanceExtensions();
    for (auto& ext : exts) {
       switch (ext) {
           case RVRExtensions::HandTracking: {
               usingHandtracking_ = true;
               PushBackExtensions({XR_EXT_HAND_TRACKING_EXTENSION_NAME,
                                   XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME,
                                   XR_FB_HAND_TRACKING_AIM_EXTENSION_NAME,
                                   XR_FB_HAND_TRACKING_CAPSULES_EXTENSION_NAME});
               continue;
           }
           case RVRExtensions::PassThrough: {
               usingPassthrough_ = true;
               PushBackExtensions({XR_FB_PASSTHROUGH_EXTENSION_NAME});
               continue;
           }
        }
    }
    PushBackExtensions(androidExtensions);
    PushBackExtensions(vulkanExtensions);
    ConvertToCharPointers();
}

void ExtensionManager::PushBackExtensions(const std::vector<std::string>& extensionsToPush) {
    for (const auto& extension : extensionsToPush)
        extensions_.push_back(extension);
}

void ExtensionManager::EnsureExtensionsAvailable() {
    uint32_t numInputExtensions = 0;
    uint32_t numOutputExtensions = 0;
    xrEnumerateInstanceExtensionProperties(nullptr,
                                           numInputExtensions,
                                           &numOutputExtensions, nullptr);
    numInputExtensions = numOutputExtensions;
    std::vector<XrExtensionProperties> extensionProps(numOutputExtensions, {XR_TYPE_EXTENSION_PROPERTIES});
    xrEnumerateInstanceExtensionProperties(nullptr,
                                           numInputExtensions,
                                           &numOutputExtensions, extensionProps.data());

    for (const auto& requiredExtension : extensions_) {
        bool found = false;
        for (auto availableExtension : extensionProps) {
            if (!strcmp(requiredExtension.c_str(), availableExtension.extensionName)) {
                found = true;
                break;
            }
        }
        if (!found)
            THROW(Fmt("Required extension %s was not found", requiredExtension.c_str()));
    }
    Log::Write(Log::Level::Info, "All extension requirements satisfied");
}

std::vector<const char*> ExtensionManager::GetExtensions() {
    return extensionsAsCharPointers_;
}

void ExtensionManager::ConvertToCharPointers() {
    extensionsAsCharPointers_.reserve(extensions_.size());
    for (const std::string& str : extensions_) {
        char* cStr = new char[str.length() + 1];
        std::strcpy(cStr, str.c_str());
        extensionsAsCharPointers_.emplace_back(cStr);
    }
}

ExtensionManager::~ExtensionManager() {
    for (auto str : extensionsAsCharPointers_)
        delete[] str;
}
}