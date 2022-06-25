#pragma once

#include "pch.h"
#include "common.h"
#include "options.h"
#include "rvr_vulkan_renderer.h"
#include "xr_linear.h"
#include "rvr_android_platform.h"
#include "rvr_reference_space.h"
#include <array>
#include <cmath>

namespace Side {
    const int LEFT = 0;
    const int RIGHT = 1;
    const int COUNT = 2;
};  // namespace Side

class RVRApp {
public:
  RVRApp(RVRAndroidPlatform* androidPlatform, RVRVulkanRenderer* vulkanRenderer);

  ~RVRApp();

  // Create an Instance and other basic instance-level initialization.
  void CreateInstance();

  // Select a System for the view configuration specified in the Options and initialize the graphics device for the selected
  // system.
  void InitializeSystem();

  // Create a Session and other basic session-level initialization.
  void InitializeSession();

  void InitializeActions();

  // Create a Swapchain which requires coordinating with the graphics plugin to select the format, getting the system graphics
  // properties, getting the view configuration and grabbing the resulting swapchain images.
  void CreateSwapchains();

  // Process any events in the event queue.
  void PollXrEvents(bool* exitRenderLoop, bool* requestRestart);

  // Manage session lifecycle to track if RenderFrame should be called.
  bool IsSessionRunning() const;

  // Manage session state to track if input should be processed.
  bool IsSessionFocused() const;

  // Sample input actions and generate haptic feedback.
  void PollActions();

  // Create and submit a frame.
  void RenderFrame();

  bool RenderLayer(XrTime predictedDisplayTime,
                   std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                   XrCompositionLayerProjection& layer);

  void CreateVisualizedSpaces();

  // Return event if one is available, otherwise return null.
  const XrEventDataBaseHeader* TryReadNextEvent();

  void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent, bool* exitRenderLoop,
                                      bool* requestRestart);

  void LogActionSourceName(XrAction action, const std::string& actionName) const;
  bool UpdateRVRObjectFromLocatedSpace(XrTime& predictedDisplayTime, XrSpace& space, Cube& rvrObject);
  bool UpdateRVRObjectFromTrackedOrigin(XrTime& predictedDisplayTime, const XrVector3f playerWorldPos, Cube& rvrObject);

private:
    RVRAndroidPlatform* androidPlatform_;
    RVRVulkanRenderer* vulkanRenderer_;
    XrInstance xrInstance_{XR_NULL_HANDLE};
    XrSession xrSession_{XR_NULL_HANDLE};
    XrSpace appSpace_{XR_NULL_HANDLE};
    XrViewConfigurationType xrViewConfigType_{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    XrSystemId xrSystemId_{XR_NULL_SYSTEM_ID};
    XrFormFactor formFactor_{XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};
    XrEnvironmentBlendMode blendMode_{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};

    std::vector<XrViewConfigurationView> xrConfigViews_;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> xrSwapchainImages_;
    std::vector<XrView> xrViews_;
    int64_t xrColorSwapchainFormat_{-1};

    std::map<RVRReferenceSpace, XrSpace> referenceSpaces_;

    // Application's current lifecycle state according to the runtime
    XrSessionState xrSessionState_{XR_SESSION_STATE_UNKNOWN};
    bool xrSessionRunning_{false};

    XrEventDataBuffer xrEventDataBuffer_;

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
    InputState input_;

    struct Swapchain {
        XrSwapchain handle;
        int32_t width;
        int32_t height;
    };
    std::vector<Swapchain> swapchains_;

};
