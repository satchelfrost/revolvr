/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <array>

namespace rvr {
class ShaderProgram {
public:
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderInfo{
            {{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
             {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}}};

    ShaderProgram() = default;
    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;
    ShaderProgram(ShaderProgram &&) = delete;
    ShaderProgram &operator=(ShaderProgram &&) = delete;

    ~ShaderProgram();

    void LoadVertexShader(const std::vector<char> &code);
    void LoadFragmentShader(const std::vector<char> &code);

    void Init(VkDevice device);

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};

    void Load(uint32_t index, const std::vector<char> &code);
};
}