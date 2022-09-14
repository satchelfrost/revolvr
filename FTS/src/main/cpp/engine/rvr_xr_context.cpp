
#include "include/rvr_xr_context.h"

RVRXRContext::RVRXRContext(RVRAndroidContext* androidContext, RVRVulkanRenderer* vulkanRenderer)
: androidContext_(androidContext), vulkanRenderer_(vulkanRenderer) {

}

RVRXRContext::~RVRXRContext() {
    if (input.actionSet != XR_NULL_HANDLE) {
        for (auto hand : {Side::LEFT, Side::RIGHT}) {
            xrDestroySpace(input.handSpace[hand]);
        }
        xrDestroyActionSet(input.actionSet);
    }

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

void RVRXRContext::Initialize() {
    InitializePlatformLoader();
    CreateInstance();
    InitializeSystem();
    InitializeSession();
    CreateSwapchains();
}

void RVRXRContext::InitializePlatformLoader() {
    // Initialize the loader for this platform
    PFN_xrInitializeLoaderKHR initializeLoader = nullptr;
    if (XR_SUCCEEDED(
            xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)(&initializeLoader)))) {
        XrLoaderInitInfoAndroidKHR loaderInitInfoAndroid;
        memset(&loaderInitInfoAndroid, 0, sizeof(loaderInitInfoAndroid));
        loaderInitInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
        loaderInitInfoAndroid.next = nullptr;
        loaderInitInfoAndroid.applicationVM = androidContext_->GetAndroidApp()->activity->vm;
        loaderInitInfoAndroid.applicationContext = androidContext_->GetAndroidApp()->activity->clazz;
        initializeLoader((const XrLoaderInitInfoBaseHeaderKHR*)&loaderInitInfoAndroid);
    }
}

void RVRXRContext::CreateInstance() {
    CHECK(xrInstance_ == XR_NULL_HANDLE);

    // Create union of extensions required by platform and graphics plugins.
    std::vector<const char*> extensions;

    // Transform platform and graphics extension std::strings to C strings.
    const std::vector<std::string> platformExtensions = RVRAndroidContext::GetInstanceExtensions();
    std::transform(platformExtensions.begin(), platformExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });
    const std::vector<std::string> graphicsExtensions = vulkanRenderer_->GetInstanceExtensions();
    std::transform(graphicsExtensions.begin(), graphicsExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.next = androidContext_->GetInstanceCreateExtension();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.enabledExtensionNames = extensions.data();

    strcpy(createInfo.applicationInfo.applicationName, "HelloXR");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    CHECK_XRCMD(xrCreateInstance(&createInfo, &xrInstance_));
}

void RVRXRContext::InitializeSystem() {
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

void RVRXRContext::InitializeSession() {
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

void RVRXRContext::InitializeActions() {
    // Create an action set.
    {
        XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
        strcpy_s(actionSetInfo.actionSetName, "gameplay");
        strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
        actionSetInfo.priority = 0;
        CHECK_XRCMD(xrCreateActionSet(xrInstance_, &actionSetInfo, &input.actionSet));
    }

    // Get the XrPath for the left and right hands - we will use them as subaction paths.
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left", &input.handSubactionPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right", &input.handSubactionPath[Side::RIGHT]));

    // Create actions.
    {
        // Create an input action for grabbing objects with the left and right hands.
        XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "grab_object");
        strcpy_s(actionInfo.localizedActionName, "Grab Object");
        actionInfo.countSubactionPaths = uint32_t(input.handSubactionPath.size());
        actionInfo.subactionPaths = input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(input.actionSet, &actionInfo, &input.grabAction));

        // Create an input action getting the left and right hand poses.
        actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
        strcpy_s(actionInfo.actionName, "hand_pose");
        strcpy_s(actionInfo.localizedActionName, "Hand Pose");
        actionInfo.countSubactionPaths = uint32_t(input.handSubactionPath.size());
        actionInfo.subactionPaths = input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(input.actionSet, &actionInfo, &input.poseAction));

        // Create output actions for vibrating the left and right controller.
        actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
        strcpy_s(actionInfo.actionName, "vibrate_hand");
        strcpy_s(actionInfo.localizedActionName, "Vibrate Hand");
        actionInfo.countSubactionPaths = uint32_t(input.handSubactionPath.size());
        actionInfo.subactionPaths = input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(input.actionSet, &actionInfo, &input.vibrateAction));

        // Create input actions for quitting the session using the left and right controller.
        // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
        // We will just suggest bindings for both hands, where possible.
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "quit_session");
        strcpy_s(actionInfo.localizedActionName, "Quit Session");
        actionInfo.countSubactionPaths = 0;
        actionInfo.subactionPaths = nullptr;
        CHECK_XRCMD(xrCreateAction(input.actionSet, &actionInfo, &input.quitAction));
    }

    std::array<XrPath, Side::COUNT> selectPath;
    std::array<XrPath, Side::COUNT> squeezeValuePath;
    std::array<XrPath, Side::COUNT> squeezeForcePath;
    std::array<XrPath, Side::COUNT> squeezeClickPath;
    std::array<XrPath, Side::COUNT> posePath;
    std::array<XrPath, Side::COUNT> hapticPath;
    std::array<XrPath, Side::COUNT> menuClickPath;
    std::array<XrPath, Side::COUNT> bClickPath;
    std::array<XrPath, Side::COUNT> triggerValuePath;
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/select/click", &selectPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/select/click", &selectPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/squeeze/value", &squeezeValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/squeeze/value", &squeezeValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/squeeze/force", &squeezeForcePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/squeeze/force", &squeezeForcePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/squeeze/click", &squeezeClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/squeeze/click", &squeezeClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/grip/pose", &posePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/grip/pose", &posePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/output/haptic", &hapticPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/output/haptic", &hapticPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/menu/click", &menuClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/menu/click", &menuClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/b/click", &bClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/b/click", &bClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left/input/trigger/value", &triggerValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right/input/trigger/value", &triggerValuePath[Side::RIGHT]));

    // Suggest bindings for the Oculus Touch.
    {
        XrPath oculusTouchInteractionProfilePath;
        CHECK_XRCMD(
                xrStringToPath(xrInstance_, "/interaction_profiles/oculus/touch_controller", &oculusTouchInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{{{input.grabAction, squeezeValuePath[Side::LEFT]},
                                                        {input.grabAction, squeezeValuePath[Side::RIGHT]},
                                                        {input.poseAction, posePath[Side::LEFT]},
                                                        {input.poseAction, posePath[Side::RIGHT]},
                                                        {input.quitAction, menuClickPath[Side::LEFT]},
                                                        {input.vibrateAction, hapticPath[Side::LEFT]},
                                                        {input.vibrateAction, hapticPath[Side::RIGHT]}}};
        XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
        suggestedBindings.interactionProfile = oculusTouchInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(xrInstance_, &suggestedBindings));
    }

    XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo.action = input.poseAction;
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = input.handSubactionPath[Side::LEFT];
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &input.handSpace[Side::LEFT]));
    actionSpaceInfo.subactionPath = input.handSubactionPath[Side::RIGHT];
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &input.handSpace[Side::RIGHT]));

    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &input.actionSet;
    CHECK_XRCMD(xrAttachSessionActionSets(session, &attachInfo));
}

void RVRXRContext::InitializeReferenceSpaces() {
    CHECK(session != XR_NULL_HANDLE);
    RVRReferenceSpace referenceSpaces[] = { //RVRReferenceSpace::RVRHead,
            RVRReferenceSpace::Hud,
            //RVRReferenceSpace::RVRHeadInitial,
            RVRReferenceSpace::TrackedOrigin
            //RVRReferenceSpace::RVRStageRight,
            //RVRReferenceSpace::RVRStageLeft
    };

    for (const auto& referenceSpace : referenceSpaces) {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(referenceSpace);
        XrSpace space;
        XrResult res = xrCreateReferenceSpace(session, &referenceSpaceCreateInfo, &space);
        if (XR_SUCCEEDED(res))
            initializedRefSpaces_[referenceSpace] = space;
        else
            Log::Write(Log::Level::Error,
                       Fmt("Failed to create reference space %d with error %d", referenceSpace, res));
    }
}

void RVRXRContext::CreateSwapchains() {
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

const XrEventDataBaseHeader* RVRXRContext::TryReadNextEvent() {
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

void RVRXRContext::PollXrEvents(bool* exitRenderLoop, bool* requestRestart) {
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
                LogActionSourceName(input.grabAction, "Grab");
                LogActionSourceName(input.quitAction, "Quit");
                LogActionSourceName(input.poseAction, "Pose");
                LogActionSourceName(input.vibrateAction, "Vibrate");
                break;
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
            default: {
                Log::Write(Log::Level::Info, Fmt("Ignoring event type %d", event->type));
                break;
            }
        }
    }
}

void RVRXRContext::PollActions() {
    input.handActive = {XR_FALSE, XR_FALSE};

    // Sync actions
    const XrActiveActionSet activeActionSet{input.actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    CHECK_XRCMD(xrSyncActions(session, &syncInfo));

    // Get pose and grab action state and start haptic vibrate when hand is 90% squeezed.
    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.action = input.grabAction;
        getInfo.subactionPath = input.handSubactionPath[hand];

        XrActionStateFloat grabValue{XR_TYPE_ACTION_STATE_FLOAT};
        CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &grabValue));
        if (grabValue.isActive == XR_TRUE) {
            // Scale the rendered hand by 1.0f (open) to 0.5f (fully squeezed).
            input.handScale[hand] = 1.0f - 0.5f * grabValue.currentState;
            if (grabValue.currentState > 0.9f) {
                XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
                vibration.amplitude = 0.5;
                vibration.duration = XR_MIN_HAPTIC_DURATION;
                vibration.frequency = XR_FREQUENCY_UNSPECIFIED;

                XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
                hapticActionInfo.action = input.vibrateAction;
                hapticActionInfo.subactionPath = input.handSubactionPath[hand];
                CHECK_XRCMD(xrApplyHapticFeedback(session, &hapticActionInfo, (XrHapticBaseHeader*)&vibration));
            }
        }

        getInfo.action = input.poseAction;
        XrActionStatePose poseState{XR_TYPE_ACTION_STATE_POSE};
        CHECK_XRCMD(xrGetActionStatePose(session, &getInfo, &poseState));
        input.handActive[hand] = poseState.isActive;
    }

    // There were no subaction paths specified for the quit action, because we don't care which hand did it.
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO, nullptr, input.quitAction, XR_NULL_PATH};
    XrActionStateBoolean quitValue{XR_TYPE_ACTION_STATE_BOOLEAN};
    CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &quitValue));
    if ((quitValue.isActive == XR_TRUE) && (quitValue.changedSinceLastSync == XR_TRUE) && (quitValue.currentState == XR_TRUE)) {
        CHECK_XRCMD(xrRequestExitSession(session));
    }
}

void RVRXRContext::HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent,
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

bool RVRXRContext::IsSessionRunning() const {
    return xrSessionRunning_;
}

bool RVRXRContext::IsSessionFocused() const {
    return xrSessionState_ == XR_SESSION_STATE_FOCUSED;
}

void RVRXRContext::LogActionSourceName(XrAction action, const std::string& actionName) const {
    XrBoundSourcesForActionEnumerateInfo getInfo = {XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO};
    getInfo.action = action;
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
               Fmt("%s action is bound to %s", actionName.c_str(), ((!sourceName.empty()) ? sourceName.c_str() : "nothing")));
}

void RVRXRContext::RefreshTrackedSpaceLocations() {
    for (auto trackedSpaceLocation : trackedSpaceLocations.locations) {
        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        XrResult res;
        switch(trackedSpaceLocation) {
            case TrackedSpaceLocations::LeftHand:
                res = xrLocateSpace(input.handSpace[Side::LEFT],
                                    appSpace, predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.leftHand = spaceLocation;
                break;
            case TrackedSpaceLocations::RightHand:
                res = xrLocateSpace(input.handSpace[Side::RIGHT],
                                    appSpace, predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.rightHand = spaceLocation;
                break;
            case TrackedSpaceLocations::VrOrigin:
                res = xrLocateSpace(initializedRefSpaces_[RVRReferenceSpace::TrackedOrigin],
                                    appSpace, predictedDisplayTime,
                                    &spaceLocation);
                if (TrackedSpaceLocations::ValidityCheck(res, spaceLocation))
                    trackedSpaceLocations.vrOrigin = spaceLocation;
                break;
        }
    }
}
