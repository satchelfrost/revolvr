#pragma once

#include "pch.h"

namespace rvr {
class AndroidContext {
public:
    AndroidContext(android_app* app);
    void HandleEvents(bool isSessionRunning);

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    std::vector<std::string> GetInstanceExtensions();

    void HandleAndroidCmd(android_app *app, int32_t cmd);

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

    bool resumed = false;
    ANativeWindow* nativeWindow = nullptr;

private:
    android_app* app_ = nullptr;
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_{};
};
}