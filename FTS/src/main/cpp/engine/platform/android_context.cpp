#include "pch.h"
#include "common.h"
#include "platform/rvr_android_context.h"

RVRAndroidContext* RVRAndroidContext::instance_ = nullptr;

RVRAndroidContext* RVRAndroidContext::Instance() {
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

void RVRAndroidContext::HandleAndroidCmd(android_app *app, int32_t cmd) {
    switch (cmd) {
        // There is no APP_CMD_CREATE. The ANativeActivity creates the
        // application thread from onCreate(). The application thread
        // then calls android_main().
        case APP_CMD_START: {
            Log::Write(Log::Level::Info, "    APP_CMD_START");
            Log::Write(Log::Level::Info, "onStart()");
            break;
        }
        case APP_CMD_RESUME: {
            Log::Write(Log::Level::Info, "onResume()");
            Log::Write(Log::Level::Info, "    APP_CMD_RESUME");
            resumed = true;
            break;
        }
        case APP_CMD_PAUSE: {
            Log::Write(Log::Level::Info, "onPause()");
            Log::Write(Log::Level::Info, "    APP_CMD_PAUSE");
            resumed = false;
            break;
        }
        case APP_CMD_STOP: {
            Log::Write(Log::Level::Info, "onStop()");
            Log::Write(Log::Level::Info, "    APP_CMD_STOP");
            break;
        }
        case APP_CMD_DESTROY: {
            Log::Write(Log::Level::Info, "onDestroy()");
            Log::Write(Log::Level::Info, "    APP_CMD_DESTROY");
            nativeWindow = nullptr;
            break;
        }
        case APP_CMD_INIT_WINDOW: {
            Log::Write(Log::Level::Info, "surfaceCreated()");
            Log::Write(Log::Level::Info, "    APP_CMD_INIT_WINDOW");
            nativeWindow = app->window;
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            Log::Write(Log::Level::Info, "surfaceDestroyed()");
            Log::Write(Log::Level::Info, "    APP_CMD_TERM_WINDOW");
            nativeWindow = nullptr;
            break;
        }
        default:
            break;
    }
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



