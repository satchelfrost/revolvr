/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/shader_stages.h>
#include <rendering//utilities/vulkan_results.h>

namespace rvr {
ShaderStages::ShaderStages(VkDevice device, std::unique_ptr<VulkanShader> vertShader,
                             std::unique_ptr<VulkanShader> fragShader) : vertShader_(std::move(vertShader)),
                             fragShader_(std::move(fragShader)) {
    shaderInfo_[0] = vertShader_->GetShaderStageInfo();
    shaderInfo_[1] = fragShader_->GetShaderStageInfo();
    if (shaderInfo_[0].stage != VK_SHADER_STAGE_VERTEX_BIT)
        THROW("Vertex shader was not set with VK_SHADER_STAGE_VERTEX_BIT.");
    if (shaderInfo_[1].stage != VK_SHADER_STAGE_FRAGMENT_BIT)
        THROW("Fragment shader was not set with VK_SHADER_STAGE_FRAGMENT_BIT.");
}

std::array<VkPipelineShaderStageCreateInfo, 2> ShaderStages::GetShaderInfo() {
    return shaderInfo_;
}

std::vector<VkPushConstantRange> ShaderStages::GetPushConstants() {
    std::vector<VkPushConstantRange> vertPushConstants = vertShader_->GetPushConstants();
    std::vector<VkPushConstantRange> fragPushConstants = fragShader_->GetPushConstants();

    std::vector<VkPushConstantRange> allPushConstants = vertPushConstants;
    allPushConstants.insert(allPushConstants.end(), fragPushConstants.begin(),
                            fragPushConstants.end());
    return allPushConstants;
}

std::vector<VkDescriptorSetLayout> ShaderStages::GetDescriptorSetLayouts() {
    std::vector<VkDescriptorSetLayout> vertLayouts = vertShader_->GetDescriptorSetLayouts();
    std::vector<VkDescriptorSetLayout> fragLayouts = fragShader_->GetDescriptorSetLayouts();

    std::vector<VkDescriptorSetLayout> allLayouts = vertLayouts;
    allLayouts.insert(allLayouts.end(), fragLayouts.begin(),
                            fragLayouts.end());
    return allLayouts;
}
}