/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <array>
#include "shader.h"

namespace rvr {
class ShaderStages {
private:
    VkDevice device_{VK_NULL_HANDLE};
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderInfo_{{{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
                                                                {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}}};
    std::unique_ptr<Shader> vertShader_;
    std::unique_ptr<Shader> fragShader_;

public:
    std::array<VkPipelineShaderStageCreateInfo, 2> GetShaderInfo();
    ShaderStages(VkDevice device, std::unique_ptr<Shader> vertShader, std::unique_ptr<Shader> fragShader);
    std::vector<VkPushConstantRange> GetPushConstants();
    std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts();
};
}
