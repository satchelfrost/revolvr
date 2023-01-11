#pragma once

#include "pch.h"
#include "common.h"
#include "math/xr_linear.h"
#include "platform/rvr_android_context.h"
#include "renderer/rvr_vulkan_renderer.h"
#include "rvr_xr_context.h"
#include "rvr_game_loop_timer.h"
#include "rvr_reference_space.h"
#include "xr_app_helpers.h"
#include "scene.h"

#include <array>
#include <cmath>

namespace rvr {
class RVRApp {
public:

  RVRApp();
  ~RVRApp();

  void Run(struct android_app* app);

protected:
  void UpdateSystems();

  // Create and submit a frame.
  void Render();

  bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                   XrCompositionLayerProjection& layer);

private:
    float deltaTime_;
    rvr::Scene scene_;
    RVRVulkanRenderer* vulkanRenderer_;
    RVRXRContext* xrContext_;
    std::vector<Cube> renderBuffer_;
};
}