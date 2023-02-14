#pragma once

#include <pch.h>
#include <common.h>
#include <math/xr_linear.h>
#include <platform/android_context.h>
#include <renderer/vulkan_renderer.h>
#include <xr_context.h>
#include <game_loop_timer.h>
#include <rvr_reference_space.h>
#include <xr_app_helpers.h>
#include <scene.h>

#include <array>
#include <cmath>

namespace rvr {
class App {
public:

  App();
  ~App();

  void Run(struct android_app* app);

protected:
  void UpdateSystems();

  // Create and submit a frame.
  void Render();

  bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                   XrCompositionLayerProjection& layer);

private:
    float deltaTime_;
    Scene scene_;
    VulkanRenderer* vulkanRenderer_;
    XrContext* xrContext_;
    std::vector<Cube> renderBuffer_;

    bool requestRestart_; // TODO: do something with this variable
    bool exitRenderLoop_; // TODO: do something with this variable
};
}