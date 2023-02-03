#include "xr_context.h"
#include <platform/android_context.h>

namespace rvr {
XrContext::XrContext(VulkanRenderer* vulkanRenderer) : vulkanRenderer_(vulkanRenderer) {
    Initialize();
}

XrContext::~XrContext() {
    for (Swapchain swapchain : swapchains) {
        xrDestroySwapchain(swapchain.handle);
    }

    for (auto referenceSpace : initializedRefSpaces_) {
        xrDestroySpace(referenceSpace.second);
    }

    if (appSpace != XR_NULL_HANDLE) {
        xrDestroySpace(appSpace);
    }

    if (session != XR_NULL_HANDLE) {
        xrDestroySession(session);
    }

    if (xrInstance_ != XR_NULL_HANDLE) {
        xrDestroyInstance(xrInstance_);
    }
}

void XrContext::Initialize() {
    InitializePlatformLoader();
    CreateInstance();
    InitializeSystem();
    InitializeSession();
    actionManager.CreateActionSpace(session);
    CreateSwapchains();
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
        loaderInitInfoAndroid.applicationVM = AndroidContext::Instance()->GetAndroidApp()->activity->vm;
        loaderInitInfoAndroid.applicationContext = AndroidContext::Instance()->GetAndroidApp()->activity->clazz;
        initializeLoader((const XrLoaderInitInfoBaseHeaderKHR*)&loaderInitInfoAndroid);
    }
}

void XrContext::CreateInstance() {
    CHECK(xrInstance_ == XR_NULL_HANDLE);

    // Create union of extensions required by platform and graphics plugins.
    std::vector<const char*> extensions;

    // Transform platform and graphics extension std::strings to C strings.
    const std::vector<std::string> platformExtensions = AndroidContext::GetInstanceExtensions();
    std::transform(platformExtensions.begin(), platformExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });
    const std::vector<std::string> graphicsExtensions = vulkanRenderer_->GetInstanceExtensions();
    std::transform(graphicsExtensions.begin(), graphicsExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.next = AndroidContext::Instance()->GetInstanceCreateExtension();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.enabledExtensionNames = extensions.data();

    strcpy(createInfo.applicationInfo.applicationName, "HelloXR");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    CHECK_XRCMD(xrCreateInstance(&createInfo, &xrInstance_));
}

void XrContext::InitializeSystem() {
    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(xrSystemId_ == XR_NULL_SYSTEM_ID);

    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    CHECK_XRCMD(xrGetSystem(xrInstance_, &systemInfo, &xrSystemId_));

    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(xrSystemId_ != XR_NULL_SYSTEM_ID);

    // The graphics API can initialize the graphics device
    vulkanRenderer_->InitializeDevice(xrInstance_, xrSystemId_);
}

void XrContext::InitializeSession() {
    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(session == XR_NULL_HANDLE);

    XrSessionCreateInfo createInfo{XR_TYPE_SESSION_CREATE_INFO};
    createInfo.next = vulkanRenderer_->GetGraphicsBinding();
    createInfo.systemId = xrSystemId_;
    CHECK_XRCMD(xrCreateSession(xrInstance_, &createInfo, &session));

    InitializeActions();
    InitializeReferenceSpaces();

    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(RVRReferenceSpace::TrackedOrigin);
    CHECK_XRCMD(xrCreateReferenceSpace(session, &referenceSpaceCreateInfo, &appSpace));
}

void XrContext::InitializeActions() {
    // Todo: put this inside of actionManager.Init()
    actionManager.Init(xrInstance_);
    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &actionManager.actionSet;
    CHECK_XRCMD(xrAttachSessionActionSets(session, &attachInfo));
}

void XrContext::InitializeReferenceSpaces() {
    CHECK(session != XR_NULL_HANDLE);
    RVRReferenceSpace referenceSpaces[] = {
            RVRReferenceSpace::Hud,
            RVRReferenceSpace::TrackedOrigin
    };

    for (const auto& referenceSpace : referenceSpaces) {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(referenceSpace);
        XrSpace space;
        XrResult res = xrCreateReferenceSpace(session, &referenceSpaceCreateInfo, &space);
        if (XR_SUCCEEDED(res))
            initializedRefSpaces_[referenceSpace] = space;
        else
            THROW(Fmt("Failed to create reference space %d with error %d", referenceSpace, res));

    }
}

void XrContext::CreateSwapchains() {
    CHECK(session != XR_NULL_HANDLE);
    CHECK(swapchains.empty());
    CHECK(configViews.empty());

    // Read graphics properties for preferred swapchain length and logging.
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    CHECK_XRCMD(xrGetSystemProperties(xrInstance_, xrSystemId_, &systemProperties));

    // Query and cache view configuration views.
    uint32_t viewCount;
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(xrInstance_, xrSystemId_, viewConfigType, 0, &viewCount, nullptr));
    configViews.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(xrInstance_, xrSystemId_, viewConfigType, viewCount, &viewCount,
                                                  configViews.data()));

    // Create and cache view buffer for xrLocateViews later.
    views.resize(viewCount, {XR_TYPE_VIEW});

    // Create the swapchain and get the images.
    if (viewCount > 0) {
        // Select a swapchain format.
        uint32_t swapchainFormatCount;
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session, 0, &swapchainFormatCount, nullptr));
        std::vector<int64_t> swapchainFormats(swapchainFormatCount);
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session, (uint32_t)swapchainFormats.size(), &swapchainFormatCount,
                                                swapchainFormats.data()));
        CHECK(swapchainFormatCount == swapchainFormats.size());
        colorSwapchainFormat = vulkanRenderer_->SelectColorSwapchainFormat(swapchainFormats);

        // Create a swapchain for each view.
        for (uint32_t i = 0; i < viewCount; i++) {
            const XrViewConfigurationView& vp = configViews[i];

            // Create the swapchain.
            XrSwapchainCreateInfo swapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
            swapchainCreateInfo.arraySize = 1;
            swapchainCreateInfo.format = colorSwapchainFormat;
            swapchainCreateInfo.width = vp.recommendedImageRectWidth;
            swapchainCreateInfo.height = vp.recommendedImageRectHeight;
            swapchainCreateInfo.mipCount = 1;
            swapchainCreateInfo.faceCount = 1;
            swapchainCreateInfo.sampleCount = vulkanRenderer_->GetSupportedSwapchainSampleCount(vp);
            swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            Swapchain swapchain;
            swapchain.width = (int32_t)swapchainCreateInfo.width;
            swapchain.height = (int32_t)swapchainCreateInfo.height;
            CHECK_XRCMD(xrCreateSwapchain(session, &swapchainCreateInfo, &swapchain.handle));

            swapchains.push_back(swapchain);

            uint32_t imageCount;
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, 0, &imageCount, nullptr));
            // XXX This should really just return XrSwapchainImageBaseHeader*
            std::vector<XrSwapchainImageBaseHeader*> swapchainImages =
                    vulkanRenderer_->AllocateSwapchainImageStructs(imageCount, swapchainCreateInfo);
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, imageCount, &imageCount, swapchainImages[0]));

            swapchainImageMap.insert(std::make_pair(swapchain.handle, std::move(swapchainImages)));
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
                Log::Write(Log::Level::Info, Fmt("XrEventDataInstanceLossPending by %lld", instanceLossPending.lossTime));
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
            default: {
                Log::Write(Log::Level::Info, Fmt("Ignoring event type %d", event->type));
                break;
            }
        }
    }
}

void XrContext::PollActions() {
    // Updates all actions
    actionManager.Update(session);

    // Todo: the following below can be decoupled into rituals,
    //  once that is done PollActions() can be deleted in favor of actionManager.Update()

    // Left right hand scale & vibration
    auto grab = dynamic_cast<FloatAction*>(actionManager.GetAction(ActionType::GripTrigger));
    auto vibrate = dynamic_cast<HapticAction*>(actionManager.GetAction(ActionType::Vibrate));
    for (Hand hand : grab->hands) {
        actionManager.handScale[(int)hand] = 1.0f - 0.5 * grab->GetHandState(hand).currentState;
        if (grab->GetHandState(hand).currentState > 0.9)
            vibrate->ApplyVibration(session, hand, 0.5);
    }

    // Check if pose is active
    auto pose = dynamic_cast<PoseAction*>(actionManager.GetAction(ActionType::GripPose));
    actionManager.handActive = {XR_FALSE, XR_FALSE};
    for (Hand hand : pose->hands)
        actionManager.handActive[(int)hand] = pose->isHandActive(hand);

    // Check for quitting
    auto quit = dynamic_cast<BoolAction*>(actionManager.GetAction(ActionType::Menu));
    XrActionStateBoolean quitValue = quit->CurrHandState(Hand::Left);
    if ((quitValue.isActive == XR_TRUE) && (quitValue.changedSinceLastSync == XR_TRUE) && (quitValue.currentState == XR_TRUE)) {
        CHECK_XRCMD(xrRequestExitSession(session));
    }

    // A button pressed
    auto a = dynamic_cast<BoolAction*>(actionManager.GetAction(ActionType::A));
    if (a->StateTurnedOn(Hand::Right))
        Log::Write(Log::Level::Info, "A button pressed");

    // left joystick
    auto joystick = dynamic_cast<Vec2Action*>(actionManager.GetAction(ActionType::Joystick));
    float x = joystick->GetHandState(Hand::Left).currentState.x;
    float y = joystick->GetHandState(Hand::Left).currentState.y;
    if (x != 0 && y != 0)
        Log::Write(Log::Level::Info, Fmt("left joystick = {%.2f, %.2f}", x, y));
}

void XrContext::HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent,
                                               bool* exitRenderLoop,
                                               bool* requestRestart) {
    const XrSessionState oldState = xrSessionState_;
    xrSessionState_ = stateChangedEvent.state;

    Log::Write(Log::Level::Info,
               Fmt("XrEventDataSessionStateChanged: state %s->%s session=%lld time=%lld",
                   to_string(oldState),
                   to_string(xrSessionState_),
                   stateChangedEvent.session,
                   stateChangedEvent.time));

    if ((stateChangedEvent.session != XR_NULL_HANDLE) && (stateChangedEvent.session != session)) {
        Log::Write(Log::Level::Info, "XrEventDataSessionStateChanged for unknown session");
        return;
    }

    switch (xrSessionState_) {
        case XR_SESSION_STATE_READY: {
            CHECK(session != XR_NULL_HANDLE);
            XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
            sessionBeginInfo.primaryViewConfigurationType = viewConfigType;
            CHECK_XRCMD(xrBeginSession(session, &sessionBeginInfo));
            xrSessionRunning_ = true;
            break;
        }
        case XR_SESSION_STATE_STOPPING: {
            CHECK(session != XR_NULL_HANDLE);
            xrSessionRunning_ = false;
            CHECK_XRCMD(xrEndSession(session))
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
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(session, &getInfo, 0, &pathCount, nullptr));
    std::vector<XrPath> paths(pathCount);
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(session, &getInfo, uint32_t(paths.size()), &pathCount, paths.data()));

    std::string sourceName;
    for (auto& path : paths) {
        constexpr XrInputSourceLocalizedNameFlags all = XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

        XrInputSourceLocalizedNameGetInfo nameInfo = {XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO};
        nameInfo.sourcePath = path;
        nameInfo.whichComponents = all;

        uint32_t size = 0;
        CHECK_XRCMD(xrGetInputSourceLocalizedName(session, &nameInfo, 0, &size, nullptr));
        if (size < 1) {
            continue;
        }
        std::vector<char> grabSource(size);
        CHECK_XRCMD(xrGetInputSourceLocalizedName(session, &nameInfo, uint32_t(grabSource.size()), &size, grabSource.data()));
        if (!sourceName.empty()) {
            sourceName += " and ";
        }
        sourceName += "'";
        sourceName += std::string(grabSource.data(), size - 1);
        sourceName += "'";
    }

    Log::Write(Log::Level::Info,
               Fmt("%s action is bound to %s", toString(action->type).c_str(),
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
                                    appSpace,
                                    frameState.predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.leftHand = spaceLocation;
                break;
            case TrackedSpaceLocations::RightHand:
                res = xrLocateSpace(pose->GetHandSpace(Hand::Right),
                                    appSpace,
                                    frameState.predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.rightHand = spaceLocation;
                break;
            case TrackedSpaceLocations::VrOrigin:
                res = xrLocateSpace(initializedRefSpaces_[RVRReferenceSpace::TrackedOrigin],
                                    appSpace,
                                    frameState.predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.vrOrigin = spaceLocation;
                break;
        }
    }
}

void XrContext::AddMainLayer() {
    layers_.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&mainLayer));
}

void XrContext::BeginFrame() {
    CHECK(session != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    CHECK_XRCMD(xrWaitFrame(session, &frameWaitInfo, &frameState));

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    CHECK_XRCMD(xrBeginFrame(session, &frameBeginInfo));
}

void XrContext::EndFrame() {
    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    frameEndInfo.layerCount = (uint32_t)layers_.size();
    frameEndInfo.layers = layers_.data();
    CHECK_XRCMD(xrEndFrame(session, &frameEndInfo));
    layers_.clear();
}
}