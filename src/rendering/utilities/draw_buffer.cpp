/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/draw_buffer.h>
#include <rendering/utilities/vulkan_utils.h>

#include <utility>

namespace rvr {
DrawBuffer::DrawBuffer(const std::shared_ptr<RenderingContext>& context, size_t sizeOfIndex, size_t sizeOfIndices,
                       size_t sizeOfVertex, size_t sizeOfVertices, VertexBufferLayout vbl) :
vertexBufferLayout_(std::move(vbl)) {
    indexBuffer_ = std::make_unique<VulkanBuffer>(context, sizeOfIndex, sizeOfIndices,
                                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    vertexBuffer_ = std::make_unique<VulkanBuffer>(context, sizeOfVertex, sizeOfVertices,
                                                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void DrawBuffer::UpdateIndices(const void *data) {
    indexBuffer_->Update(data);
}

void DrawBuffer::UpdateVertices(const void *data) {
    vertexBuffer_->Update(data);
}

VkVertexInputBindingDescription DrawBuffer::GetVertexInputBindingDesc() {
    return vertexBufferLayout_.GetVertexInputBindingDesc();
}

std::vector<VkVertexInputAttributeDescription> DrawBuffer::GetVtxAttrDescriptions() {
    return vertexBufferLayout_.GetVtxAttrDescriptions();
}

VkBuffer DrawBuffer::GetIndexBuffer() {
    return indexBuffer_->GetBuffer();
}

VkBuffer DrawBuffer::GetVertexBuffer() {
    return vertexBuffer_->GetBuffer();
}

uint32_t DrawBuffer::GetIndexCount() {
    return indexBuffer_->GetCount();
}
}