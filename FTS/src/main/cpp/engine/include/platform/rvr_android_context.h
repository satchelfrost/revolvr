#pragma once

struct android_app;
struct AAssetManager;

class RVRAndroidContext {
public:
    RVRAndroidContext(android_app* app);

    void HandleEvents(bool isSessionRunning);

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    static std::vector<std::string> GetInstanceExtensions();

    bool resumed = false;
    ANativeWindow* nativeWindow = nullptr;

private:
    android_app* app_ = nullptr;
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_;
};

