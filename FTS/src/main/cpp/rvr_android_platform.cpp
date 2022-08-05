#include "include/pch.h"
#include "include/common.h"
#include "include/rvr_android_platform.h"

RVRAndroidPlatform* RVRAndroidPlatform::instance_ = nullptr;

RVRAndroidPlatform::RVRAndroidPlatform() {}

RVRAndroidPlatform* RVRAndroidPlatform::GetInstance() {
    if (!instance_)
        instance_ = new RVRAndroidPlatform();
    return instance_;
}

void RVRAndroidPlatform::Init(android_app* app) {
    app_ = app;
    instanceCreateInfoAndroid_ = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    instanceCreateInfoAndroid_.applicationVM = app->activity->vm;
    instanceCreateInfoAndroid_.applicationActivity = app->activity->clazz;
}

XrBaseInStructure * RVRAndroidPlatform::GetInstanceCreateExtension() const {
    CHECK_MSG(app_, "Android app has not been initialized")
    return (XrBaseInStructure *) &instanceCreateInfoAndroid_;
}

std::vector<std::string> RVRAndroidPlatform::GetInstanceExtensions() const {
    CHECK_MSG(app_, "Android app has not been initialized")
    return {XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME};
}

AAssetManager * RVRAndroidPlatform::GetAndroidAssetManager() {
    CHECK_MSG(app_, "Android app has not been initialized")
    return app_->activity->assetManager;
}

android_app* RVRAndroidPlatform::GetAndroidApp() const {
    CHECK_MSG(app_, "Android app has not been initialized")
    return app_;
}



