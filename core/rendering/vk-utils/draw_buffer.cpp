/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "draw_buffer.h"
#include "vk_utils.h"

#include <utility>

namespace rvr {
DrawBuffer::DrawBuffer(std::unique_ptr<Buffer> indexBuffer, std::unique_ptr<Buffer> vertexBuffer) :
indexBuffer_(std::move(indexBuffer)), vertexBuffer_(std::move(vertexBuffer)) {}

void DrawBuffer::UpdateIndices(const void *data) {
    indexBuffer_->Map();
    indexBuffer_->WriteToBuffer(data);
    indexBuffer_->Unmap();
}

void DrawBuffer::UpdateVertices(const void *data) {
    vertexBuffer_->Map();
    vertexBuffer_->WriteToBuffer(data);
    vertexBuffer_->Unmap();
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
