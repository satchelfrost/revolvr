/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/shader_program.h>
#include <rendering//utilities/vulkan_results.h>

namespace rvr {
ShaderProgram::ShaderProgram(VkDevice device, const std::vector<char> &vertexCode, const std::vector<char> &fragCode) :
        device_(device){
    LoadVertexShader(vertexCode);
    LoadFragmentShader(fragCode);
}

ShaderProgram::~ShaderProgram() {
    if (device_ != nullptr) {
        for (auto &si: shaderInfo_) {
            if (si.module != VK_NULL_HANDLE) {
                vkDestroyShaderModule(device_, shaderInfo_[0].module, nullptr);
            }
            si.module = VK_NULL_HANDLE;
        }
    }
    shaderInfo_ = {};
    device_ = nullptr;
}

void ShaderProgram::LoadVertexShader(const std::vector<char> &code) {
    Load(0, code);
}

void ShaderProgram::LoadFragmentShader(const std::vector<char> &code) {
    Load(1, code);
}

void ShaderProgram::Load(uint32_t index, const std::vector<char> &code) {
    VkShaderModuleCreateInfo modInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};

    auto &si = shaderInfo_[index];
    si.pName = "main";
    std::string name;

    switch (index) {
        case 0:
            si.stage = VK_SHADER_STAGE_VERTEX_BIT;
            name = "vertex";
            break;
        case 1:
            si.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            name = "fragment";
            break;
        default:
            THROW(Fmt("Unknown code index %d", index));
    }

    modInfo.codeSize = code.size();
    modInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    CHECK_MSG((modInfo.codeSize > 0) && modInfo.pCode,
              Fmt("Invalid %s shader ", name.c_str()));

    CHECK_VKCMD(vkCreateShaderModule(device_, &modInfo, nullptr, &si.module));

    Log::Write(Log::Level::Info, Fmt("Loaded %s shader", name.c_str()));
}

std::array<VkPipelineShaderStageCreateInfo, 2> ShaderProgram::GetShaderInfo() {
    return shaderInfo_;
}
}