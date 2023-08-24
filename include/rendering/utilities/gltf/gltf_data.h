#pragma once

#include <pch.h>
#include <glm/ext.hpp>
#include <rendering/utilities/gltf/texture.h>

namespace rvr {
namespace gltf {
// The vertex layout for the samples' model
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
};

// Single vertex buffer for all primitives
struct Vertices {
    VkBuffer buffer;
    VkDeviceMemory memory;
};

// Single index buffer for all primitives
struct Indices {
    int count;
    VkBuffer buffer;
    VkDeviceMemory memory;
};

// The following structures roughly represent the glTF scene structure
// To keep things simple, they only contain those properties that are required for this sample
//    struct Node;

// A primitive contains the data for a single draw call
struct Primitive {
    uint32_t firstIndex;
    uint32_t indexCount;
    int32_t materialIndex;
};

// Contains the node's (optional) geometry and can be made up of an arbitrary number of primitives
struct Mesh {
    std::vector<Primitive> primitives;
};

// A node represents an object in the glTF scene graph
struct Node {
    Node* parent;
    std::vector<Node*> children;
    Mesh mesh;
    glm::mat4 matrix;
    ~Node() {
        for (auto& child : children) {
            delete child;
        }
    }
};

// A glTF material stores information in e.g. the texture that is attached to it and colors
struct Material {
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    uint32_t baseColorTextureIndex;
};

// Contains the texture for a single glTF image
// Images may be reused by texture objects and are as such separated
struct Image {
    VulkanTexture texture;
    // We also store (and create) a descriptor set that's used to access this texture from the fragment shader
    VkDescriptorSet descriptorSet;
};

// A glTF texture stores a reference to the image and a sampler
// In this sample, we are only interested in the image
struct Texture {
    int32_t imageIndex;
};
}
}