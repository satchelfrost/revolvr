/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>
#include <rendering/utilities/vulkan_buffer.h>

namespace rvr {
class DrawBuffer {
private:
    std::unique_ptr<VulkanBuffer> indexBuffer_;
    std::unique_ptr<VulkanBuffer> vertexBuffer_;

public:
    DrawBuffer(const std::shared_ptr<RenderingContext>& context, size_t sizeOfIndex, size_t sizeOfIndices,
               size_t sizeOfVertex, size_t sizeOfVertices);
    void UpdateIndices(const void *data);
    void UpdateVertices(const void *data);
};
}