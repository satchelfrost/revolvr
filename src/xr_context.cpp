/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "xr_context.h"
#include "ecs/system/spatial_system.h"
#include <global_context.h>
#include <instance_extension_manager.h>
#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
XrContext::XrContext() {
    vulkanContext_ = GlobalContext::Inst()->GetVulkanContext();

    InitializePlatformLoader();
    CreateXrInstance();
    InitializeSystem();
    InitializeSession();
    InitializeReferenceSpaces();
    InitializeActions();
    actionManager.CreateActionSpaces(session_);
    handTrackerLeft_.Init(xrInstance_, session_, HandTracker::Hand::Left);
    handTrackerRight_.Init(xrInstance_, session_, HandTracker::Hand::Right);
    CreateSwapchains();
}

XrContext::~XrContext() {
    for (Swapchain swapchain : swapchains_)
        xrDestroySwapchain(swapchain.handle);

    for (auto referenceSpace : initializedRefSpaces_)
        xrDestroySpace(referenceSpace.second);

    if (appSpace_ != XR_NULL_HANDLE)
        xrDestroySpace(appSpace_);

    handTrackerLeft_.EndSession();
    handTrackerRight_.EndSession();
    actionManager.EndSession();

    if (session_ != XR_NULL_HANDLE)
        xrDestroySession(session_);

    if (xrInstance_ != XR_NULL_HANDLE)
        xrDestroyInstance(xrInstance_);
}

void XrContext::InitializePlatformLoader() {
    // Initialize the loader for this platform
    PFN_xrInitializeLoaderKHR initializeLoader = nullptr;
    if (XR_SUCCEEDED(
            xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)(&initializeLoader)))) {
        XrLoaderInitInfoAndroidKHR loaderInitInfoAndroid;
        memset(&loaderInitInfoAndroid, 0, sizeof(loaderInitInfoAndroid));
        loaderInitInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
        loaderInitInfoAndroid.next = nullptr;
        auto androidContext = GlobalContext::Inst()->GetAndroidContext();
        loaderInitInfoAndroid.applicationVM = androidContext->GetAndroidApp()->activity->vm;
        loaderInitInfoAndroid.applicationContext = androidContext->GetAndroidApp()->activity->clazz;
        initializeLoader((const XrLoaderInitInfoBaseHeaderKHR*)&loaderInitInfoAndroid);
    }
}

void XrContext::CreateXrInstance() {
    CHECK(xrInstance_ == XR_NULL_HANDLE);

    InstanceExtensionManager instanceExtensionManager;
    auto extensions = instanceExtensionManager.GetExtensions();

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.next = GlobalContext::Inst()->GetAndroidContext()->GetInstanceCreateExtension();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.enabledExtensionNames = extensions.data();

    strcpy(createInfo.applicationInfo.applicationName, "HelloXR");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    CHECK_XRCMD(xrCreateInstance(&createInfo, &xrInstance_));
}

void XrContext::InitializeSystem() {
    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(xrSystemId_ == XR_NULL_SYSTEM_ID);

    // Make sure the system has a head mounted display
    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    CHECK_XRCMD(xrGetSystem(xrInstance_, &systemInfo, &xrSystemId_));

    // Make sure the system has hand tracking
    XrSystemHandTrackingPropertiesEXT handTrackingSystemProperties{XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT};
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES, &handTrackingSystemProperties};
    CHECK_XRCMD(xrGetSystemProperties(xrInstance_, xrSystemId_, &systemProperties));
    if(!handTrackingSystemProperties.supportsHandTracking)
        THROW("Application requires hand tracking");

    vulkanContext_->InitDevice(xrInstance_, xrSystemId_);
}

void XrContext::InitializeSession() {
    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(session_ == XR_NULL_HANDLE);

    XrSessionCreateInfo createInfo{XR_TYPE_SESSION_CREATE_INFO};
    createInfo.next = vulkanContext_->GetGraphicsBinding();
    createInfo.systemId = xrSystemId_;
    CHECK_XRCMD(xrCreateSession(xrInstance_, &createInfo, &session_));
}

void XrContext::InitializeActions() {
    actionManager.Init(xrInstance_);
    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &actionManager.actionSet;
    CHECK_XRCMD(xrAttachSessionActionSets(session_, &attachInfo));
}

void XrContext::InitializeReferenceSpaces() {
    CHECK(session_ != XR_NULL_HANDLE);
    RVRReferenceSpace referenceSpaces[] = {
            RVRReferenceSpace::Hud,
            RVRReferenceSpace::TrackedOrigin,
            RVRReferenceSpace::Head
    };

    for (const auto& referenceSpace : referenceSpaces) {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(referenceSpace);
        XrSpace space;
        XrResult res = xrCreateReferenceSpace(session_, &referenceSpaceCreateInfo, &space);
        if (XR_SUCCEEDED(res))
            initializedRefSpaces_[referenceSpace] = space;
        else
            THROW(Fmt("Failed to create reference space %d with error %d", referenceSpace, res));

    }

    // Save the app space
    appSpace_ = initializedRefSpaces_[RVRReferenceSpace::TrackedOrigin];
}

void XrContext::CreateSwapchains() {
    CHECK(session_ != XR_NULL_HANDLE);
    CHECK(swapchains_.empty());
    CHECK(configViews_.empty());

    // Read graphics properties for preferred swapchain length and logging.
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    CHECK_XRCMD(xrGetSystemProperties(xrInstance_, xrSystemId_, &systemProperties));

    // Query and cache view configuration views.
    uint32_t viewCount;
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(xrInstance_, xrSystemId_,
                                                  viewConfigType_, 0,
                                                  &viewCount, nullptr));
    configViews_.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(xrInstance_, xrSystemId_,
                                                  viewConfigType_, viewCount,
                                                  &viewCount, configViews_.data()));

    // Create and cache view buffer for xrLocateViews later.
    views_.resize(viewCount, {XR_TYPE_VIEW});

    // Create the swapchain and get the images.
    if (viewCount > 0) {
        // Select a swapchain format.
        uint32_t swapchainFormatCount;
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session_, 0, &swapchainFormatCount,
                                                nullptr));
        std::vector<int64_t> swapchainFormats(swapchainFormatCount);
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session_, (uint32_t)swapchainFormats.size(),
                                                &swapchainFormatCount,
                                                swapchainFormats.data()));
        CHECK(swapchainFormatCount == swapchainFormats.size());
        auto colorSwapchainFormat = SelectColorSwapchainFormat(swapchainFormats);
        vulkanContext_->InitRenderingContext(static_cast<VkFormat>(colorSwapchainFormat));

        // Create a swapchain for each view.
        for (uint32_t i = 0; i < viewCount; i++) {
            const XrViewConfigurationView& vp = configViews_[i];

            // Create the swapchain.
            XrSwapchainCreateInfo swapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
            swapchainCreateInfo.arraySize = 1;
            swapchainCreateInfo.format = colorSwapchainFormat;
            swapchainCreateInfo.width = vp.recommendedImageRectWidth;
            swapchainCreateInfo.height = vp.recommendedImageRectHeight;
            swapchainCreateInfo.mipCount = 1;
            swapchainCreateInfo.faceCount = 1;
            swapchainCreateInfo.sampleCount = VulkanContext::GetSupportedSwapchainSampleCount(vp);
            swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            Swapchain swapchain{};
            swapchain.width = (int32_t)swapchainCreateInfo.width;
            swapchain.height = (int32_t)swapchainCreateInfo.height;
            CHECK_XRCMD(xrCreateSwapchain(session_, &swapchainCreateInfo, &swapchain.handle));

            swapchains_.push_back(swapchain);

            uint32_t imageCount;
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, 0,
                                                   &imageCount, nullptr));
            XrSwapchainImageBaseHeader* swapchainImages = vulkanContext_->AllocateSwapchainImageStructs(
                    imageCount, swapchainCreateInfo);
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, imageCount,
                                                   &imageCount, swapchainImages));
            vulkanContext_->SwapchainImagesReady(swapchainImages);
            swapchainImages_.insert(std::make_pair(swapchain.handle, swapchainImages));
        }
    }
}

const XrEventDataBaseHeader* XrContext::TryReadNextEvent() {
    // It is sufficient to clear the just the XrEventDataBuffer header to
    // XR_TYPE_EVENT_DATA_BUFFER
    auto* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(&xrEventDataBuffer_);
    *baseHeader = {XR_TYPE_EVENT_DATA_BUFFER};
    const XrResult xr = xrPollEvent(xrInstance_, &xrEventDataBuffer_);
    if (xr == XR_SUCCESS) {
        if (baseHeader->type == XR_TYPE_EVENT_DATA_EVENTS_LOST) {
            const auto* const eventsLost = reinterpret_cast<const XrEventDataEventsLost*>(baseHeader);
            Log::Write(Log::Level::Warning, Fmt("%d events lost", eventsLost));
        }

        return baseHeader;
    }
    if (xr == XR_EVENT_UNAVAILABLE) {
        return nullptr;
    }
    THROW_XR(xr, "xrPollEvent");
}

void XrContext::PollXrEvents(bool* exitRenderLoop, bool* requestRestart) {
    *exitRenderLoop = *requestRestart = false;

    // Process all pending messages.
    while (const XrEventDataBaseHeader* event = TryReadNextEvent()) {
        switch (event->type) {
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                const auto& instanceLossPending = *reinterpret_cast<const XrEventDataInstanceLossPending*>(event);
                PrintInfo(Fmt("XrEventDataInstanceLossPending by %lld", instanceLossPending.lossTime));
                *exitRenderLoop = true;
                *requestRestart = true;
                return;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                auto sessionStateChangedEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(event);
                HandleSessionStateChangedEvent(sessionStateChangedEvent, exitRenderLoop, requestRestart);
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                for (auto action : actionManager.GetActions())
                    LogActionSourceName(action);
                break;
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
                PrintInfo("XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING");
                break;
            default: {
                PrintInfo("Ignoring event type %d" + std::to_string(event->type));
                break;
            }
        }
    }
}

void XrContext::Update() {
    actionManager.Update(session_);
    handTrackerLeft_.Update(frameState_.predictedDisplayTime, appSpace_);
    handTrackerRight_.Update(frameState_.predictedDisplayTime, appSpace_);
}

void XrContext::HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent,
                                               bool* exitRenderLoop,
                                               bool* requestRestart) {
    const XrSessionState oldState = xrSessionState_;
    xrSessionState_ = stateChangedEvent.state;
    PrintInfo(Fmt("%s -> %s", to_string(oldState), to_string(xrSessionState_)));

    if ((stateChangedEvent.session != XR_NULL_HANDLE) && (stateChangedEvent.session != session_)) {
        PrintInfo("XrEventDataSessionStateChanged for unknown session");
        return;
    }

    switch (xrSessionState_) {
        case XR_SESSION_STATE_READY: {
            CHECK(session_ != XR_NULL_HANDLE);
            XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
            sessionBeginInfo.primaryViewConfigurationType = viewConfigType_;
            CHECK_XRCMD(xrBeginSession(session_, &sessionBeginInfo));
            xrSessionRunning_ = true;
            break;
        }
        case XR_SESSION_STATE_STOPPING: {
            CHECK(session_ != XR_NULL_HANDLE);
            xrSessionRunning_ = false;
            CHECK_XRCMD(xrEndSession(session_))
            break;
        }
        case XR_SESSION_STATE_EXITING: {
            *exitRenderLoop = true;
            // Do not attempt to restart because user closed this session.
            *requestRestart = false;
            break;
        }
        case XR_SESSION_STATE_LOSS_PENDING: {
            *exitRenderLoop = true;
            // Poll for a new instance.
            *requestRestart = true;
            break;
        }
        default:
            break;
    }
}

bool XrContext::IsSessionRunning() const {
    return xrSessionRunning_;
}

bool XrContext::IsSessionFocused() const {
    return xrSessionState_ == XR_SESSION_STATE_FOCUSED;
}

void XrContext::LogActionSourceName(Action* action) const {
    XrBoundSourcesForActionEnumerateInfo getInfo = {XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO};
    getInfo.action = action->GetAction();
    uint32_t pathCount = 0;
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(session_, &getInfo, 0, &pathCount, nullptr));
    std::vector<XrPath> paths(pathCount);
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(session_, &getInfo, uint32_t(paths.size()), &pathCount, paths.data()));

    std::string sourceName;
    for (auto& path : paths) {
        constexpr XrInputSourceLocalizedNameFlags all = XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

        XrInputSourceLocalizedNameGetInfo nameInfo = {XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO};
        nameInfo.sourcePath = path;
        nameInfo.whichComponents = all;

        uint32_t size = 0;
        CHECK_XRCMD(xrGetInputSourceLocalizedName(session_, &nameInfo, 0, &size, nullptr));
        if (size < 1) {
            continue;
        }
        std::vector<char> grabSource(size);
        CHECK_XRCMD(xrGetInputSourceLocalizedName(session_, &nameInfo, uint32_t(grabSource.size()), &size, grabSource.data()));
        if (!sourceName.empty()) {
            sourceName += " and ";
        }
        sourceName += "'";
        sourceName += std::string(grabSource.data(), size - 1);
        sourceName += "'";
    }

    PrintVerbose(Fmt("%s action is bound to %s", toString(action->type).c_str(),
                       ((!sourceName.empty()) ? sourceName.c_str() : "nothing")));
}

void XrContext::RefreshTrackedSpaceLocations() {
    auto pose = dynamic_cast<PoseAction*>(actionManager.GetAction(ActionType::GripPose));
    for (auto trackedSpaceLocation : trackedSpaceLocations.locations) {
        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        XrResult res;
        switch(trackedSpaceLocation) {
            case TrackedSpaceLocations::LeftHand:
                res = xrLocateSpace(pose->GetHandSpace(Hand::Left),
                                    appSpace_,
                                    frameState_.predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.leftHand = spaceLocation;
                break;
            case TrackedSpaceLocations::RightHand:
                res = xrLocateSpace(pose->GetHandSpace(Hand::Right),
                                    appSpace_,
                                    frameState_.predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.rightHand = spaceLocation;
                break;
            case TrackedSpaceLocations::VrOrigin:
                res = xrLocateSpace(initializedRefSpaces_[RVRReferenceSpace::TrackedOrigin],
                                    appSpace_,
                                    frameState_.predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.vrOrigin = spaceLocation;
                break;
            case TrackedSpaceLocations::Head:
                res = xrLocateSpace(initializedRefSpaces_[RVRReferenceSpace::Head],
                                    appSpace_,
                                    frameState_.predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.head = spaceLocation;
                break;
        }
    }
}

void XrContext::BeginFrame() {
    CHECK(session_ != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    CHECK_XRCMD(xrWaitFrame(session_, &frameWaitInfo, &frameState_));

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    CHECK_XRCMD(xrBeginFrame(session_, &frameBeginInfo));
}

void XrContext::EndFrame() {
    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = frameState_.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    frameEndInfo.layerCount = (uint32_t)layers_.size();
    frameEndInfo.layers = layers_.data();
    CHECK_XRCMD(xrEndFrame(session_, &frameEndInfo));
    layers_.clear();
}

void XrContext::Render() {
    if (frameState_.shouldRender == XR_TRUE) {
        if (RenderLayer(projectionLayerViews_, layer_)) {
            layers_.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer_));
        }
    }
}

bool XrContext::RenderLayer(std::vector<XrCompositionLayerProjectionView> &projectionLayerViews,
                            XrCompositionLayerProjection &layer) {
    XrViewState viewState{XR_TYPE_VIEW_STATE};
    auto viewCapacityInput = (uint32_t)views_.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = viewConfigType_;
    viewLocateInfo.displayTime = frameState_.predictedDisplayTime;
    viewLocateInfo.space = appSpace_;

    XrResult res = xrLocateViews(session_, &viewLocateInfo, &viewState, viewCapacityInput,
                                 &viewCountOutput, views_.data());
    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == configViews_.size());
    CHECK(viewCountOutput == swapchains_.size());

    projectionLayerViews.resize(viewCountOutput);

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        // Each view has a separate swapchain which is acquired, rendered to, and released.
        const Swapchain viewSwapchain = swapchains_[i];

        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

        uint32_t swapchainImageIndex;
        CHECK_XRCMD(xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex));

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        CHECK_XRCMD(xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo));

        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        projectionLayerViews[i].pose = views_[i].pose;
        projectionLayerViews[i].fov = views_[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

        const XrSwapchainImageBaseHeader* const swapchainImage = swapchainImages_[viewSwapchain.handle];
        vulkanContext_->RenderView(projectionLayerViews[i], swapchainImage, swapchainImageIndex);

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));
    }

    layer.space = appSpace_;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}

void XrContext::RequestExit() {
    XrResult result = xrRequestExitSession(session_);
    CHECK_XRCMD(result);
}
}