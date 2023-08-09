#pragma once

#include <pch.h>
#include <tiny_gltf.h>
#include <rendering/utilities/gltf/gltf_data.h>

namespace rvr {
class VulkanGLTFModel {
public:
    VkDevice device_;
    VkQueue copyQueue_;

    gltf::Vertices vertices_;
    gltf::Indices indices_;
    std::vector<gltf::Image> images_;
    std::vector<gltf::Texture> textures_;
    std::vector<gltf::Material> materials_;
    std::vector<gltf::Node*> nodes_;

    ~VulkanGLTFModel();
    void LoadImages(tinygltf::Model& input);
    void LoadTextures(tinygltf::Model& input);
    void LoadMaterials(tinygltf::Model& input);
    void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, gltf::Node* parent,
                  std::vector<uint32_t>& indexBuffer, std::vector<gltf::Vertex>& vertexBuffer);

    void DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, gltf::Node* node);
    void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
};
}