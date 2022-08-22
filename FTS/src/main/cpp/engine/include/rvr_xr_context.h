#pragma once

#include "pch.h"
#include "include/common.h"
#include "include/platform/rvr_android_context.h"
#include "include/renderer/rvr_vulkan_renderer.h"
#include "include/math/xr_linear.h"

#include "rvr_reference_space.h"
#include "rvr_scene_tree.h"
#include "xr_app_helpers.h"

#include <array>
#include <cmath>

class RVRXRContext {
public:
    RVRXRContext();

    // Create an Instance and other basic instance-level initialization.
    void CreateInstance();

    // Select a System for the view configuration specified in the Options and initialize the graphics device for the selected
    // system.
    void InitializeSystem();

    // Create a Session and other basic session-level initialization.
    void InitializeSession();

    void InitializeActions();

    void InitializeReferenceSpaces();

    // Return event if one is available, otherwise return null.
    const XrEventDataBaseHeader* TryReadNextEvent();

    // Process any events in the event queue.
    void PollXrEvents(bool* exitRenderLoop, bool* requestRestart);

    void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent,
                                        bool* exitRenderLoop,
                                        bool* requestRestart);

    // Manage session lifecycle to track if RenderFrame should be called.
    bool IsSessionRunning() const;

    // Manage session state to track if input should be processed.
    bool IsSessionFocused() const;

private:
    XrInstance xrInstance_{XR_NULL_HANDLE};
    XrSession xrSession_{XR_NULL_HANDLE};
    XrSpace appSpace_{XR_NULL_HANDLE};
    XrViewConfigurationType xrViewConfigType_{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    XrSystemId xrSystemId_{XR_NULL_SYSTEM_ID};
    XrTime predictedDisplayTime_{XR_NO_DURATION};

    std::vector<XrViewConfigurationView> xrConfigViews_;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> xrSwapchainImages_;
    std::vector<XrView> xrViews_;
    int64_t xrColorSwapchainFormat_{-1};

    std::map<RVRReferenceSpace, XrSpace> initializedRefSpaces_;

    // Application's current lifecycle state according to the runtime
    XrSessionState xrSessionState_{XR_SESSION_STATE_UNKNOWN};
    bool xrSessionRunning_{false};

    XrEventDataBuffer xrEventDataBuffer_;

    TrackedSpaceLocations trackedSpaceLocations_;
};

