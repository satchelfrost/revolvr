#include "rvr_app.h"
#include "ecs/system/spatial_system.h"
#include "ecs/system/render_system.h"
#include "ecs/component/types/spatial.h"
#include "ecs/ecs.h"
#include <ecs/system/ritual_system.h>
#include <ecs/system/collision_system.h>

Cube MakeCube(float scale, XrVector3f position) {
    Cube cube{};
    cube.Scale = {scale, scale, scale};
    cube.Pose = RVRMath::Pose::Identity();
    cube.Pose.position = position;
    return cube;
}

static void app_handle_cmd(struct android_app* app, int32_t cmd) {
    auto* rvrApp = (RVRApp*)app->userData;
    rvrApp->HandleAndroidCmd(app, cmd);
}

RVRApp::RVRApp() : shouldCallBegin_(true) {

}

 RVRApp::~RVRApp() {
     delete xrContext_;
     delete vulkanRenderer_;
     delete androidContext_;
 }

void RVRApp::HandleAndroidCmd(struct android_app* app, int32_t cmd) {
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
            androidContext_->resumed = true;
            break;
        }
        case APP_CMD_PAUSE: {
            Log::Write(Log::Level::Info, "onPause()");
            Log::Write(Log::Level::Info, "    APP_CMD_PAUSE");
            androidContext_->resumed = false;
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
            androidContext_->nativeWindow = nullptr;
            break;
        }
        case APP_CMD_INIT_WINDOW: {
            Log::Write(Log::Level::Info, "surfaceCreated()");
            Log::Write(Log::Level::Info, "    APP_CMD_INIT_WINDOW");
            androidContext_->nativeWindow = app->window;
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            Log::Write(Log::Level::Info, "surfaceDestroyed()");
            Log::Write(Log::Level::Info, "    APP_CMD_TERM_WINDOW");
            androidContext_->nativeWindow = nullptr;
            break;
        }
        default:
            break;
    }
}

void RVRApp::Run(struct android_app *app) {
     JNIEnv* Env;
     app->activity->vm->AttachCurrentThread(&Env, nullptr);
     app->userData = this;
     app->onAppCmd = app_handle_cmd;

     bool requestRestart = false;
     bool exitRenderLoop = false;

     // Create platform abstraction
     androidContext_ = RVRAndroidContext::GetInstance();
     androidContext_->Init(app);

     // Create graphics API implementation.
     vulkanRenderer_ = new RVRVulkanRenderer(androidContext_);

     // Create xr abstraction
     xrContext_ = new RVRXRContext(androidContext_, vulkanRenderer_);
     xrContext_->Initialize();

     // Initialize ECS
     rvr::ECS::Instance()->Init();

     // Load and Initialize Scene
     scene_.LoadScene("test_scene");

     OnInitialize();

     RVRGameLoopTimer timer;
     while (app->destroyRequested == 0) {
         float dt = timer.RefreshDeltaTime();
         SetDeltaTime(dt);

         androidContext_->HandleEvents(xrContext_->IsSessionRunning());

         xrContext_->PollXrEvents(&exitRenderLoop, &requestRestart);
         if (!xrContext_->IsSessionRunning()) {
             // Throttle loop since xrWaitFrame won't be called.
             std::this_thread::sleep_for(std::chrono::milliseconds(250));
             continue;
         }
         Input();
//         Update();
         Render();
     }

     OnShutdown();
     app->activity->vm->DetachCurrentThread();
}

void RVRApp::Input() {
    xrContext_->PollActions();
    OnInput();
}

void RVRApp::Update() {
    // Refresh tracked spaces and then update scene tree
    xrContext_->RefreshTrackedSpaceLocations();
    rvr::SpatialSystem::UpdateTrackedSpaces(xrContext_->trackedSpaceLocations);
    rvr::SpatialSystem::UpdateSpatials();
    rvr::CollisionSystem::RunCollisionChecks();

    // We should call begin whenever new scenes are loaded
    if (shouldCallBegin_) {
        rvr::RitualSystem::Begin();
        shouldCallBegin_ = false;
    }
    rvr::RitualSystem::Update(deltaTime_);

    OnUpdate();
}

void RVRApp::Render() {
    CHECK(xrContext_->session != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    CHECK_XRCMD(xrWaitFrame(xrContext_->session, &frameWaitInfo, &frameState));

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    CHECK_XRCMD(xrBeginFrame(xrContext_->session, &frameBeginInfo));

    // Save the predicted display time for various xrLocateSpace calls
    xrContext_->predictedDisplayTime = frameState.predictedDisplayTime;

    // TODO: Look into decoupling Update from predictedDisplayTime.
    Update();

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    if (frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(projectionLayerViews, layer)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = xrContext_->predictedDisplayTime;
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    frameEndInfo.layerCount = (uint32_t)layers.size();
    frameEndInfo.layers = layers.data();
    CHECK_XRCMD(xrEndFrame(xrContext_->session, &frameEndInfo));
}

bool RVRApp::RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                         XrCompositionLayerProjection& layer) {
    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)xrContext_->views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = xrContext_->viewConfigType;
    viewLocateInfo.displayTime = xrContext_->predictedDisplayTime;
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

    // TODO: The actual rendering code needs to be refactored.
    //       OnRender() is called here as a stopgap solution.
    OnRender();

    // Convert renderable to a cube for now
    for (auto spatial : rvr::RenderSystem::GetRenderSpatials()) {
        Cube cube{};
        cube.Pose = spatial->worldPose;
        cube.Scale = spatial->scale;
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

