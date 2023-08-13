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
#include <rendering/utilities/draw_buffer.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class PipelineLayout {
private:
    VkDevice device_{VK_NULL_HANDLE};

public:
    VkPipelineLayout layout{VK_NULL_HANDLE};

    void Create(VkDevice device);
    ~PipelineLayout();
    PipelineLayout() = default;
    PipelineLayout(const PipelineLayout &) = delete;
    PipelineLayout &operator=(const PipelineLayout &) = delete;
    PipelineLayout(PipelineLayout &&) = delete;
    PipelineLayout &operator=(PipelineLayout &&) = delete;
};

class Pipeline {
private:
    VkDevice device_{VK_NULL_HANDLE};
    PipelineLayout pipelineLayout_{};
    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPrimitiveTopology topology_{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::vector <VkDynamicState> dynamicStateEnables_;
    std::shared_ptr<DrawBuffer> drawBuffer_;

public:
    void Dynamic(VkDynamicState state);
    Pipeline(std::shared_ptr<RenderingContext>& context, ShaderProgram& shaderProgram,
             const std::shared_ptr<DrawBuffer>& drawBuffer);
    VkPipeline GetPipeline();
    void Release();
    Pipeline() = default;
};
}