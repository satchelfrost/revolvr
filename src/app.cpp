#include <app.h>
#include <ecs/component/types/spatial.h>
#include <ecs/ecs.h>
#include <game_loop_timer.h>
#include <platform/android_context.h>
#include <rvr_reference_space.h>
#include <xr_app_helpers.h>
#include <xr_context.h>

namespace rvr {
static void app_handle_cmd(struct android_app* app, int32_t cmd) {
    AndroidContext::Instance()->HandleAndroidCmd(app, cmd);
}

App::App() : requestRestart_(false), exitRenderLoop_(false), globalContext_(nullptr), deltaTime_(0) {
}

App::~App() {
    delete globalContext_;
}

void App::Run(struct android_app *app) {
    JNIEnv* Env;
    app->activity->vm->AttachCurrentThread(&Env, nullptr);
    app->userData = this;
    app->onAppCmd = app_handle_cmd;

    globalContext_ = GlobalContext::Inst();
    globalContext_->Init(app);

    // Load and Initialize Scene
    // TODO: scene switcher, for now we are hard coding
//    scene_.LoadScene("test_scenes/cloning");
//    scene_.LoadScene("test_scenes/timer");
//    scene_.LoadScene("test_scenes/hand_collision");
//    scene_.LoadScene("test_scenes/sound_test");
//    scene_.LoadScene("test_scenes/spinning_pointer");
//    scene_.LoadScene("test_scenes/generic");
    scene_.LoadScene("test_scenes/conway");

    globalContext_->BeginSystems();

    AndroidContext* androidContext = globalContext_->GetAndroidContext();
    XrContext* xrContext = globalContext_->GetXrContext();
    VulkanRenderer* vulkanRenderer = globalContext_->GetVulkanRenderer();

    GameLoopTimer timer;
    while (app->destroyRequested == 0) {
        // Update timer
        timer.RefreshDeltaTime(deltaTime_);

        // Handle Android events
        androidContext->HandleEvents(xrContext->IsSessionRunning());

        // Handle OpenXR Events
        xrContext->PollXrEvents(&exitRenderLoop_, &requestRestart_);

        // Do not begin frame unless session is running
        if (!xrContext->IsSessionRunning()) continue;

        // Begin frame sequence
        xrContext->BeginFrame();
        xrContext->UpdateActions();
        globalContext_->UpdateSystems(deltaTime_);
        vulkanRenderer->Render();
        xrContext->EndFrame();
    }

    app->activity->vm->DetachCurrentThread();
}
}
