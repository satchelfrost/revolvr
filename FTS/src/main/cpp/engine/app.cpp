#include <app.h>
#include <ecs/component/types/spatial.h>
#include <ecs/ecs.h>
#include <ecs/system/render_system.h>

namespace rvr {
static void app_handle_cmd(struct android_app* app, int32_t cmd) {
    AndroidContext::Instance()->HandleAndroidCmd(app, cmd);
}

App::App() : requestRestart_(false), exitRenderLoop_(false) {
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
    scene_.LoadScene("test_scene");

    globalContext_->GetAudioEngine()->start();

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
        Render(xrContext, vulkanRenderer);
        xrContext->EndFrame();
    }

    app->activity->vm->DetachCurrentThread();
}

void App::Render(XrContext* xrContext, VulkanRenderer* vulkanRenderer) {
    if (xrContext->frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(xrContext->projectionLayerViews, xrContext->mainLayer, xrContext,
                        vulkanRenderer)) {
            xrContext->AddMainLayer();
        }
    }
}

bool App::RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                      XrCompositionLayerProjection& layer, XrContext* xrContext,
                      VulkanRenderer* vulkanRenderer) {
    XrViewState viewState{XR_TYPE_VIEW_STATE};
    auto viewCapacityInput = (uint32_t)xrContext->views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = xrContext->viewConfigType;
    viewLocateInfo.displayTime = xrContext->frameState.predictedDisplayTime;
    viewLocateInfo.space = xrContext->appSpace;

    XrResult res = xrLocateViews(xrContext->session, &viewLocateInfo, &viewState, viewCapacityInput,
                                 &viewCountOutput, xrContext->views.data());
    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == xrContext->configViews.size());
    CHECK(viewCountOutput == xrContext->swapchains.size());

    projectionLayerViews.resize(viewCountOutput);

    // Convert renderable to a cube for now
    for (auto spatial : system::render::GetRenderSpatials()) {
        Cube cube{};
        cube.Pose = spatial->GetWorld().GetPose().ToXrPosef();
        cube.Scale = math::vector::ToXrVector3f(spatial->GetWorld().GetScale());
        renderBuffer_.push_back(cube);
    }

    // Draw a simple grid
    DrawGrid();

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        // Each view has a separate swapchain which is acquired, rendered to, and released.
        const Swapchain viewSwapchain = xrContext->swapchains[i];

        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

        uint32_t swapchainImageIndex;
        CHECK_XRCMD(xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex));

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        CHECK_XRCMD(xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo));

        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        projectionLayerViews[i].pose = xrContext->views[i].pose;
        projectionLayerViews[i].fov = xrContext->views[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

        const XrSwapchainImageBaseHeader* const swapchainImage = xrContext->swapchainImageMap[viewSwapchain.handle][swapchainImageIndex];
        vulkanRenderer->RenderView(projectionLayerViews[i], swapchainImage, xrContext->colorSwapchainFormat, renderBuffer_);

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));
    }

    // Clear the renderBuffer for the next frame
    renderBuffer_.clear();

    layer.space = xrContext->appSpace;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}

void App::DrawGrid() {
    auto player = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(0);
    // Draw the horizontal lines
    for (int i = -10; i <= 10; i ++) {
        Cube cube{};
        auto pose = math::Pose();
        pose.SetPosition(0, 0, (float)i);
        // Correct for the player position
        pose.SetPosition(pose.GetPosition() - player->GetLocal().GetPosition());
        cube.Pose = pose.ToXrPosef();
        cube.Scale = {20.0f, 0.1f, 0.1f};
        renderBuffer_.push_back(cube);
    }
    // Draw the vertical lines
    for (int i = -10; i <= 10; i ++) {
        Cube cube{};
        auto pose = math::Pose();
        pose.SetPosition((float)i, 0, 0);
        // Correct for the player position
        pose.SetPosition(pose.GetPosition() - player->GetLocal().GetPosition());
        cube.Pose = pose.ToXrPosef();
        cube.Scale = {0.1f, 0.1f, 20.0f};
        renderBuffer_.push_back(cube);
    }
}
}
