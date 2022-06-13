#include "include/pch.h"
#include "include/common.h"
#include "include/rvr_android_platform.h"

RVRAndroidPlatform::RVRAndroidPlatform(android_app* app) {
    app_ = app;
    instanceCreateInfoAndroid_ = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    instanceCreateInfoAndroid_.applicationVM = app->activity->vm;
    instanceCreateInfoAndroid_.applicationActivity = app->activity->clazz;
}

XrBaseInStructure *RVRAndroidPlatform::GetInstanceCreateExtension() const {
    return (XrBaseInStructure *) &instanceCreateInfoAndroid_;
}

std::vector<std::string> RVRAndroidPlatform::GetInstanceExtensions() const {
    return {XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME};
}

AAssetManager *RVRAndroidPlatform::GetAndroidAssetManager() {
    return app_->activity->assetManager;
}


