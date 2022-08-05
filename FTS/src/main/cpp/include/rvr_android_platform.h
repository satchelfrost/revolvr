#pragma once

struct android_app;
struct AAssetManager;

class RVRAndroidPlatform {
public:
    RVRAndroidPlatform();
    void Init(android_app* app);
    static RVRAndroidPlatform* GetInstance();

    void operator=(const RVRAndroidPlatform &) = delete;
    RVRAndroidPlatform(RVRAndroidPlatform& other) = delete;

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    std::vector<std::string> GetInstanceExtensions() const;

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

protected:
    static RVRAndroidPlatform* instance_;

private:
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_;
    android_app* app_;
};

