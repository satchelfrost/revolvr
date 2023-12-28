/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "shader.h"
#include "vk_utils.h"
#include "vk_results.h"

#include <common.h>
#include <utility>

namespace rvr {
Shader::Shader(VkDevice device, const std::string& shaderFileName, Shader::ShaderType type) :
device_(device) {
    // Load the byte code
    auto code = CreateSPIRVVector(shaderFileName.c_str());
    if (code.empty())
        THROW(Fmt("Failed to compile shader %s", shaderFileName.c_str()));

    // Create shader module
    VkShaderModuleCreateInfo moduleInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    shaderInfo_.pName = "main";
    std::string shaderStageName;
    switch (type) {
        case ShaderType::Vertex:
            shaderInfo_.stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStageName = "vertex";
            break;
        case ShaderType::Fragment:
            shaderInfo_.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStageName = "fragment";
            break;
        default:
            THROW("No such shader stage");
    }

    moduleInfo.codeSize = code.size();
    moduleInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    CHECK_MSG((moduleInfo.codeSize > 0) && moduleInfo.pCode, Fmt("Invalid %s shader ", shaderStageName.c_str()));
    VkResult result = vkCreateShaderModule(device, &moduleInfo, nullptr,
                                           &shaderInfo_.module);
    CHECK_VKCMD(result);
    PrintInfo("Loaded " + shaderStageName + " shader " + shaderFileName);
}

VkPipelineShaderStageCreateInfo Shader::GetShaderStageInfo() {
    return shaderInfo_;
}

void Shader::PushConstant(std::string debugName, size_t sizeInBytes) {
    VkPushConstantRange range{};
    range.stageFlags = shaderInfo_.stage;
    range.offset = pushConstantOffset_;
    range.size = sizeInBytes;
    NamedPushConstant namedPushConstant {
        std::move(debugName),
        range
    };
    namedPushConstants_.push_back(namedPushConstant);

    // Update push constant offset
    pushConstantOffset_ += sizeInBytes;
}

std::vector<VkPushConstantRange> Shader::GetPushConstants() {
    std::vector<VkPushConstantRange> pushConstants;
    for (const auto& namedPushConstant : namedPushConstants_)
        pushConstants.push_back(namedPushConstant.pushConstantRange);
    return pushConstants;
}

Shader::~Shader() {
    vkDestroyShaderModule(device_, shaderInfo_.module, nullptr);
}

void Shader::AddSetLayout(VkDescriptorSetLayout layout) {
    setLayouts_.push_back(layout);
}

std::vector<VkDescriptorSetLayout> Shader::GetDescriptorSetLayouts() {
    return setLayouts_;
}
}
