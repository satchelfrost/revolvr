/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/vulkan_results.h>

namespace rvr {
class DrawBuffer {
private:
    VkDevice device_{VK_NULL_HANDLE};
    VkBuffer indexBuffer_{VK_NULL_HANDLE};
    VkBuffer vertexBuffer_{VK_NULL_HANDLE};
    VkDeviceMemory indexMemory_{VK_NULL_HANDLE};
    VkDeviceMemory vertexMemory_{VK_NULL_HANDLE};
    size_t sizeInBytes_;
    struct {
        uint32_t idx;
        uint32_t vtx;
    } count_ = {0, 0};
    void AllocateBufferMemory(VkPhysicalDevice physicalDevice, VkBuffer buf, VkDeviceMemory *mem) const;

public:
    VkVertexInputBindingDescription bindDesc{};
    std::vector <VkVertexInputAttributeDescription> attrDesc{};
    void Init(VkDevice device, const std::vector<VkVertexInputAttributeDescription> &attr);
    DrawBuffer() = default;
    DrawBuffer(const DrawBuffer &) = delete;
    DrawBuffer &operator=(const DrawBuffer &) = delete;
    DrawBuffer(DrawBuffer &&) = delete;
    DrawBuffer &operator=(DrawBuffer &&) = delete;
    ~DrawBuffer();

    void UpdateIndices(const uint16_t *data, uint32_t elements, uint32_t offset);

    template<typename T>
    void UpdateVertices(const T *data, uint32_t elements, uint32_t offset);

    template<typename T>
    bool Create(VkPhysicalDevice physicalDevice, uint32_t idxCount, uint32_t vtxCount);
};

template<typename T>
bool DrawBuffer::Create(VkPhysicalDevice physicalDevice, uint32_t idxCount, uint32_t vtxCount) {
    VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufInfo.size = sizeof(uint16_t) * idxCount;
    CHECK_VKCMD(vkCreateBuffer(device_, &bufInfo, nullptr, &indexBuffer_));
    AllocateBufferMemory(physicalDevice, indexBuffer_, &indexMemory_);
    CHECK_VKCMD(vkBindBufferMemory(device_, indexBuffer_, indexMemory_, 0));

    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = sizeof(T) * vtxCount;
    CHECK_VKCMD(vkCreateBuffer(device_, &bufInfo, nullptr, &vertexBuffer_));
    AllocateBufferMemory(physicalDevice, vertexBuffer_, &vertexMemory_);
    CHECK_VKCMD(vkBindBufferMemory(device_, vertexBuffer_, vertexMemory_, 0));

    bindDesc.binding = 0;
    bindDesc.stride = sizeof(T);
    bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    count_ = {idxCount, vtxCount};

    return true;
}

template<typename T>
void DrawBuffer::UpdateVertices(const T *data, uint32_t elements, uint32_t offset) {
    T *map = nullptr;
    CHECK_VKCMD(vkMapMemory(device_, vertexMemory_, sizeof(map[0]) * offset,
                            sizeof(map[0]) * elements, 0, (void **) &map));
    for (size_t i = 0; i < elements; ++i)
       map[i] = data[i];
    vkUnmapMemory(device_, vertexMemory_);
}
}