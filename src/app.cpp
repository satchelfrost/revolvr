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

namespace rvr {

App::App() : requestRestart_(false), exitRenderLoop_(false), globalContext_(nullptr), deltaTime_(0) {
}

App::~App() {
    delete globalContext_;
}

void App::Run(struct android_app *app) {
    globalContext_ = GlobalContext::Inst();
    globalContext_->Init(app);

    // Load and Initialize Scene
//    scene_.LoadScene("test_scenes/cloning");
//    scene_.LoadScene("test_scenes/timer");
//    scene_.LoadScene("test_scenes/hand_collision");
//    scene_.LoadScene("test_scenes/sound_test");
//    scene_.LoadScene("test_scenes/spinning_pointer");
    scene_.LoadScene("test_scenes/generic");
//    scene_.LoadScene("test_scenes/conway");
//    scene_.LoadScene("test_scenes/hand_tracking");
//    scene_.LoadScene("test_scenes/movement");
//    scene_.LoadScene("test_scenes/hand_shooting_stuff");

    // Initialize resources after scene has been loaded
    globalContext_->GetVulkanContext()->InitializeResources();

    if(!globalContext_->GetAudioEngine()->start())
        THROW("Failed to start Audio engine");

    AndroidContext* androidContext = globalContext_->GetAndroidContext();
    XrContext* xrContext = globalContext_->GetXrContext();
    VulkanContext* vulkanRenderer = globalContext_->GetVulkanContext();

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
