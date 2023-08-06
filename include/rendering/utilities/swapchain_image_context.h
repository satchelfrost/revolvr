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

class MemoryAllocator;

class SwapchainImageContext {
public:
    std::vector <XrSwapchainImageVulkan2KHR> swapchainImages;
    std::vector <RenderTarget> renderTarget;
    VkExtent2D size{};
    DepthBuffer depthBuffer{};
    RenderPass rp{};
    Pipeline pipe{};
    XrStructureType swapchainImageType;

    SwapchainImageContext() = default;
    SwapchainImageContext(XrStructureType _swapchainImageType);

    void Create(VkDevice device, MemoryAllocator *memAllocator, uint32_t capacity,
                const XrSwapchainCreateInfo &swapchainCreateInfo, const PipelineLayout &layout,
                const ShaderProgram &sp, const VertexBuffer<Geometry::Vertex> &vb);

    void BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo);
    XrSwapchainImageBaseHeader* GetFirstImagePointer();

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};


