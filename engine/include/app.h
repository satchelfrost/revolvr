#pragma once

#include <pch.h>
#include <common.h>
#include <platform/android_context.h>
#include <xr_context.h>
#include <game_loop_timer.h>
#include <rvr_reference_space.h>
#include <xr_app_helpers.h>
#include <scene.h>

#include <global_context.h>

#include <array>
#include <cmath>

namespace rvr {
class App {
public:
    App();
    ~App();
    void Run(struct android_app* app);

protected:
    void Render(XrContext* xrContext, VulkanRenderer* vulkanRenderer);
    bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                   XrCompositionLayerProjection& layer, XrContext* xrContext,
                   VulkanRenderer* vulkanRenderer);

private:
    void DrawGrid();

    float deltaTime_;
    Scene scene_;
    std::vector<Cube> renderBuffer_;
    GlobalContext* globalContext_;

    bool requestRestart_; // TODO: do something with this variable
    bool exitRenderLoop_; // TODO: do something with this variable
};
}