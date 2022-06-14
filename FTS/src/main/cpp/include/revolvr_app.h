#pragma once

#include "pch.h"
#include "common.h"
#include "options.h"
#include "graphicsplugin.h"
#include "xr_linear.h"
#include "rvr_android_platform.h"
#include <array>
#include <cmath>

namespace Side {
    const int LEFT = 0;
    const int RIGHT = 1;
    const int COUNT = 2;
};  // namespace Side

class RVRApp {
public:
  RVRApp(const std::shared_ptr<Options>& options,
         RVRAndroidPlatform& android_platform,
         const std::shared_ptr<IGraphicsPlugin>& graphicsPlugin);

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
  void PollEvents(bool* exitRenderLoop, bool* requestRestart);

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

  void CreateInstanceInternal();

  void CreateVisualizedSpaces();

  // Return event if one is available, otherwise return null.
  const XrEventDataBaseHeader* TryReadNextEvent();

  void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent, bool* exitRenderLoop,
                                      bool* requestRestart);

  // Loggers
  void LogInstanceInfo();
  void LogViewConfigurations();
  void LogEnvironmentBlendMode(XrViewConfigurationType type);
  void LogReferenceSpaces();
  void LogLayersAndExtensions();
  void LogActionSourceName(XrAction action, const std::string& actionName) const;

private:
    const std::shared_ptr<Options> m_options;
    RVRAndroidPlatform m_androidPlatform;
    std::shared_ptr<IGraphicsPlugin> m_graphicsPlugin;
    XrInstance m_instance{XR_NULL_HANDLE};
    XrSession m_session{XR_NULL_HANDLE};
    XrSpace m_appSpace{XR_NULL_HANDLE};
    XrFormFactor m_formFactor{XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};
    XrViewConfigurationType m_viewConfigType{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    XrEnvironmentBlendMode m_environmentBlendMode{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};
    XrSystemId m_systemId{XR_NULL_SYSTEM_ID};

    std::vector<XrViewConfigurationView> m_configViews;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> m_swapchainImages;
    std::vector<XrView> m_views;
    int64_t m_colorSwapchainFormat{-1};

    std::vector<XrSpace> m_visualizedSpaces;

    // Application's current lifecycle state according to the runtime
    XrSessionState m_sessionState{XR_SESSION_STATE_UNKNOWN};
    bool m_sessionRunning{false};

    XrEventDataBuffer m_eventDataBuffer;

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
    InputState m_input;

    struct Swapchain {
        XrSwapchain handle;
        int32_t width;
        int32_t height;
    };
    std::vector<Swapchain> m_swapchains;

};
