/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/shader_program.h>
#include <rendering//utilities/vulkan_results.h>

namespace rvr {
ShaderProgram::ShaderProgram(VkDevice device, std::unique_ptr<VulkanShader> vertShader,
                             std::unique_ptr<VulkanShader> fragShader) : vertShader_(std::move(vertShader)),
                             fragShader_(std::move(fragShader)) {
    shaderInfo_[0] = vertShader_->GetShaderStageInfo();
    shaderInfo_[1] = fragShader_->GetShaderStageInfo();
}

std::array<VkPipelineShaderStageCreateInfo, 2> ShaderProgram::GetShaderInfo() {
    return shaderInfo_;
}

std::vector<VkPushConstantRange> ShaderProgram::GetPushConstants() {
    std::vector<VkPushConstantRange> vertPushConstants = vertShader_->GetPushConstants();
    std::vector<VkPushConstantRange> fragPushConstants = fragShader_->GetPushConstants();

    std::vector<VkPushConstantRange> allPushConstants = vertPushConstants;
    allPushConstants.insert(allPushConstants.end(), fragPushConstants.begin(),
                            fragPushConstants.end());
    return allPushConstants;
}
}