/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/point_cloud/point_cloud_res.h>
#include <platform/asset_stream.h>
#include "happly.h"
#include "glm/gtc/type_ptr.hpp"

namespace rvr {
PointCloudResource::PointCloudResource(std::shared_ptr<RenderingContext> renderingContext, const std::string& fileName)
: renderingContext_(std::move(renderingContext)) {
    auto vertices = GetVertexDataFromPly(fileName);
    auto vertexStagingBuffer = VulkanBuffer(renderingContext_, sizeof(Geometry::Vertex),
                                            vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                            MemoryType::HostVisible);
    vertexStagingBuffer.Map();
    vertexStagingBuffer.WriteToBuffer(vertices.data());
    vertexStagingBuffer.Unmap();
    vertexBuffer_ = std::make_unique<VulkanBuffer>(renderingContext_, sizeof(Geometry::Vertex), vertices.size(),
                                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                   MemoryType::DeviceLocal);
    renderingContext_->CopyBuffer(vertexStagingBuffer.GetBuffer(), vertexBuffer_->GetBuffer(),
                                  vertexStagingBuffer.GetSizeOfBuffer(), 0, 0);
}

void PointCloudResource::AddPushConstant(glm::mat4 transform) {
    pushConstantTransforms_.push_back(transform);
}

void PointCloudResource::ClearPushConstants() {
    pushConstantTransforms_.clear();
}

std::vector<Geometry::Vertex> PointCloudResource::GetVertexDataFromPly(const std::string &fileName) {
    AssetStream sb(fileName);
    std::istream is(&sb);
    std::vector<Geometry::Vertex> vertices;
    try {
        happly::PLYData plyIn(is);
        std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
        std::vector<std::array<unsigned char, 3>> vColor = plyIn.getVertexColors();
        if (vPos.size() != vColor.size())
            throw std::runtime_error("vertex positions and colors do not match in size");

        vertices.resize(vPos.size());
        Geometry::Vertex vertex{};
        for (size_t i = 0; i < vPos.size(); i++) {
            vertex.Position.x = (float)vPos[i][0];
            vertex.Position.y = (float)vPos[i][1];
            vertex.Position.z = (float)vPos[i][2];
            vertex.Color.x    = (float)vColor[i][0] / 255.0f;
            vertex.Color.y    = (float)vColor[i][1] / 255.0f;
            vertex.Color.z    = (float)vColor[i][2] / 255.0f;
            vertices[i] = vertex;
        }
    } catch (const std::exception& e) {
        rvr::PrintError(e.what());
    }

    return vertices;
}

void PointCloudResource::Draw(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout) {
    if (!vertexBuffer_) {
        PrintWarning("Point cloud vertex buffer not done loading");
        return;
    }

    VkDeviceSize offset = 0;
    VkBuffer vtxBuffer = vertexBuffer_->GetBuffer();
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vtxBuffer, &offset);
    for (const auto& transform : pushConstantTransforms_) {
        vkCmdPushConstants(cmdBuffer, pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(transform),
                           glm::value_ptr(transform));
        vkCmdDraw(cmdBuffer, vertexBuffer_->GetCount(), 1,
                  0, 0);
    }
}
}
