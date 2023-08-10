/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/vulkan_results.h>
#include <rendering/utilities/rendering_context.h>
#include "vulkan_buffer.h"

namespace rvr {
class DrawBuffer {
private:
    VkDevice device_{VK_NULL_HANDLE};
    std::shared_ptr<VulkanBuffer> indexBuffer_;
    std::shared_ptr<VulkanBuffer> vertexBuffer_;
    std::shared_ptr<RenderingContext> renderingContext_;

public:
    VkVertexInputBindingDescription bindDesc{};
    std::vector <VkVertexInputAttributeDescription> attrDesc{};
    void Init(const std::shared_ptr<RenderingContext>& context, const std::vector<VkVertexInputAttributeDescription> &attr);
    DrawBuffer() = default;
    DrawBuffer(const DrawBuffer &) = delete;
    DrawBuffer &operator=(const DrawBuffer &) = delete;
    DrawBuffer(DrawBuffer &&) = delete;
    DrawBuffer &operator=(DrawBuffer &&) = delete;

    void UpdateIndices(const void *data);
    void UpdateVertices(const void *data);

    void CreateVertexBuffer(size_t sizeInBytes);
    void CreateIndexBuffer(size_t sizeInBytes);
};

template<typename T>
bool DrawBuffer::Create(VkPhysicalDevice physicalDevice, uint32_t idxCount, uint32_t vtxCount) {

    bindDesc.binding = 0;
    bindDesc.stride = sizeof(T);
    bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    count_ = {idxCount, vtxCount};

    return true;
}
}