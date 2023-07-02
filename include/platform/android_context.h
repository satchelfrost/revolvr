/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include "pch.h"
#include <platform/keyboard_handler.h>

namespace rvr {
// Callbacks for Android commands and input events
static void AppHandleCmd(struct android_app* app, int32_t cmd);
static int32_t AppHandleInput(struct android_app* app, AInputEvent* event);

class AndroidContext {
public:
    AndroidContext(android_app* app);
    ~AndroidContext();
    void HandleEvents(bool isSessionRunning);
    std::vector<char> GetProcessedKeyBuffer();

    // Provide extension to XrInstanceCreateInfo for xrCreateInstance.
    XrBaseInStructure* GetInstanceCreateExtension() const;

    // OpenXR instance-level extensions required by this platform.
    std::vector<std::string> GetInstanceExtensions();

    void HandleAndroidCmd(android_app *app, int32_t cmd);
    int32_t HandleAndroidInput(android_app *app, AInputEvent* event);

    AAssetManager* GetAndroidAssetManager();

    android_app* GetAndroidApp() const;

    bool resumed = false;
    ANativeWindow* nativeWindow = nullptr;

private:
    android_app* app_ = nullptr;
    XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid_{};
    KeyboardHandler keyboardHandler_;
};
}