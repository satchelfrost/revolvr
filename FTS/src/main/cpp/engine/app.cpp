#include <app.h>
#include <ecs/ecs.h>
#include <ecs/component/types/spatial.h>
#include <ecs/system/collision_system.h>
#include <ecs/system/io_system.h>
#include <ecs/system/spatial_system.h>
#include <ecs/system/render_system.h>
#include <ecs/system/ritual_system.h>

namespace rvr {
static void app_handle_cmd(struct android_app* app, int32_t cmd) {
    AndroidContext::Instance()->HandleAndroidCmd(app, cmd);
}

App::App() : requestRestart_(false), exitRenderLoop_(false) {
}

App::~App() {
    delete vulkanRenderer_;
    delete AndroidContext::Instance();
    delete xrContext_;
}

void App::Run(struct android_app *app) {
    JNIEnv* Env;
    app->activity->vm->AttachCurrentThread(&Env, nullptr);
    app->userData = this;
    app->onAppCmd = app_handle_cmd;

    // Create android abstraction
    AndroidContext::Instance()->Init(app);

    // Create graphics API implementation.
    vulkanRenderer_ = new VulkanRenderer();

    // Create xr abstraction
    xrContext_ = XrContext::Instance();
    xrContext_->Initialize(vulkanRenderer_);

    // Initialize ECS
    ECS::Instance()->Init();

    // Load and Initialize Scene
    scene_.LoadScene("test_scene");

    GameLoopTimer timer;
    while (app->destroyRequested == 0) {
        // Update timer
        timer.RefreshDeltaTime(deltaTime_);

        // Handle Android events
        AndroidContext::Instance()->HandleEvents(xrContext_->IsSessionRunning());

        // Handle OpenXR Events
        xrContext_->PollXrEvents(&exitRenderLoop_, &requestRestart_);

        // Do not begin frame unless session is running
        if (!xrContext_->IsSessionRunning()) continue;

        // Begin frame sequence
        xrContext_->BeginFrame();
        xrContext_->UpdateActions();
        UpdateSystems();
        Render();
        xrContext_->EndFrame();
    }

    app->activity->vm->DetachCurrentThread();
}


void App::UpdateSystems() {
    // TODO: Move UpdateSpatials to the end of the function as a catch all
    system::spatial::UpdateTrackedSpaces(xrContext_);
    system::spatial::UpdateSpatials();
    system::collision::RunCollisionChecks();
    system::ritual::Update(deltaTime_);
}

void App::Render() {
    if (xrContext_->frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(xrContext_->projectionLayerViews, xrContext_->mainLayer)) {
            xrContext_->AddMainLayer();
        }
    }
}

bool App::RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                         XrCompositionLayerProjection& layer) {
    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)xrContext_->views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = xrContext_->viewConfigType;
    viewLocateInfo.displayTime = xrContext_->frameState.predictedDisplayTime;
    viewLocateInfo.space = xrContext_->appSpace;

    XrResult res = xrLocateViews(xrContext_->session, &viewLocateInfo, &viewState, viewCapacityInput,
                                 &viewCountOutput, xrContext_->views.data());
    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == xrContext_->configViews.size());
    CHECK(viewCountOutput == xrContext_->swapchains.size());

    projectionLayerViews.resize(viewCountOutput);

    // Convert renderable to a cube for now
    for (auto spatial : system::render::GetRenderSpatials()) {
        Cube cube{};
        cube.Pose = spatial->world.pose.ToXrPosef();
        cube.Scale = math::vector::ToXrVector3f(spatial->world.scale);
        renderBuffer_.push_back(cube);
    }

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        // Each view has a separate swapchain which is acquired, rendered to, and released.
        const Swapchain viewSwapchain = xrContext_->swapchains[i];

        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

        uint32_t swapchainImageIndex;
        CHECK_XRCMD(xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex));

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        CHECK_XRCMD(xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo));

        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        projectionLayerViews[i].pose = xrContext_->views[i].pose;
        projectionLayerViews[i].fov = xrContext_->views[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

        const XrSwapchainImageBaseHeader* const swapchainImage = xrContext_->swapchainImageMap[viewSwapchain.handle][swapchainImageIndex];
        vulkanRenderer_->RenderView(projectionLayerViews[i], swapchainImage, xrContext_->colorSwapchainFormat, renderBuffer_);

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));
    }

    // Clear the renderBuffer for the next frame
    renderBuffer_.clear();

    layer.space = xrContext_->appSpace;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}
}