#pragma once

#include "pch.h"

class RVRAndroidContext {
public:
    static RVRAndroidContext* GetInstance();

    void Init(android_app* app);
    void HandleEvents(bool isSessionRunning);

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    static std::vector<std::string> GetInstanceExtensions();

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

    bool resumed = false;
    ANativeWindow* nativeWindow = nullptr;

protected:
    static RVRAndroidContext* instance_;

private:
    RVRAndroidContext() = default;
    void operator=(const RVRAndroidContext &) = delete;
    RVRAndroidContext(const RVRAndroidContext& other) = delete;

    android_app* app_ = nullptr;
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_;
};

