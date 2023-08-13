/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/depth_buffer.h>
#include <rendering/utilities/command_buffer.h>
#include <rendering/utilities/pipeline.h>
#include <rendering/utilities/render_target.h>
#include <math/transform.h>

namespace rvr {
class SwapchainImageContext {
private:
    std::shared_ptr<RenderingContext> renderingContext_;
    std::vector <XrSwapchainImageVulkan2KHR> swapchainImages_;
    std::vector<VkImageView> swapChainImageViews_;

    VkExtent2D swapchainExtent_;
    VkViewport viewport_{};
    VkRect2D scissor_{};

    std::vector <std::unique_ptr<RenderTarget>> renderTargets_;

    std::unique_ptr<CommandBuffer> cmdBuffer_;
//    std::unique_ptr<VulkanImage> colorImage_;
    uint32_t sampleCount_;

public:
    SwapchainImageContext(const std::shared_ptr<RenderingContext>& renderingContext, uint32_t capacity,
                          const XrSwapchainCreateInfo &swapchainCreateInfo);
    void Draw(uint32_t imageIndex, uint32_t indexCount, const std::shared_ptr<Pipeline>& pipeline,
              const std::vector<math::Transform> &transforms);
    void BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo);
    XrSwapchainImageBaseHeader* GetFirstImagePointer();
    void InitRenderTargets();
    VkExtent2D GetSwapchainExtent();
    VkSampleCountFlagBits GetSampleFlagBits() const;
};
}