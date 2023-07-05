/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <pch.h>
#include <common.h>

#include <rendering/utilities/depth_buffer.h>
#include <rendering/utilities/geometry.h>
#include <rendering/utilities/pipeline.h>
#include <rendering/utilities/render_target.h>
#include <rendering/utilities/render_pass.h>
#include <rendering/utilities/vertex_buffer.h>

class MemoryAllocator;

class SwapchainImageContext {
public:
    // A packed array of XrSwapchainImageVulkan2KHR's for xrEnumerateSwapchainImages
    std::vector <XrSwapchainImageVulkan2KHR> swapchainImages;
    std::vector <RenderTarget> renderTarget;
    VkExtent2D size{};
    DepthBuffer depthBuffer{};
    RenderPass rp{};
    Pipeline pipe{};
    XrStructureType swapchainImageType;

    SwapchainImageContext() = default;
    SwapchainImageContext(XrStructureType _swapchainImageType);

    std::vector<XrSwapchainImageBaseHeader *> Create(VkDevice device,
           MemoryAllocator *memAllocator, uint32_t capacity,
           const XrSwapchainCreateInfo &swapchainCreateInfo, const PipelineLayout &layout,
           const ShaderProgram &sp, const VertexBuffer<Geometry::Vertex> &vb);

    uint32_t ImageIndex(const XrSwapchainImageBaseHeader *swapchainImageHeader);

    void BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo);

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};


