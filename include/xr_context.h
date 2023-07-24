/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <platform/android_context.h>
#include <rendering/vulkan_context.h>

#include "rvr_reference_space.h"
#include "xr_app_helpers.h"
#include <io/action/action_manager.h>
#include <io/hand_tracking/hand_tracker.h>

#include <array>
#include <cmath>

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

    // Manage session lifecycle to track if Render should be called.
    bool IsSessionRunning() const;

    // Manage session state to track if input should be processed.
    bool IsSessionFocused() const;

    // Process any events in the event queue.
    void PollXrEvents(bool* exitRenderLoop, bool* requestRestart);

    void Update();

    void RefreshTrackedSpaceLocations();

    // Render functions
    void BeginFrame();
    void AddMainLayer();
    void EndFrame();

private:
    static void InitializePlatformLoader();

    // Create an Instance and other basic instance-level initialization.
    void CreateXrInstance();

    // Select a System for the view configuration specified in the Options and initialize the graphics device for the selected
    // system.
    void InitializeSystem();

    // Create a Session and other basic session-level initialization.
    void InitializeSession();

    void InitializeActions();

    void InitializeReferenceSpaces();

    // Create a Swapchain which requires coordinating with the graphics plugin to select the format, getting the system graphics
    // properties, getting the view configuration and grabbing the resulting swapchain images.
    void CreateSwapchains();


    // Return event if one is available, otherwise return null.
    const XrEventDataBaseHeader* TryReadNextEvent();

    void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent,
                                        bool* exitRenderLoop,
                                        bool* requestRestart);

    void LogActionSourceName(Action* action) const;

    VulkanContext* vulkanRenderer_;

    XrInstance xrInstance_{XR_NULL_HANDLE};
    XrSystemId xrSystemId_{XR_NULL_SYSTEM_ID};

    std::map<RVRReferenceSpace, XrSpace> initializedRefSpaces_;

    // Application's current lifecycle state according to the runtime
    XrSessionState xrSessionState_{XR_SESSION_STATE_UNKNOWN};
    bool xrSessionRunning_{false};

    XrEventDataBuffer xrEventDataBuffer_;

public:
    XrSession session{XR_NULL_HANDLE};
    XrSpace appSpace{XR_NULL_HANDLE};

    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    XrCompositionLayerProjection mainLayer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};

    std::vector<XrView> views;
    std::vector<XrViewConfigurationView> configViews;
    XrViewConfigurationType viewConfigType{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};

    std::vector<Swapchain> swapchains;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> swapchainImageMap;
    int64_t colorSwapchainFormat{-1};

    ActionManager actionManager;
    TrackedSpaceLocations trackedSpaceLocations;

    // TODO: Create hand tracker manager so that these can be private
    HandTracker handTrackerLeft_;
    HandTracker handTrackerRight_;

private:
    std::vector<XrCompositionLayerBaseHeader*> layers_;
};
}