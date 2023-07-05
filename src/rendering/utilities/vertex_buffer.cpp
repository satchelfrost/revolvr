/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <rendering/utilities/vertex_buffer.h>
#include <rendering/utilities/memory_allocator.h>

VertexBufferBase::~VertexBufferBase() {
    if (m_vkDevice != nullptr) {
        if (idxBuf != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_vkDevice, idxBuf, nullptr);
        }
        if (idxMem != VK_NULL_HANDLE) {
            vkFreeMemory(m_vkDevice, idxMem, nullptr);
        }
        if (vtxBuf != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_vkDevice, vtxBuf, nullptr);
        }
        if (vtxMem != VK_NULL_HANDLE) {
            vkFreeMemory(m_vkDevice, vtxMem, nullptr);
        }
    }
    idxBuf = VK_NULL_HANDLE;
    idxMem = VK_NULL_HANDLE;
    vtxBuf = VK_NULL_HANDLE;
    vtxMem = VK_NULL_HANDLE;
    bindDesc = {};
    attrDesc.clear();
    count = {0, 0};
    m_vkDevice = nullptr;
}

void VertexBufferBase::Init(VkDevice device, const MemoryAllocator *memAllocator,
                            const std::vector<VkVertexInputAttributeDescription> &attr) {
    m_vkDevice = device;
    m_memAllocator = memAllocator;
    attrDesc = attr;
}

void VertexBufferBase::AllocateBufferMemory(VkBuffer buf, VkDeviceMemory *mem) const {
    VkMemoryRequirements memReq = {};
    vkGetBufferMemoryRequirements(m_vkDevice, buf, &memReq);
    m_memAllocator->Allocate(memReq, mem);
}
