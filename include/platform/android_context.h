#pragma once

#include "pch.h"

namespace rvr {
class AndroidContext {
public:
    static AndroidContext* Instance();

    void Init(android_app* app);
    void HandleEvents(bool isSessionRunning);

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    static std::vector<std::string> GetInstanceExtensions();

    void HandleAndroidCmd(android_app *app, int32_t cmd);

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

    bool resumed = false;
    ANativeWindow* nativeWindow = nullptr;

protected:
    static AndroidContext* instance_;

private:
    AndroidContext() = default;
    void operator=(const AndroidContext &) = delete;
    AndroidContext(const AndroidContext& other) = delete;

    android_app* app_ = nullptr;
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_;
};
}