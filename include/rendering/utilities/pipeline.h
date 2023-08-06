/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/render_pass.h>
#include <rendering/utilities/shader_program.h>
#include <rendering/utilities/vertex_buffer.h>

namespace rvr {
class PipelineLayout {
public:
    VkPipelineLayout layout{VK_NULL_HANDLE};

    PipelineLayout() = default;

    PipelineLayout(const PipelineLayout &) = delete;
    PipelineLayout &operator=(const PipelineLayout &) = delete;
    PipelineLayout(PipelineLayout &&) = delete;
    PipelineLayout &operator=(PipelineLayout &&) = delete;

    ~PipelineLayout();

    void Create(VkDevice device);

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};

// Pipeline wrapper for rendering pipeline state
class Pipeline {
public:
    VkPipeline pipe{VK_NULL_HANDLE};
    VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::vector <VkDynamicState> dynamicStateEnables;

    Pipeline() = default;

    void Dynamic(VkDynamicState state);
    void Create(VkDevice device, VkExtent2D size, const PipelineLayout &layout, const RenderPass &rp,
           const ShaderProgram &sp,
           const VertexBufferBase &vb);

    void Release();

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
}