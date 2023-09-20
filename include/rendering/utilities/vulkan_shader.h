#pragma once

#include <pch.h>

namespace rvr {
class VulkanShader {
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
    VulkanShader(VkDevice device, const std::string& shaderFileName, ShaderType type);
    ~VulkanShader();
    VkPipelineShaderStageCreateInfo GetShaderStageInfo();
    void PushConstant(std::string debugName, size_t sizeInBytes);
    std::vector<VkPushConstantRange> GetPushConstants();
    void AddSetLayout(VkDescriptorSetLayout layout);
    std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts();
};
}