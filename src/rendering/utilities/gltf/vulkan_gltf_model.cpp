/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "rendering/utilities/gltf/vulkan_gltf_model.h"
#include <global_context.h>
#include <utility>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#include <tiny_gltf.h>

namespace rvr {
VulkanGLTFModel::VulkanGLTFModel(std::shared_ptr<RenderingContext> renderingContext, const std::string& fileName) :
        renderingContext_(std::move(renderingContext)) {
    LoadGLTFFile(fileName);
}

// The following non-class Load* helper functions are defined here to avoid multiple includes of tiny_gltf.h
void LoadImages(tinygltf::Model& input, std::vector<gltf::Image>& images,
                const std::shared_ptr<RenderingContext>& renderingContext);
void LoadTextures(tinygltf::Model& input, std::vector<gltf::Texture>& textures);
void LoadMaterials(tinygltf::Model& input, std::vector<gltf::Material>& materials);
void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, gltf::Node* parent,
              std::vector<uint32_t>& indexBuffer, std::vector<gltf::Vertex>& vertexBuffer,
              std::vector<gltf::Node*>& storedNodes);

void VulkanGLTFModel::LoadGLTFFile(const std::string& fileName) {
    tinygltf::asset_manager = GlobalContext::Inst()->GetAndroidContext()->GetAndroidAssetManager();
    tinygltf::Model gltfInput;
    tinygltf::TinyGLTF gltfContext;
    std::string error{}, warning{};

    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfInput, &error,
                                                    &warning, fileName);
    if (!error.empty())
        PrintError(error);
    if (!warning.empty())
        PrintWarning(warning);

    std::vector<uint32_t> gltfIndexBuffer;
    std::vector<gltf::Vertex> gltfVertexBuffer;
    if (fileLoaded) {
        LoadImages(gltfInput, images_, renderingContext_);
        LoadMaterials(gltfInput, materials_);
        LoadTextures(gltfInput, textures_);
        const tinygltf::Scene& scene = gltfInput.scenes[0];
        for (int sceneNode : scene.nodes) {
            const tinygltf::Node node = gltfInput.nodes[sceneNode];
            LoadNode(node, gltfInput, nullptr, gltfIndexBuffer, gltfVertexBuffer, nodes_);
        }
        PrintInfo("Model " + fileName + " loaded");
    }
    else {
        THROW("Failed to open gltf file " + fileName);
    }

    auto vertexStagingBuffer = VulkanBuffer(renderingContext_, sizeof(gltf::Vertex),
                                            gltfVertexBuffer.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                            MemoryType::HostVisible);
    auto indexStagingBuffer = VulkanBuffer(renderingContext_, sizeof(uint32_t),
                                           gltfIndexBuffer.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           MemoryType::HostVisible);
    vertexStagingBuffer.Map();
    vertexStagingBuffer.WriteToBuffer(gltfVertexBuffer.data());
    vertexStagingBuffer.Unmap();
    indexStagingBuffer.Map();
    indexStagingBuffer.WriteToBuffer(gltfIndexBuffer.data());
    indexStagingBuffer.Unmap();
    auto vertexBuffer = std::make_unique<VulkanBuffer>(renderingContext_, sizeof(gltf::Vertex),
                                                   gltfVertexBuffer.size(),
                                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                   MemoryType::DeviceLocal);
    auto indexBuffer = std::make_unique<VulkanBuffer>(renderingContext_, sizeof(uint32_t),
                                                  gltfIndexBuffer.size(),
                                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                  MemoryType::DeviceLocal);
    renderingContext_->CopyBuffer(vertexStagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(),
                                 vertexStagingBuffer.GetSizeOfBuffer(), 0, 0);
    renderingContext_->CopyBuffer(indexStagingBuffer.GetBuffer(), indexBuffer->GetBuffer(),
                                  indexStagingBuffer.GetSizeOfBuffer(), 0, 0);
    drawBuffer_ = std::make_unique<DrawBuffer>(std::move(indexBuffer), std::move(vertexBuffer));
}

VulkanGLTFModel::~VulkanGLTFModel() {
    for (auto node : nodes_)
        delete node;
}

void LoadImages(tinygltf::Model& input, std::vector<gltf::Image>& images,
                                 const std::shared_ptr<RenderingContext>& renderingContext) {
    // Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
    // loading them from disk, we fetch them from the glTF loader and upload the buffers
    images.resize(input.images.size());
    for (size_t i = 0; i < input.images.size(); i++) {
        tinygltf::Image& glTFImage = input.images[i];
        // Get the image data from the glTF loader
        unsigned char* buffer = nullptr;
        VkDeviceSize bufferSize = 0;
        bool deleteBuffer = false;
        // We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
        if (glTFImage.component == 3) {
            bufferSize = glTFImage.width * glTFImage.height * 4;
            buffer = new unsigned char[bufferSize];
            unsigned char* rgba = buffer;
            unsigned char* rgb = &glTFImage.image[0];
            for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
                memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                rgba += 4;
                rgb += 3;
            }
            deleteBuffer = true;
        }
        else {
            buffer = &glTFImage.image[0];
            bufferSize = glTFImage.image.size();
        }
        // Load texture from image buffer
//        images[i].texture.FromBuffer(buffer, bufferSize, VK_FORMAT_R8G8B8A8_UNORM, glTFImage.width,
//                                     glTFImage.height, renderingContext_);
        images[i].texture.FromBuffer(buffer, bufferSize, glTFImage.width,
                                      glTFImage.height, renderingContext);
        if (deleteBuffer) {
            delete[] buffer;
        }
    }
}

void LoadTextures(tinygltf::Model& input, std::vector<gltf::Texture>& textures) {
    textures.resize(input.textures.size());
    for (size_t i = 0; i < input.textures.size(); i++) {
        textures[i].imageIndex = input.textures[i].source;
    }
}

void LoadMaterials(tinygltf::Model& input, std::vector<gltf::Material>& materials) {
    materials.resize(input.materials.size());
    for (size_t i = 0; i < input.materials.size(); i++) {
        // We only read the most basic properties required for our sample
        tinygltf::Material glTFMaterial = input.materials[i];
        // Get the base color factor
        if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
            materials[i].baseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
        }
        // Get base color texture index
        if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
            materials[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
        }
    }
}

void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, gltf::Node* parent,
              std::vector<uint32_t>& indexBuffer, std::vector<gltf::Vertex>& vertexBuffer,
              std::vector<gltf::Node*>& storedNodes) {
    auto node = new gltf::Node{};
    node->matrix = glm::mat4(1.0f);
    node->parent = parent;

    // Get the local node matrix
    // It's either made up from translation, rotation, scale or a 4x4 matrix
    if (inputNode.translation.size() == 3) {
        node->matrix = glm::translate(node->matrix,
                                      glm::vec3(glm::make_vec3(inputNode.translation.data())));
    }
    if (inputNode.rotation.size() == 4) {
        glm::quat q = glm::make_quat(inputNode.rotation.data());
        node->matrix *= glm::mat4(q);
    }
    if (inputNode.scale.size() == 3) {
        node->matrix = glm::scale(node->matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
    }
    if (inputNode.matrix.size() == 16) {
        node->matrix = glm::make_mat4x4(inputNode.matrix.data());
    };

    // Load node's children
    if (inputNode.children.size() > 0)
        for (size_t i = 0; i < inputNode.children.size(); i++)
            LoadNode(input.nodes[inputNode.children[i]], input , node, indexBuffer, vertexBuffer,
                     storedNodes);

    // If the node contains mesh data, we load vertices and indices from the buffers
    // In glTF this is done via accessors and buffer views
    if (inputNode.mesh > -1) {
        const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
        // Iterate through all primitives of this node's mesh
        for (size_t i = 0; i < mesh.primitives.size(); i++) {
            const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
            uint32_t firstIndex = static_cast<uint32_t>(indexBuffer.size());
            uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
            uint32_t indexCount = 0;
            // Vertices
            {
                const float* positionBuffer = nullptr;
                const float* normalsBuffer = nullptr;
                const float* texCoordsBuffer = nullptr;
                size_t vertexCount = 0;

                // Get buffer data for vertex positions
                if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    vertexCount = accessor.count;
                }
                // Get buffer data for vertex normals
                if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }
                // Get buffer data for vertex texture coordinates
                // glTF supports multiple sets, we only load the first one
                if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }

                // Append data to model's vertex buffer
                for (size_t v = 0; v < vertexCount; v++) {
                    gltf::Vertex vert{};
                    vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
                    vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                    vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
                    vert.color = glm::vec3(1.0f);
                    vertexBuffer.push_back(vert);
                }
            }
            // Indices
            {
                const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
                const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

                indexCount += static_cast<uint32_t>(accessor.count);

                // glTF supports different component types of indices
                switch (accessor.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                        const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                        const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                        const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    default:
                        std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                        return;
                }
            }
            gltf::Primitive primitive{};
            primitive.firstIndex = firstIndex;
            primitive.indexCount = indexCount;
            primitive.materialIndex = glTFPrimitive.material;
            node->mesh.primitives.push_back(primitive);
        }
    }

    if (parent) {
        parent->children.push_back(node);
    }
    else {
        storedNodes.push_back(node);
    }
}

void VulkanGLTFModel::DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, gltf::Node* node) {
    if (node->mesh.primitives.size() > 0) {
        // Pass the node's matrix via push constants
        // Traverse the node hierarchy to the top-most parent to get the final matrix of the current node
        glm::mat4 nodeMatrix = node->matrix;
        gltf::Node* currentParent = node->parent;
        while (currentParent) {
            nodeMatrix = currentParent->matrix * nodeMatrix;
            currentParent = currentParent->parent;
        }
        glm::mat4 normalMatrix = glm::inverseTranspose(nodeMatrix);
        // Pass the final matrix to the vertex shader using push constants
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), sizeof(glm::mat4), &normalMatrix);
        for (gltf::Primitive& primitive : node->mesh.primitives) {
            if (primitive.indexCount > 0) {
                // Get the texture index for this primitive
                gltf::Texture texture = textures_[materials_[primitive.materialIndex].baseColorTextureIndex];
                // Bind the descriptor for the current primitive's texture
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &images_[texture.imageIndex].descriptorSet, 0, nullptr);
                vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
            }
        }
    }
    for (auto& child : node->children)
        DrawNode(commandBuffer, pipelineLayout, child);
}

void VulkanGLTFModel::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    // All vertices and indices are stored in single buffers, so we only need to bind once
    VkDeviceSize offsets[1] = { 0 };
    VkBuffer vertexBuffer = drawBuffer_->GetVertexBuffer();
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, drawBuffer_->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    for (auto& transform : pushConstantTransforms_) {
        // Render all nodes at top-level
        for (gltf::Node* node : nodes_) {
            // set the root node to the incoming transform
            node->matrix = transform; // TODO: perhaps transform should be a parameter to DrawNode
            DrawNode(commandBuffer, pipelineLayout, node);
        }
    }
}

uint32_t VulkanGLTFModel::GetNumImages() {
    return static_cast<uint32_t>(images_.size());
}

std::vector<gltf::Image>& VulkanGLTFModel::GetImages() {
    return images_;
}

void VulkanGLTFModel::AddPushConstant(glm::mat4 transform) {
    pushConstantTransforms_.push_back(transform);
}

void VulkanGLTFModel::ClearPushConstants() {
    pushConstantTransforms_.clear();
}
}