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
#include <rendering/utilities/vertex_buffer.h>

namespace rvr {
class SwapchainImageContext {
private:
    std::vector <XrSwapchainImageVulkan2KHR> swapchainImages_;
    std::vector <RenderTarget> renderTarget_;
    VkExtent2D size_{};
    DepthBuffer depthBuffer_{};
    RenderPass renderPass_{};
    Pipeline pipeline_{};
    XrStructureType swapchainImageType_;
    VkDevice device_{VK_NULL_HANDLE};

public:
    void TransitionLayout(CmdBuffer* cmdBuffer, VkImageLayout imageLayout);
    SwapchainImageContext(XrStructureType swapchainImageType);
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t capacity,
                const XrSwapchainCreateInfo &swapchainCreateInfo, const PipelineLayout &layout,
                const ShaderProgram &sp, const VertexBuffer<Geometry::Vertex> &vb);
    void BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo);
    XrSwapchainImageBaseHeader* GetFirstImagePointer();
    VkPipeline GetPipeline();
};
}