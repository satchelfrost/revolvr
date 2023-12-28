/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

namespace rvr {
class Shader {
public:
enum ShaderType {
    Vertex,
    Fragment
};
private:
    VkPipelineShaderStageCreateInfo shaderInfo_{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    struct NamedPushConstant {
        std::string debugName;
        VkPushConstantRange pushConstantRange;
    };
    std::vector<NamedPushConstant> namedPushConstants_{};
    uint32_t pushConstantOffset_ = 0;
    VkDevice device_;
    std::vector<VkDescriptorSetLayout> setLayouts_;
public:
    Shader(VkDevice device, const std::string& shaderFileName, ShaderType type);
    ~Shader();
    VkPipelineShaderStageCreateInfo GetShaderStageInfo();
    void PushConstant(std::string debugName, size_t sizeInBytes);
    std::vector<VkPushConstantRange> GetPushConstants();
    void AddSetLayout(VkDescriptorSetLayout layout);
    std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts();
};
}
