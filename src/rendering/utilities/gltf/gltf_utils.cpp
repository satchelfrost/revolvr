#include <rendering/utilities/gltf/gltf_utils.h>
#include <global_context.h>
#include <rendering/utilities/draw_buffer.h>

#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#include <tiny_gltf.h>

namespace rvr {
void loadGLTFFile(const std::string fileName, VulkanGLTFModel& gltfModel) {
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

    std::vector<uint32_t> indexBuffer;
    std::vector<gltf::Vertex> vertexBuffer;
    if (fileLoaded) {
        gltfModel.LoadImages(gltfInput);
        gltfModel.LoadMaterials(gltfInput);
        gltfModel.LoadTextures(gltfInput);
        const tinygltf::Scene& scene = gltfInput.scenes[0];
        for (int sceneNode : scene.nodes) {
            const tinygltf::Node node = gltfInput.nodes[sceneNode];
            gltfModel.LoadNode(node, gltfInput, nullptr, indexBuffer, vertexBuffer);
        }
    }
    else {
        THROW("Failed to open gltf file " + fileName);
    }
    auto renderingContext = GlobalContext::Inst()->GetVulkanContext()->GetRenderingContext();
    auto vertexStagingBuffer = VulkanBuffer(renderingContext, sizeof(gltf::Vertex),
                                            vertexBuffer.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                            MemoryType::HostVisible);
    auto indexStagingBuffer = VulkanBuffer(renderingContext, sizeof(uint32_t),
                                           indexBuffer.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           MemoryType::HostVisible);
    vertexStagingBuffer.Update(vertexBuffer.data());
    indexStagingBuffer.Update(indexBuffer.data());
    gltfModel.vertexBuffer_ = std::make_unique<VulkanBuffer>(renderingContext, sizeof(gltf::Vertex),
                                            vertexBuffer.size(),
                                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                            MemoryType::DeviceLocal);
    gltfModel.indexBuffer_ = VulkanBuffer(renderingContext, sizeof(uint32_t),
                                           indexBuffer.size(),
                                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                          MemoryType::DeviceLocal);
//    renderingContext->CopyBuffer(vertexStagingBuffer.GetBuffer(), targetVertexBuffer.GetBuffer(),
//                                 vertexStagingBuffer.GetSizeInBytes(), )
}
}