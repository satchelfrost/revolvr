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

namespace Side {
    const int LEFT = 0;
    const int RIGHT = 1;
    const int COUNT = 2;
};  // namespace Side

struct InputState {
    XrActionSet actionSet{XR_NULL_HANDLE};
    XrAction grabAction{XR_NULL_HANDLE};
    XrAction poseAction{XR_NULL_HANDLE};
    XrAction vibrateAction{XR_NULL_HANDLE};
    XrAction quitAction{XR_NULL_HANDLE};
    std::array<XrPath, Side::COUNT> handSubactionPath;
    std::array<XrSpace, Side::COUNT> handSpace;
    std::array<float, Side::COUNT> handScale = {{1.0f, 1.0f}};
    std::array<XrBool32, Side::COUNT> handActive;
};

struct Swapchain {
    XrSwapchain handle;
    int32_t width;
    int32_t height;
};

class RVRXRContext {
public:
    RVRXRContext(RVRAndroidContext* androidContext, RVRVulkanRenderer* vulkanRenderer);

    ~RVRXRContext();

    void Initialize();

    // Manage session lifecycle to track if Render should be called.
    bool IsSessionRunning() const;

    // Manage session state to track if input should be processed.
    bool IsSessionFocused() const;

    // Process any events in the event queue.
    void PollXrEvents(bool* exitRenderLoop, bool* requestRestart);

    // Sample input actions and generate haptic feedback.
    void PollActions();

    void RefreshTrackedSpaceLocations();

private:

    void InitializePlatformLoader();

    // Create an Instance and other basic instance-level initialization.
    void CreateInstance();

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

    void LogActionSourceName(XrAction action, const std::string& actionName) const;

    const RVRAndroidContext* androidContext_;
    RVRVulkanRenderer* vulkanRenderer_;

    XrInstance xrInstance_{XR_NULL_HANDLE};
    XrSystemId xrSystemId_{XR_NULL_SYSTEM_ID};

    std::map<RVRReferenceSpace, XrSpace> initializedRefSpaces_;

    // Application's current lifecycle state according to the runtime
    XrSessionState xrSessionState_{XR_SESSION_STATE_UNKNOWN};
    bool xrSessionRunning_{false};

    XrEventDataBuffer xrEventDataBuffer_;

public:
    XrSession session{XR_NULL_HANDLE};
    XrTime predictedDisplayTime{XR_NO_DURATION};
    XrSpace appSpace{XR_NULL_HANDLE};

    std::vector<XrView> views;
    std::vector<XrViewConfigurationView> configViews;
    XrViewConfigurationType viewConfigType{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};

    std::vector<Swapchain> swapchains;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> swapchainImageMap;
    int64_t colorSwapchainFormat{-1};

    InputState input;
    TrackedSpaceLocations trackedSpaceLocations;
};

