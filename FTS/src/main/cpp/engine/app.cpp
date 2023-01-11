#include <rvr_app.h>
#include <ecs/system/spatial_system.h>
#include <ecs/system/render_system.h>
#include <ecs/component/types/spatial.h>
#include <ecs/ecs.h>
#include <ecs/system/collision_system.h>
#include <ecs/system/ritual_system.h>

namespace rvr {
static void app_handle_cmd(struct android_app* app, int32_t cmd) {
    RVRAndroidContext::Instance()->HandleAndroidCmd(app, cmd);
}

RVRApp::RVRApp() {
}

 RVRApp::~RVRApp() {
     delete xrContext_;
     delete vulkanRenderer_;
     delete RVRAndroidContext::Instance();
 }

void RVRApp::Run(struct android_app *app) {
     JNIEnv* Env;
     app->activity->vm->AttachCurrentThread(&Env, nullptr);
     app->userData = this;
     app->onAppCmd = app_handle_cmd;

     bool requestRestart = false;
     bool exitRenderLoop = false;

     // Create platform abstraction
     RVRAndroidContext::Instance()->Init(app);

     // Create graphics API implementation.
     vulkanRenderer_ = new RVRVulkanRenderer();

     // Create xr abstraction
     xrContext_ = new RVRXRContext(RVRAndroidContext::Instance(), vulkanRenderer_);

     // Initialize ECS
     rvr::ECS::Instance()->Init();

     // Load and Initialize Scene
     scene_.LoadScene("test_scene");

     RVRGameLoopTimer timer;
     while (app->destroyRequested == 0) {
         // Update timer
         timer.RefreshDeltaTime(deltaTime_);

         // Handle Android events
         RVRAndroidContext::Instance()->HandleEvents(xrContext_->IsSessionRunning());

         // Handle OpenXR Events
         xrContext_->PollXrEvents(&exitRenderLoop, &requestRestart);

         // Do not begin frame unless session is running
         if (!xrContext_->IsSessionRunning()) continue;

         // Begin frame sequence
         xrContext_->BeginFrame();
         xrContext_->PollActions();
         UpdateSystems();
         Render();
         xrContext_->EndFrame();
     }

     app->activity->vm->DetachCurrentThread();
}


void RVRApp::UpdateSystems() {
    rvr::SpatialSystem::UpdateTrackedSpaces(xrContext_);
    rvr::SpatialSystem::UpdateSpatials();
    rvr::CollisionSystem::RunCollisionChecks();
    rvr::RitualSystem::Update(deltaTime_);
}

void RVRApp::Render() {
    if (xrContext_->frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(xrContext_->projectionLayerViews, xrContext_->mainLayer)) {
            xrContext_->AddMainLayer();
        }
    }
}

bool RVRApp::RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
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
    for (auto spatial : rvr::RenderSystem::GetRenderSpatials()) {
        Cube cube{};
        cube.Pose = spatial->worldPose;
        cube.Scale = spatial->scale;

        if (spatial->id == 1) // left hand from scene description
            cube.Scale = {cube.Scale.x * xrContext_->input.handScale[Side::LEFT],
                          cube.Scale.y * xrContext_->input.handScale[Side::LEFT],
                          cube.Scale.z * xrContext_->input.handScale[Side::LEFT]};

        if (spatial->id == 2) // left hand from scene description
            cube.Scale = {cube.Scale.x * xrContext_->input.handScale[Side::RIGHT],
                          cube.Scale.y * xrContext_->input.handScale[Side::RIGHT],
                          cube.Scale.z * xrContext_->input.handScale[Side::RIGHT]};

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