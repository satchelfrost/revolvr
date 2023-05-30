#pragma once

#include <pch.h>
#include <common.h>
#include <array>

// ShaderProgram to hold a pair of vertex & fragment shaders
struct ShaderProgram {
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderInfo{
            {{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
             {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}}};

    ShaderProgram() = default;

    ~ShaderProgram() {
        if (m_vkDevice != nullptr) {
            for (auto &si: shaderInfo) {
                if (si.module != VK_NULL_HANDLE) {
                    vkDestroyShaderModule(m_vkDevice, shaderInfo[0].module, nullptr);
                }
                si.module = VK_NULL_HANDLE;
            }
        }
        shaderInfo = {};
        m_vkDevice = nullptr;
    }

    ShaderProgram(const ShaderProgram &) = delete;

    ShaderProgram &operator=(const ShaderProgram &) = delete;

    ShaderProgram(ShaderProgram &&) = delete;

    ShaderProgram &operator=(ShaderProgram &&) = delete;

    void LoadVertexShader(const std::vector<char> &code) { Load(0, code); }

    void LoadFragmentShader(const std::vector<char> &code) { Load(1, code); }

    void Init(VkDevice device) { m_vkDevice = device; }

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};

    void Load(uint32_t index, const std::vector<char> &code) {
        VkShaderModuleCreateInfo modInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};

        auto &si = shaderInfo[index];
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

        CHECK_VKCMD(vkCreateShaderModule(m_vkDevice, &modInfo, nullptr, &si.module));

        Log::Write(Log::Level::Info, Fmt("Loaded %s shader", name.c_str()));
    }
};
