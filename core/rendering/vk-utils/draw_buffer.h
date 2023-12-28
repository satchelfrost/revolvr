/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include "rendering_context.h"
#include "buffer.h"
#include "vertex_buffer_layout.h"

namespace rvr {
class DrawBuffer {
private:
    std::unique_ptr<Buffer> indexBuffer_;
    std::unique_ptr<Buffer> vertexBuffer_;

public:
    DrawBuffer(std::unique_ptr<Buffer> indexBuffer, std::unique_ptr<Buffer> vertexBuffer);
    void UpdateIndices(const void *data);
    void UpdateVertices(const void *data);
    VkBuffer GetIndexBuffer();
    VkBuffer GetVertexBuffer();
    uint32_t GetIndexCount();
};
}
