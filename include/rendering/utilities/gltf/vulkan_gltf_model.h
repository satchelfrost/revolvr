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
    void LoadGLTFFile(const std::string& fileName);

public:
    VulkanGLTFModel(std::shared_ptr<RenderingContext> renderingContext, const std::string& fileName);
    ~VulkanGLTFModel();
    void DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, gltf::Node* node);
    void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
    uint32_t GetNumImages();
};
}