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

class RVRApp {
public:

  RVRApp();
  ~RVRApp();

  void Run(struct android_app* app);

  void HandleAndroidCmd(android_app *app, int32_t cmd);

protected:

    virtual void OnInitialize() {}
    virtual void OnShutdown() {}

    virtual void OnUpdate() {}
    virtual void OnInput() {}

    virtual void OnRender() {}

private:

  // Process and react to input
  // TODO: This method needs to reevaluated
  void Input();

  // Update game state
  void Update();

  // Create and submit a frame.
  void Render();

  bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                   XrCompositionLayerProjection& layer);

  void SetDeltaTime(float dt) { deltaTime_ = dt; }

private:
    float deltaTime_;
    rvr::Scene scene_;
    RVRAndroidContext* androidContext_;
    RVRVulkanRenderer* vulkanRenderer_;
    RVRXRContext* xrContext_;
    std::vector<Cube> renderBuffer_;
};
