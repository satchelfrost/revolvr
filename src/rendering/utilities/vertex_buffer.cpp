/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/vertex_buffer.h>
#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
DrawBuffer::~DrawBuffer() {
    if (device_ != nullptr) {
        if (indexBuffer_ != VK_NULL_HANDLE) {
            vkDestroyBuffer(device_, indexBuffer_, nullptr);
        }
        if (indexMemory_ != VK_NULL_HANDLE) {
            vkFreeMemory(device_, indexMemory_, nullptr);
        }
        if (vertexBuffer_ != VK_NULL_HANDLE) {
            vkDestroyBuffer(device_, vertexBuffer_, nullptr);
        }
        if (vertexMemory_ != VK_NULL_HANDLE) {
            vkFreeMemory(device_, vertexMemory_, nullptr);
        }
    }
    indexBuffer_ = VK_NULL_HANDLE;
    indexMemory_ = VK_NULL_HANDLE;
    vertexBuffer_ = VK_NULL_HANDLE;
    vertexMemory_ = VK_NULL_HANDLE;
    bindDesc = {};
    attrDesc.clear();
    count_ = {0, 0};
    device_ = nullptr;
}

void DrawBuffer::Init(VkDevice device, const std::vector<VkVertexInputAttributeDescription> &attr) {
    attrDesc = attr;
    device_ = device;
}

void DrawBuffer::AllocateBufferMemory(VkPhysicalDevice physicalDevice, VkBuffer buf, VkDeviceMemory *mem) const {
    VkMemoryRequirements memReq = {};
    vkGetBufferMemoryRequirements(device_, buf, &memReq);
    uint32_t memIdx = FindMemoryType(physicalDevice, memReq.memoryTypeBits,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkMemoryAllocateInfo allocInfo = CreateMemAllocInfo(memReq.size, memIdx);
    VkResult result = vkAllocateMemory(device_, &allocInfo, nullptr, mem);
    CHECK_VKCMD(result);
}


void DrawBuffer::UpdateIndices(const uint16_t *data, uint32_t elements, uint32_t offset) {
    uint16_t *map = nullptr;
    CHECK_VKCMD(vkMapMemory(device_, indexMemory_, sizeof(map[0]) * offset,
                            sizeof(map[0]) * elements, 0, (void **) &map));
    for (size_t i = 0; i < elements; ++i)
        map[i] = data[i];
    vkUnmapMemory(device_, indexMemory_);
}
}