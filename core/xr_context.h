/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <platform/android_context.h>
#include <rendering/vk_context.h>

#include "rvr_reference_space.h"
#include "xr_app_helpers.h"
#include <io/action/action_manager.h>
#include <io/hand_tracking/hand_tracker.h>

#include <array>
#include <cmath>
#include <rendering/passthrough/passthrough.h>

namespace rvr {
struct Swapchain {
    XrSwapchain handle;
    int32_t width;
    int32_t height;
};

class XrContext {
public:
    XrContext();
    ~XrContext();
    bool IsSessionRunning() const;
    bool IsSessionFocused() const;
    void PollXrEvents(bool* exitRenderLoop, bool* requestRestart);
    void Update();
    void RefreshTrackedSpaceLocations();
    void BeginFrame();
    void EndFrame();
    void Render();
    void RequestExit();

private:
    bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer);
    static void InitializePlatformLoader();
    void CreateXrInstance();
    void InitializeSystem();
    void InitializeSession();
    void InitializeActions();
    void InitializeReferenceSpaces();
    void CreateSwapchains();
    const XrEventDataBaseHeader* TryReadNextEvent();
    void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent,
                                        bool* exitRenderLoop,
                                        bool* requestRestart);
    void LogActionSourceName(Action* action) const;

    VulkanContext* vulkanContext_;
    XrInstance xrInstance_{XR_NULL_HANDLE};
    XrSystemId xrSystemId_{XR_NULL_SYSTEM_ID};
    std::map<RVRReferenceSpace, XrSpace> initializedRefSpaces_;
    XrSessionState xrSessionState_{XR_SESSION_STATE_UNKNOWN};
    bool xrSessionRunning_{false};
    XrEventDataBuffer xrEventDataBuffer_;

    XrSession session_{XR_NULL_HANDLE};
    XrSpace appSpace_{XR_NULL_HANDLE};

    XrFrameState frameState_{XR_TYPE_FRAME_STATE};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews_;
    std::vector<XrCompositionLayerBaseHeader*> layers_;
    XrCompositionLayerProjection layer_{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrView> views_;
    std::vector<XrViewConfigurationView> configViews_;
    XrViewConfigurationType viewConfigType_{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};

    // Passthrough
    std::unique_ptr<PassThrough> passThrough_;
    XrCompositionLayerPassthroughFB ptLayer_{XR_TYPE_COMPOSITION_LAYER_PASSTHROUGH_FB};

    std::vector<Swapchain> swapchains_;
    std::map<XrSwapchain, const XrSwapchainImageBaseHeader*> swapchainImages_;

public:
    ActionManager actionManager;
    TrackedSpaceLocations trackedSpaceLocations;

    HandTracker handTrackerLeft_;
    HandTracker handTrackerRight_;
};
}