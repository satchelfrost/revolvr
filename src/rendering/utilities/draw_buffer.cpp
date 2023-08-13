/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/draw_buffer.h>
#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
DrawBuffer::DrawBuffer(const std::shared_ptr<RenderingContext>& context, size_t sizeOfIndex, size_t sizeOfIndices,
                       size_t sizeOfVertex, size_t sizeOfVertices) {
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
}