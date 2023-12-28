/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <pch.h>
#include <common.h>
#include <platform/android_context.h>

namespace rvr {
static void AppHandleCmd(struct android_app* app, int32_t cmd) {
    auto state = reinterpret_cast<AndroidContext*>(app->userData);
    state->HandleAndroidCmd(app, cmd);
}

static int32_t AppHandleInput(struct android_app* app, AInputEvent* event) {
    auto state = reinterpret_cast<AndroidContext*>(app->userData);
    return state->HandleAndroidInput(app, event);
}

AndroidContext::AndroidContext(android_app *app) {
    app_ = app;
    JNIEnv* Env;
    app->activity->vm->AttachCurrentThread(&Env, nullptr);
    app->onAppCmd = AppHandleCmd;
    app->onInputEvent = AppHandleInput;
    app->userData = this;
    instanceCreateInfoAndroid_ = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    instanceCreateInfoAndroid_.applicationVM = app->activity->vm;
    instanceCreateInfoAndroid_.applicationActivity = app->activity->clazz;
}

XrBaseInStructure * AndroidContext::GetInstanceCreateExtension() const {
    return (XrBaseInStructure *) &instanceCreateInfoAndroid_;
}

void AndroidContext::HandleAndroidCmd(android_app *app, int32_t cmd) {
    switch (cmd) {
        // There is no APP_CMD_CREATE. The ANativeActivity creates the
        // application thread from onCreate(). The application thread
        // then calls android_main().
        case APP_CMD_START:
            break;
        case APP_CMD_RESUME:
            resumed = true;
            break;
        case APP_CMD_PAUSE:
            resumed = false;
            break;
        case APP_CMD_STOP:
            break;
        case APP_CMD_DESTROY:
            nativeWindow = nullptr;
            break;
        case APP_CMD_INIT_WINDOW:
            nativeWindow = app->window;
            break;
        case APP_CMD_TERM_WINDOW:
            nativeWindow = nullptr;
            break;
        default:
            break;
    }
}

std::vector<std::string> AndroidContext::GetInstanceExtensions() {
    return {XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME};
}

AAssetManager * AndroidContext::GetAndroidAssetManager() {
    return app_->activity->assetManager;
}

android_app* AndroidContext::GetAndroidApp() const {
    return app_;
}

void AndroidContext::HandleEvents(bool isSessionRunning) {
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

int32_t AndroidContext::HandleAndroidInput(android_app *app, AInputEvent *event) {
    int32_t eventSource = AInputEvent_getSource(event);
    switch (eventSource) {
        case AINPUT_SOURCE_MOUSE:
            break;
        case AINPUT_SOURCE_KEYBOARD:
            keyboardHandler_.HandleKeyEvent(event);
            break;
        default:
            break;
    }
    return 0;
}

AndroidContext::~AndroidContext() {
    app_->activity->vm->DetachCurrentThread();
}

std::vector<char> AndroidContext::GetProcessedKeyBuffer() {
    return keyboardHandler_.GetProcessedKeyBuffer();
}
}