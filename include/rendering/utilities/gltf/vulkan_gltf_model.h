#pragma once

#include <pch.h>
#include <rendering/utilities/gltf/gltf_data.h>
#include "rendering/utilities/rendering_context.h"
#include "rendering/utilities/draw_buffer.h"

#define TINYGLTF_NO_FS
#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#include <tiny_gltf.h>

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
    void LoadImages(tinygltf::Model& input);
    void LoadTextures(tinygltf::Model& input);
    void LoadMaterials(tinygltf::Model& input);
    void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, gltf::Node* parent,
                  std::vector<uint32_t>& indexBuffer, std::vector<gltf::Vertex>& vertexBuffer);
public:
    VulkanGLTFModel(std::shared_ptr<RenderingContext> renderingContext, const std::string& fileName);
    ~VulkanGLTFModel();
    void DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, gltf::Node* node);
    void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
};
}