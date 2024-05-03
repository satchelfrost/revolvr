/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <app.h>
#include <ecs/component/types/spatial.h>
#include <ecs/ecs.h>
#include <game_loop_timer.h>
#include <platform/android_context.h>
#include <rvr_reference_space.h>
#include <xr_app_helpers.h>
#include <xr_context.h>

#include <utility>

namespace rvr {

App::App(std::vector<RVRExtensions> exts) : requestRestart_(false), exitRenderLoop_(false), globalContext_(nullptr),
deltaTime_(0), exts_(std::move(exts)) {}

App::~App() {
    delete globalContext_;
}

void App::Run(struct android_app *app, const std::string& defaultScene) {
    globalContext_ = GlobalContext::Inst();
    globalContext_->Init(app, exts_);

    // Load and Initialize Scene
    scene_.LoadScene(defaultScene);

    // Initialize resources after scene has been loaded
    globalContext_->GetVulkanContext()->InitializeResources();

    if(!globalContext_->GetAudioEngine()->start())
        THROW("Failed to start Audio engine");

    AndroidContext* androidContext = globalContext_->GetAndroidContext();
    XrContext* xrContext = globalContext_->GetXrContext();

    GameLoopTimer timer;
    while (app->destroyRequested == 0) {
        // Update timer
        timer.RefreshDeltaTime(deltaTime_);

        // Handle Android events
        androidContext->HandleEvents(xrContext->IsSessionRunning());

        // Handle OpenXR Events
        xrContext->PollXrEvents(&exitRenderLoop_, &requestRestart_);
        if (exitRenderLoop_) {
            PrintInfo(("Exit render loop requested"));
            ANativeActivity_finish(app->activity);
            continue;
        }

        if (requestRestart_) {
            PrintWarning("Requested restart");
        }

        // Do not begin frame unless session is running
        if (!xrContext->IsSessionRunning()) {
            PrintVerbose("XrSession not running yet, sleep 250 ms");
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            continue;
        }

        xrContext->BeginFrame();
        xrContext->Update();
        globalContext_->UpdateSystems(deltaTime_);
        xrContext->Render();
        xrContext->EndFrame();
    }
}
}
