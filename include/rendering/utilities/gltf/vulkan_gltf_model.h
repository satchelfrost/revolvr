/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <rendering/utilities/gltf/gltf_data.h>
#include "rendering/utilities/rendering_context.h"
#include "rendering/utilities/draw_buffer.h"

namespace rvr {
class VulkanGLTFModel {
private:
    std::unique_ptr<DrawBuffer> drawBuffer_ = nullptr;
    std::vector<gltf::Image> images_;
    std::vector<gltf::Texture> textures_;
    std::vector<gltf::Material> materials_;
    std::vector<gltf::Node*> nodes_;
    std::shared_ptr<RenderingContext> renderingContext_;
    std::vector<glm::mat4> pushConstantTransforms_;
    void LoadGLTFFile(const std::string& fileName);

public:
    VulkanGLTFModel(std::shared_ptr<RenderingContext> renderingContext, const std::string& fileName);
    ~VulkanGLTFModel();
    void DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, gltf::Node* node);
    void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
    void AddPushConstant(glm::mat4 transform);
    void ClearPushConstants();
    uint32_t GetNumImages();
    std::vector<gltf::Image>& GetImages();
};
}