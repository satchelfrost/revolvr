#pragma once

struct android_app;
struct AAssetManager;

class RVRAndroidContext {
public:
    RVRAndroidContext();
    static RVRAndroidContext* GetInstance();

    void operator=(const RVRAndroidContext &) = delete;
    RVRAndroidContext(RVRAndroidContext& other) = delete;

    void Init(android_app* app);
    void HandleEvents(bool isSessionRunning);

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    std::vector<std::string> GetInstanceExtensions() const;

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

    bool resumed = false;
    ANativeWindow* nativeWindow = nullptr;

protected:
    static RVRAndroidPlatform* instance_;

private:
    android_app* app_ = nullptr;
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_;
};

