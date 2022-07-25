#pragma once

struct android_app;
struct AAssetManager;

class RVRAndroidPlatform {
public:
    RVRAndroidPlatform(android_app* app);

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    std::vector<std::string> GetInstanceExtensions() const;

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

private:
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_;
    android_app* app_;
};

