/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>
#include <rendering/utilities/vulkan_buffer.h>
#include <rendering/utilities/vertex_buffer_layout.h>

namespace rvr {
class DrawBuffer {
private:
    std::unique_ptr<VulkanBuffer> indexBuffer_;
    std::unique_ptr<VulkanBuffer> vertexBuffer_;

public:
    DrawBuffer(std::unique_ptr<VulkanBuffer> indexBuffer, std::unique_ptr<VulkanBuffer> vertexBuffer);
    void UpdateIndices(const void *data);
    void UpdateVertices(const void *data);
    VkBuffer GetIndexBuffer();
    VkBuffer GetVertexBuffer();
    uint32_t GetIndexCount();
};
}