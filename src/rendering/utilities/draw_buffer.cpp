/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/draw_buffer.h>
#include <rendering/utilities/vulkan_utils.h>

#include <utility>

namespace rvr {
DrawBuffer::DrawBuffer(std::unique_ptr<VulkanBuffer> indexBuffer, std::unique_ptr<VulkanBuffer> vertexBuffer) :
indexBuffer_(std::move(indexBuffer)), vertexBuffer_(std::move(vertexBuffer)) {}

void DrawBuffer::UpdateIndices(const void *data) {
    indexBuffer_->Update(data);
}

void DrawBuffer::UpdateVertices(const void *data) {
    vertexBuffer_->Update(data);
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