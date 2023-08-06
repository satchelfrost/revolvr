/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/vertex_buffer.h>
#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
VertexBufferBase::~VertexBufferBase() {
    if (device_ != nullptr) {
        if (idxBuf != VK_NULL_HANDLE) {
            vkDestroyBuffer(device_, idxBuf, nullptr);
        }
        if (idxMem != VK_NULL_HANDLE) {
            vkFreeMemory(device_, idxMem, nullptr);
        }
        if (vtxBuf != VK_NULL_HANDLE) {
            vkDestroyBuffer(device_, vtxBuf, nullptr);
        }
        if (vtxMem != VK_NULL_HANDLE) {
            vkFreeMemory(device_, vtxMem, nullptr);
        }
    }
    idxBuf = VK_NULL_HANDLE;
    idxMem = VK_NULL_HANDLE;
    vtxBuf = VK_NULL_HANDLE;
    vtxMem = VK_NULL_HANDLE;
    bindDesc = {};
    attrDesc.clear();
    count = {0, 0};
    device_ = nullptr;
}

void VertexBufferBase::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                            const std::vector<VkVertexInputAttributeDescription> &attr) {
    device_ = device;
    attrDesc = attr;
}

void VertexBufferBase::AllocateBufferMemory(VkPhysicalDevice physicalDevice, VkBuffer buf, VkDeviceMemory *mem) const {
    VkMemoryRequirements memReq = {};
    vkGetBufferMemoryRequirements(device_, buf, &memReq);
    uint32_t memIdx = FindMemoryType(physicalDevice, memReq.memoryTypeBits,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkMemoryAllocateInfo allocInfo = CreateMemAllocInfo(memReq.size, memIdx);
    VkResult result = vkAllocateMemory(device_, &allocInfo, nullptr, mem);
    CHECK_VKCMD(result);
}
}