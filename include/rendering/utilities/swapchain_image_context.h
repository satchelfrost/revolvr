/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/depth_buffer.h>
#include <rendering/utilities/geometry.h>
#include <rendering/utilities/pipeline.h>
#include <rendering/utilities/render_target.h>
#include <rendering/utilities/render_pass.h>
#include <rendering/utilities/draw_buffer.h>
#include <math/transform.h>

namespace rvr {
class SwapchainImageContext {
private:
    std::vector <XrSwapchainImageVulkan2KHR> swapchainImages_;
    VkFormat swapchainImageFormat_;
    VkExtent2D swapchainExtent_;
    VkViewport viewport_{};
    VkRect2D scissor_{};
    std::vector <RenderTarget> renderTarget_;
    DepthBuffer depthBuffer_;
    RenderPass renderPass_;

public:
    SwapchainImageContext(uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo);
    void Draw(uint32_t imageIndex, uint32_t indexCount, std::shared_ptr<Pipeline> pipeline,
              const std::vector<math::Transform> &transforms);
    void BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo);
    XrSwapchainImageBaseHeader* GetFirstImagePointer();
    void InitResources();
};
}