/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/draw_buffer.h>
#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
void DrawBuffer::Init(const std::shared_ptr<RenderingContext>& context,
                      const std::vector<VkVertexInputAttributeDescription> &attr) {
    attrDesc = attr;
    device_ = renderingContext_->GetDevice();
    renderingContext_ = context;
}

void DrawBuffer::CreateVertexBuffer(size_t sizeInBytes) {
    vertexBuffer_ = std::make_shared<VulkanBuffer>(renderingContext_, sizeInBytes,
                                                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void DrawBuffer::CreateIndexBuffer(size_t sizeInBytes) {
    indexBuffer_ = std::make_shared<VulkanBuffer>(renderingContext_, sizeInBytes,
                                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void DrawBuffer::UpdateIndices(const void *data) {
    indexBuffer_->Update(data);
}

void DrawBuffer::UpdateVertices(const void *data) {
    vertexBuffer_->Update(data);
}
}