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
    VkPipelineLayout layout_{VK_NULL_HANDLE};

public:
    PipelineLayout(VkDevice device, std::vector<VkPushConstantRange> pushConstantRanges);
    ~PipelineLayout();
    VkPipelineLayout GetLayout();
};

class Pipeline {
private:
    VkDevice device_{VK_NULL_HANDLE};
    VkPipeline pipeline_{VK_NULL_HANDLE};
    VkPrimitiveTopology topology_{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::unique_ptr<PipelineLayout> pipelineLayout_;

public:
//    Pipeline(std::shared_ptr<RenderingContext>& context, const std::unique_ptr<ShaderProgram>& shaderProgram,
//             VertexBufferLayout vertexBufferLayout, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
    Pipeline(std::shared_ptr<RenderingContext>& context, const std::unique_ptr<ShaderProgram>& shaderProgram,
             VertexBufferLayout vertexBufferLayout);
    ~Pipeline();
    void BindPipeline(VkCommandBuffer cmdBuffer);
    VkPipelineLayout GetPipelineLayout();
};
}