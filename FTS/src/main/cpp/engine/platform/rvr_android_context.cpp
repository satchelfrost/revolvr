#include "pch.h"
#include "common.h"
#include "platform/rvr_android_context.h"

RVRAndroidContext* RVRAndroidContext::instance_ = nullptr;

RVRAndroidContext* RVRAndroidContext::GetInstance() {
    if (!instance_)
        instance_ = new RVRAndroidContext();
    return instance_;
}

void RVRAndroidContext::Init(android_app *app) {
    app_ = app;
    instanceCreateInfoAndroid_ = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    instanceCreateInfoAndroid_.applicationVM = app->activity->vm;
    instanceCreateInfoAndroid_.applicationActivity = app->activity->clazz;
}

XrBaseInStructure * RVRAndroidContext::GetInstanceCreateExtension() const {
    return (XrBaseInStructure *) &instanceCreateInfoAndroid_;
}

std::vector<std::string> RVRAndroidContext::GetInstanceExtensions() {
    return {XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME};
}

AAssetManager * RVRAndroidContext::GetAndroidAssetManager() {
    return app_->activity->assetManager;
}

android_app* RVRAndroidContext::GetAndroidApp() const {
    return app_;
}

void RVRAndroidContext::HandleEvents(bool isSessionRunning) {
    // Read all pending events.
    for (;;) {
        int events;
        struct android_poll_source* source;
        // If the timeout is zero, returns immediately without blocking.
        // If the timeout is negative, waits indefinitely until an event appears.
        const int timeoutMilliseconds =
                (!resumed && !isSessionRunning && app_->destroyRequested == 0) ? -1 : 0;
        if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void**)&source) < 0) {
            break;
        }

        // Process this event.
        if (source != nullptr) {
            source->process(app_, source);
        }
    }
}



