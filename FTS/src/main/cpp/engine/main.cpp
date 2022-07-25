#include "include/pch.h"
#include "include/common.h"
#include "include/options.h"
#include "include/rvr_android_platform.h"
#include "include/rvr_vulkan_renderer.h"
#include "include/rvr_app.h"
#include "include/rvr_game_loop_timer.h"

namespace {

void ShowHelp() { Log::Write(Log::Level::Info, "adb shell setprop debug.xr.graphicsPlugin OpenGLES|Vulkan"); }

bool UpdateOptionsFromSystemProperties(Options& options) {
    options.GraphicsPlugin = "Vulkan";

    char value[PROP_VALUE_MAX] = {};
    if (__system_property_get("debug.xr.graphicsPlugin", value) != 0) {
        options.GraphicsPlugin = value;
    }

    // Check for required parameters.
    if (options.GraphicsPlugin.empty()) {
        Log::Write(Log::Level::Error, "GraphicsPlugin parameter is required");
        ShowHelp();
        return false;
    }

    return true;
}
}  // namespace

struct AndroidAppState {
    ANativeWindow* NativeWindow = nullptr;
    bool Resumed = false;
};

/**
 * Process the next main command.
 */
static void app_handle_cmd(struct android_app* app, int32_t cmd) {
    AndroidAppState* appState = (AndroidAppState*)app->userData;

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
            appState->Resumed = true;
            break;
        }
        case APP_CMD_PAUSE: {
            Log::Write(Log::Level::Info, "onPause()");
            Log::Write(Log::Level::Info, "    APP_CMD_PAUSE");
            appState->Resumed = false;
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
            appState->NativeWindow = NULL;
            break;
        }
        case APP_CMD_INIT_WINDOW: {
            Log::Write(Log::Level::Info, "surfaceCreated()");
            Log::Write(Log::Level::Info, "    APP_CMD_INIT_WINDOW");
            appState->NativeWindow = app->window;
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            Log::Write(Log::Level::Info, "surfaceDestroyed()");
            Log::Write(Log::Level::Info, "    APP_CMD_TERM_WINDOW");
            appState->NativeWindow = NULL;
            break;
        }
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
    JNIEnv* Env;
    app->activity->vm->AttachCurrentThread(&Env, nullptr);

    AndroidAppState appState = {};

    app->userData = &appState;
    app->onAppCmd = app_handle_cmd;

    std::shared_ptr<Options> options = std::make_shared<Options>();
    if (!UpdateOptionsFromSystemProperties(*options)) {
        return;
    }

    bool requestRestart = false;
    bool exitRenderLoop = false;

    // Create platform abstraction
    RVRAndroidPlatform androidPlatform(app);

    // Create graphics API implementation.
    RVRVulkanRenderer vulkanRenderer(options, &androidPlatform);

    // Initialize the OpenXR program.
    RVRApp rvrApp(&androidPlatform, &vulkanRenderer);

    // Initialize the loader for this platform
    PFN_xrInitializeLoaderKHR initializeLoader = nullptr;
    if (XR_SUCCEEDED(
            xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)(&initializeLoader)))) {
        XrLoaderInitInfoAndroidKHR loaderInitInfoAndroid;
        memset(&loaderInitInfoAndroid, 0, sizeof(loaderInitInfoAndroid));
        loaderInitInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
        loaderInitInfoAndroid.next = NULL;
        loaderInitInfoAndroid.applicationVM = app->activity->vm;
        loaderInitInfoAndroid.applicationContext = app->activity->clazz;
        initializeLoader((const XrLoaderInitInfoBaseHeaderKHR*)&loaderInitInfoAndroid);
    }

    rvrApp.CreateInstance();
    rvrApp.InitializeSystem();
    rvrApp.InitializeSession();
    rvrApp.CreateSwapchains();

    RVRGameLoopTimer timer;
    while (app->destroyRequested == 0) {
        float dt = timer.RefreshDeltaTime();
        rvrApp.SetDeltaTime(dt);

        // Read all pending events.
        for (;;) {
            int events;
            struct android_poll_source* source;
            // If the timeout is zero, returns immediately without blocking.
            // If the timeout is negative, waits indefinitely until an event appears.
            const int timeoutMilliseconds =
                (!appState.Resumed && !rvrApp.IsSessionRunning() && app->destroyRequested == 0) ? -1 : 0;
            if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void**)&source) < 0) {
                break;
            }

            // Process this event.
            if (source != nullptr) {
                source->process(app, source);
            }
        }

        rvrApp.PollXrEvents(&exitRenderLoop, &requestRestart);
        if (!rvrApp.IsSessionRunning()) {
            // Throttle loop since xrWaitFrame won't be called.
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            continue;
        }

        rvrApp.PollActions();
        rvrApp.RenderFrame();
    }

    app->activity->vm->DetachCurrentThread();
}
