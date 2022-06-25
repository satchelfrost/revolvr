#include "include/rvr_app.h"
#include "include/xr_app_helpers.h"


RVRApp::RVRApp(RVRAndroidPlatform* androidPlatform, RVRVulkanRenderer* vulkanRenderer) :
    androidPlatform_(androidPlatform),
    vulkanRenderer_(vulkanRenderer) {}

 RVRApp::~RVRApp() {
     if (input_.actionSet != XR_NULL_HANDLE) {
         for (auto hand : {Side::LEFT, Side::RIGHT}) {
             xrDestroySpace(input_.handSpace[hand]);
         }
         xrDestroyActionSet(input_.actionSet);
     }

     for (Swapchain swapchain : swapchains_) {
         xrDestroySwapchain(swapchain.handle);
     }

     for (auto referenceSpace : referenceSpaces_) {
         xrDestroySpace(referenceSpace.second);
     }

     if (appSpace_ != XR_NULL_HANDLE) {
         xrDestroySpace(appSpace_);
     }

     if (xrSession_ != XR_NULL_HANDLE) {
         xrDestroySession(xrSession_);
     }

     if (xrInstance_ != XR_NULL_HANDLE) {
         xrDestroyInstance(xrInstance_);
     }
 }

void RVRApp::CreateInstance() {
    CHECK(xrInstance_ == XR_NULL_HANDLE);

    // Create union of extensions required by platform and graphics plugins.
    std::vector<const char*> extensions;

    // Transform platform and graphics extension std::strings to C strings.
    const std::vector<std::string> platformExtensions = androidPlatform_->GetInstanceExtensions();
    std::transform(platformExtensions.begin(), platformExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });
    const std::vector<std::string> graphicsExtensions = vulkanRenderer_->GetInstanceExtensions();
    std::transform(graphicsExtensions.begin(), graphicsExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.next = androidPlatform_->GetInstanceCreateExtension();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.enabledExtensionNames = extensions.data();

    strcpy(createInfo.applicationInfo.applicationName, "HelloXR");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    CHECK_XRCMD(xrCreateInstance(&createInfo, &xrInstance_));

}


void RVRApp::InitializeSystem() {
    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(xrSystemId_ == XR_NULL_SYSTEM_ID);

    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = formFactor_;
    CHECK_XRCMD(xrGetSystem(xrInstance_, &systemInfo, &xrSystemId_));

    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(xrSystemId_ != XR_NULL_SYSTEM_ID);

    // The graphics API can initialize the graphics device
    vulkanRenderer_->InitializeDevice(xrInstance_, xrSystemId_);
}

void RVRApp::InitializeActions() {
    // Create an action set.
    {
        XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
        strcpy_s(actionSetInfo.actionSetName, "gameplay");
        strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
        actionSetInfo.priority = 0;
        CHECK_XRCMD(xrCreateActionSet(xrInstance_, &actionSetInfo, &input_.actionSet));
    }

    // Get the XrPath for the left and right hands - we will use them as subaction paths.
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/left", &input_.handSubactionPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(xrInstance_, "/user/hand/right", &input_.handSubactionPath[Side::RIGHT]));

    // Create actions.
    {
        // Create an input action for grabbing objects with the left and right hands.
        XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "grab_object");
        strcpy_s(actionInfo.localizedActionName, "Grab Object");
        actionInfo.countSubactionPaths = uint32_t(input_.handSubactionPath.size());
        actionInfo.subactionPaths = input_.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(input_.actionSet, &actionInfo, &input_.grabAction));

        // Create an input action getting the left and right hand poses.
        actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
        strcpy_s(actionInfo.actionName, "hand_pose");
        strcpy_s(actionInfo.localizedActionName, "Hand Pose");
        actionInfo.countSubactionPaths = uint32_t(input_.handSubactionPath.size());
        actionInfo.subactionPaths = input_.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(input_.actionSet, &actionInfo, &input_.poseAction));

        // Create output actions for vibrating the left and right controller.
        actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
        strcpy_s(actionInfo.actionName, "vibrate_hand");
        strcpy_s(actionInfo.localizedActionName, "Vibrate Hand");
        actionInfo.countSubactionPaths = uint32_t(input_.handSubactionPath.size());
        actionInfo.subactionPaths = input_.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(input_.actionSet, &actionInfo, &input_.vibrateAction));

        // Create input actions for quitting the session using the left and right controller.
        // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
        // We will just suggest bindings for both hands, where possible.
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "quit_session");
        strcpy_s(actionInfo.localizedActionName, "Quit Session");
        actionInfo.countSubactionPaths = 0;
        actionInfo.subactionPaths = nullptr;
        CHECK_XRCMD(xrCreateAction(input_.actionSet, &actionInfo, &input_.quitAction));
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
        std::vector<XrActionSuggestedBinding> bindings{{{input_.grabAction, squeezeValuePath[Side::LEFT]},
                                                        {input_.grabAction, squeezeValuePath[Side::RIGHT]},
                                                        {input_.poseAction, posePath[Side::LEFT]},
                                                        {input_.poseAction, posePath[Side::RIGHT]},
                                                        {input_.quitAction, menuClickPath[Side::LEFT]},
                                                        {input_.vibrateAction, hapticPath[Side::LEFT]},
                                                        {input_.vibrateAction, hapticPath[Side::RIGHT]}}};
        XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
        suggestedBindings.interactionProfile = oculusTouchInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(xrInstance_, &suggestedBindings));
    }

    XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo.action = input_.poseAction;
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = input_.handSubactionPath[Side::LEFT];
    CHECK_XRCMD(xrCreateActionSpace(xrSession_, &actionSpaceInfo, &input_.handSpace[Side::LEFT]));
    actionSpaceInfo.subactionPath = input_.handSubactionPath[Side::RIGHT];
    CHECK_XRCMD(xrCreateActionSpace(xrSession_, &actionSpaceInfo, &input_.handSpace[Side::RIGHT]));

    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &input_.actionSet;
    CHECK_XRCMD(xrAttachSessionActionSets(xrSession_, &attachInfo));
}

void RVRApp::CreateVisualizedSpaces() {
    CHECK(xrSession_ != XR_NULL_HANDLE);
    RVRReferenceSpace referenceSpaces[] = { //RVRReferenceSpace::RVRHead,
                                            RVRReferenceSpace::RVRHud
                                            //RVRReferenceSpace::RVRHeadInitial,
                                            //RVRReferenceSpace::RVRTrackedOrigin,
                                            //RVRReferenceSpace::RVRStageRight,
                                            //RVRReferenceSpace::RVRStageLeft
                                            };

    for (const auto& referenceSpace : referenceSpaces) {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(referenceSpace);
        XrSpace space;
        XrResult res = xrCreateReferenceSpace(xrSession_, &referenceSpaceCreateInfo, &space);
        if (XR_SUCCEEDED(res))
            referenceSpaces_[referenceSpace] = space;
        else
            Log::Write(Log::Level::Info,
                       Fmt("Failed to create reference space %d with error %d", referenceSpace, res));
    }
}

void RVRApp::InitializeSession() {
    CHECK(xrInstance_ != XR_NULL_HANDLE);
    CHECK(xrSession_ == XR_NULL_HANDLE);

    XrSessionCreateInfo createInfo{XR_TYPE_SESSION_CREATE_INFO};
    createInfo.next = vulkanRenderer_->GetGraphicsBinding();
    createInfo.systemId = xrSystemId_;
    CHECK_XRCMD(xrCreateSession(xrInstance_, &createInfo, &xrSession_));

    InitializeActions();
    CreateVisualizedSpaces();

    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(RVRReferenceSpace::RVRHeadInitial);
    CHECK_XRCMD(xrCreateReferenceSpace(xrSession_, &referenceSpaceCreateInfo, &appSpace_));
}

void RVRApp::CreateSwapchains() {
    CHECK(xrSession_ != XR_NULL_HANDLE);
    CHECK(swapchains_.empty());
    CHECK(xrConfigViews_.empty());

    // Read graphics properties for preferred swapchain length and logging.
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    CHECK_XRCMD(xrGetSystemProperties(xrInstance_, xrSystemId_, &systemProperties));

    // Query and cache view configuration views.
    uint32_t viewCount;
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(xrInstance_, xrSystemId_, xrViewConfigType_, 0, &viewCount, nullptr));
    xrConfigViews_.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(xrInstance_, xrSystemId_, xrViewConfigType_, viewCount, &viewCount,
                                                  xrConfigViews_.data()));

    // Create and cache view buffer for xrLocateViews later.
    xrViews_.resize(viewCount, {XR_TYPE_VIEW});

    // Create the swapchain and get the images.
    if (viewCount > 0) {
        // Select a swapchain format.
        uint32_t swapchainFormatCount;
        CHECK_XRCMD(xrEnumerateSwapchainFormats(xrSession_, 0, &swapchainFormatCount, nullptr));
        std::vector<int64_t> swapchainFormats(swapchainFormatCount);
        CHECK_XRCMD(xrEnumerateSwapchainFormats(xrSession_, (uint32_t)swapchainFormats.size(), &swapchainFormatCount,
                                                swapchainFormats.data()));
        CHECK(swapchainFormatCount == swapchainFormats.size());
        xrColorSwapchainFormat_ = vulkanRenderer_->SelectColorSwapchainFormat(swapchainFormats);

        // Create a swapchain for each view.
        for (uint32_t i = 0; i < viewCount; i++) {
            const XrViewConfigurationView& vp = xrConfigViews_[i];

            // Create the swapchain.
            XrSwapchainCreateInfo swapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
            swapchainCreateInfo.arraySize = 1;
            swapchainCreateInfo.format = xrColorSwapchainFormat_;
            swapchainCreateInfo.width = vp.recommendedImageRectWidth;
            swapchainCreateInfo.height = vp.recommendedImageRectHeight;
            swapchainCreateInfo.mipCount = 1;
            swapchainCreateInfo.faceCount = 1;
            swapchainCreateInfo.sampleCount = vulkanRenderer_->GetSupportedSwapchainSampleCount(vp);
            swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            Swapchain swapchain;
            swapchain.width = swapchainCreateInfo.width;
            swapchain.height = swapchainCreateInfo.height;
            CHECK_XRCMD(xrCreateSwapchain(xrSession_, &swapchainCreateInfo, &swapchain.handle));

            swapchains_.push_back(swapchain);

            uint32_t imageCount;
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, 0, &imageCount, nullptr));
            // XXX This should really just return XrSwapchainImageBaseHeader*
            std::vector<XrSwapchainImageBaseHeader*> swapchainImages =
                vulkanRenderer_->AllocateSwapchainImageStructs(imageCount, swapchainCreateInfo);
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, imageCount, &imageCount, swapchainImages[0]));

            xrSwapchainImages_.insert(std::make_pair(swapchain.handle, std::move(swapchainImages)));
        }
    }
}

const XrEventDataBaseHeader* RVRApp::TryReadNextEvent() {
    // It is sufficient to clear the just the XrEventDataBuffer header to
    // XR_TYPE_EVENT_DATA_BUFFER
    XrEventDataBaseHeader* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(&xrEventDataBuffer_);
    *baseHeader = {XR_TYPE_EVENT_DATA_BUFFER};
    const XrResult xr = xrPollEvent(xrInstance_, &xrEventDataBuffer_);
    if (xr == XR_SUCCESS) {
        if (baseHeader->type == XR_TYPE_EVENT_DATA_EVENTS_LOST) {
            const XrEventDataEventsLost* const eventsLost = reinterpret_cast<const XrEventDataEventsLost*>(baseHeader);
            Log::Write(Log::Level::Info, Fmt("%d events lost", eventsLost));
        }

        return baseHeader;
    }
    if (xr == XR_EVENT_UNAVAILABLE) {
        return nullptr;
    }
    THROW_XR(xr, "xrPollEvent");
}

void RVRApp::PollXrEvents(bool* exitRenderLoop, bool* requestRestart) {
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
                LogActionSourceName(input_.grabAction, "Grab");
                LogActionSourceName(input_.quitAction, "Quit");
                LogActionSourceName(input_.poseAction, "Pose");
                LogActionSourceName(input_.vibrateAction, "Vibrate");
                break;
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
            default: {
                Log::Write(Log::Level::Info, Fmt("Ignoring event type %d", event->type));
                break;
            }
        }
    }
}

void RVRApp::HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent,
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

    if ((stateChangedEvent.session != XR_NULL_HANDLE) && (stateChangedEvent.session != xrSession_)) {
        Log::Write(Log::Level::Error, "XrEventDataSessionStateChanged for unknown session");
        return;
    }

    switch (xrSessionState_) {
        case XR_SESSION_STATE_READY: {
            CHECK(xrSession_ != XR_NULL_HANDLE);
            XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
            sessionBeginInfo.primaryViewConfigurationType = xrViewConfigType_;
            CHECK_XRCMD(xrBeginSession(xrSession_, &sessionBeginInfo));
            xrSessionRunning_ = true;
            break;
        }
        case XR_SESSION_STATE_STOPPING: {
            CHECK(xrSession_ != XR_NULL_HANDLE);
            xrSessionRunning_ = false;
            CHECK_XRCMD(xrEndSession(xrSession_))
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

void RVRApp::LogActionSourceName(XrAction action, const std::string& actionName) const {
    XrBoundSourcesForActionEnumerateInfo getInfo = {XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO};
    getInfo.action = action;
    uint32_t pathCount = 0;
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(xrSession_, &getInfo, 0, &pathCount, nullptr));
    std::vector<XrPath> paths(pathCount);
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(xrSession_, &getInfo, uint32_t(paths.size()), &pathCount, paths.data()));

    std::string sourceName;
    for (auto& path : paths) {
        constexpr XrInputSourceLocalizedNameFlags all = XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

        XrInputSourceLocalizedNameGetInfo nameInfo = {XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO};
        nameInfo.sourcePath = path;
        nameInfo.whichComponents = all;

        uint32_t size = 0;
        CHECK_XRCMD(xrGetInputSourceLocalizedName(xrSession_, &nameInfo, 0, &size, nullptr));
        if (size < 1) {
            continue;
        }
        std::vector<char> grabSource(size);
        CHECK_XRCMD(xrGetInputSourceLocalizedName(xrSession_, &nameInfo, uint32_t(grabSource.size()), &size, grabSource.data()));
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

bool RVRApp::IsSessionRunning() const { return xrSessionRunning_; }

bool RVRApp::IsSessionFocused() const { return xrSessionState_ == XR_SESSION_STATE_FOCUSED; }

void RVRApp::PollActions() {
    input_.handActive = {XR_FALSE, XR_FALSE};

    // Sync actions
    const XrActiveActionSet activeActionSet{input_.actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    CHECK_XRCMD(xrSyncActions(xrSession_, &syncInfo));

    // Get pose and grab action state and start haptic vibrate when hand is 90% squeezed.
    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.action = input_.grabAction;
        getInfo.subactionPath = input_.handSubactionPath[hand];

        XrActionStateFloat grabValue{XR_TYPE_ACTION_STATE_FLOAT};
        CHECK_XRCMD(xrGetActionStateFloat(xrSession_, &getInfo, &grabValue));
        if (grabValue.isActive == XR_TRUE) {
            // Scale the rendered hand by 1.0f (open) to 0.5f (fully squeezed).
            input_.handScale[hand] = 1.0f - 0.5f * grabValue.currentState;
            if (grabValue.currentState > 0.9f) {
                XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
                vibration.amplitude = 0.5;
                vibration.duration = XR_MIN_HAPTIC_DURATION;
                vibration.frequency = XR_FREQUENCY_UNSPECIFIED;

                XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
                hapticActionInfo.action = input_.vibrateAction;
                hapticActionInfo.subactionPath = input_.handSubactionPath[hand];
                CHECK_XRCMD(xrApplyHapticFeedback(xrSession_, &hapticActionInfo, (XrHapticBaseHeader*)&vibration));
            }
        }

        getInfo.action = input_.poseAction;
        XrActionStatePose poseState{XR_TYPE_ACTION_STATE_POSE};
        CHECK_XRCMD(xrGetActionStatePose(xrSession_, &getInfo, &poseState));
        input_.handActive[hand] = poseState.isActive;
    }

    // There were no subaction paths specified for the quit action, because we don't care which hand did it.
    XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO, nullptr, input_.quitAction, XR_NULL_PATH};
    XrActionStateBoolean quitValue{XR_TYPE_ACTION_STATE_BOOLEAN};
    CHECK_XRCMD(xrGetActionStateBoolean(xrSession_, &getInfo, &quitValue));
    if ((quitValue.isActive == XR_TRUE) && (quitValue.changedSinceLastSync == XR_TRUE) && (quitValue.currentState == XR_TRUE)) {
        CHECK_XRCMD(xrRequestExitSession(xrSession_));
    }
}

void RVRApp::RenderFrame() {
    CHECK(xrSession_ != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    CHECK_XRCMD(xrWaitFrame(xrSession_, &frameWaitInfo, &frameState));

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    CHECK_XRCMD(xrBeginFrame(xrSession_, &frameBeginInfo));

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    if (frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = blendMode_;
    frameEndInfo.layerCount = (uint32_t)layers.size();
    frameEndInfo.layers = layers.data();
    CHECK_XRCMD(xrEndFrame(xrSession_, &frameEndInfo));
}


bool RVRApp::UpdateRVRObjectFromLocatedSpace(XrTime& predictedDisplayTime, XrSpace& space, Cube& rvrObject) {
    // Locate the space of interest
    XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
    XrResult res = xrLocateSpace(space, appSpace_, predictedDisplayTime, &spaceLocation);
    CHECK_XRRESULT(res, "xrLocateSpace");

    // If found and valid, map the space's location to the object
    if (XR_UNQUALIFIED_SUCCESS(res)) {
        if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
            (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
            rvrObject.Pose = spaceLocation.pose;
            return true;
        }
    }
    return false;
}

bool RVRApp::UpdateRVRObjectFromTrackedOrigin(XrTime& predictedDisplayTime,
                                              const XrVector3f playerWorldPos,
                                              Cube& rvrObject) {
    // Locate the space of interest
    XrSpaceLocation originLocation{XR_TYPE_SPACE_LOCATION};
    XrSpace trackedOrigin = referenceSpaces_[RVRReferenceSpace::RVRTrackedOrigin];
    XrResult res = xrLocateSpace(trackedOrigin, appSpace_, predictedDisplayTime, &originLocation);
    CHECK_XRRESULT(res, "xrLocateSpace");

    // If found and valid, map the space's location to the object
    if (XR_UNQUALIFIED_SUCCESS(res)) {
        if ((originLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
            (originLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
            rvrObject.Pose.orientation = originLocation.pose.orientation;
            XrVector3f relativePosition;
            XrVector3f_Sub(&relativePosition, &rvrObject.Pose.position, &playerWorldPos);
            XrVector3f_Add(&rvrObject.Pose.position, &rvrObject.Pose.position, &originLocation.pose.position);
            return true;
        }
    }
    return false;
}

Cube MakeCube(float scale, XrVector3f position) {
    Cube cube{};
    cube.Scale = {scale, scale, scale};
    cube.Pose = RVRMath::Pose::Identity();
    cube.Pose.position = position;
    return cube;
}

bool RVRApp::RenderLayer(XrTime predictedDisplayTime,
                         std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                         XrCompositionLayerProjection& layer) {
    XrResult res;

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)xrViews_.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = xrViewConfigType_;
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = appSpace_;

    res = xrLocateViews(xrSession_, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, xrViews_.data());
    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == xrConfigViews_.size());
    CHECK(viewCountOutput == swapchains_.size());

    projectionLayerViews.resize(viewCountOutput);

    // For now this is cubes but eventually we want this to be a more general object
    std::vector<Cube> cubes;

    // For each locatable space that we want to visualize, render a 25cm cube.
    for (auto space : referenceSpaces_) {
        Cube cube{};
        cube.Scale = {0.25f, 0.25f, 0.25f};
        if (UpdateRVRObjectFromLocatedSpace(predictedDisplayTime, space.second, cube))
            cubes.push_back(cube);
        else
            Log::Write(Log::Level::Info, Fmt("Unable to locate space in app space: %d", res));
    }

    // Render a 10cm cube scaled by grabAction for each hand.
    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        Cube cube{};
        float scale = 0.1f * input_.handScale[hand];
        cube.Scale = {scale, scale, scale};
        if (UpdateRVRObjectFromLocatedSpace(predictedDisplayTime, input_.handSpace[hand], cube)) {
            cubes.push_back(cube);
        }
        else if (input_.handActive[hand] == XR_TRUE) {
            // Tracking loss is expected when the hand is not active so only log a message if the hand is active.
            const char* handName[] = {"left", "right"};
            Log::Write(Log::Level::Info,
                       Fmt("Unable to locate %s hand action space in app space: %d", handName[hand], res));
        }
    }

    XrVector3f worldOrigin{0, 0, 0};
    XrVector3f z{0, 0, 5};
    XrVector3f x{5, 0, 0};
    XrVector3f zx{5, 0, 5};
    XrVector3f playerPosition{3, 0, 3};
    Cube testCubes[] {MakeCube(0.1, z),
                      MakeCube(0.5, x),
                      MakeCube(1.0, zx),
                      MakeCube(0.05, worldOrigin)};

    for (auto testCube : testCubes) {
        if (UpdateRVRObjectFromTrackedOrigin(predictedDisplayTime, playerPosition, testCube))
            cubes.push_back(testCube);
        else
            Log::Write(Log::Level::Info, Fmt("Unable to locate space in app space: %d", res));
    }

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
        projectionLayerViews[i].pose = xrViews_[i].pose;
        projectionLayerViews[i].fov = xrViews_[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

        const XrSwapchainImageBaseHeader* const swapchainImage = xrSwapchainImages_[viewSwapchain.handle][swapchainImageIndex];
        vulkanRenderer_->RenderView(projectionLayerViews[i], swapchainImage, xrColorSwapchainFormat_, cubes);

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));
    }

    layer.space = appSpace_;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}