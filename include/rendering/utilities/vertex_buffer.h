#pragma once

#include <pch.h>
#include <common.h>
#include "memory_allocator.h"

// VertexBuffer base class
struct VertexBufferBase {
    VkBuffer idxBuf{VK_NULL_HANDLE};
    VkDeviceMemory idxMem{VK_NULL_HANDLE};
    VkBuffer vtxBuf{VK_NULL_HANDLE};
    VkDeviceMemory vtxMem{VK_NULL_HANDLE};
    VkVertexInputBindingDescription bindDesc{};
    std::vector <VkVertexInputAttributeDescription> attrDesc{};
    struct {
        uint32_t idx;
        uint32_t vtx;
    } count = {0, 0};

    VertexBufferBase() = default;

    ~VertexBufferBase() {
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

    VertexBufferBase(const VertexBufferBase &) = delete;

    VertexBufferBase &operator=(const VertexBufferBase &) = delete;

    VertexBufferBase(VertexBufferBase &&) = delete;

    VertexBufferBase &operator=(VertexBufferBase &&) = delete;

    void Init(VkDevice device, const MemoryAllocator *memAllocator,
              const std::vector <VkVertexInputAttributeDescription> &attr) {
        m_vkDevice = device;
        m_memAllocator = memAllocator;
        attrDesc = attr;
    }

protected:
    VkDevice m_vkDevice{VK_NULL_HANDLE};

    void AllocateBufferMemory(VkBuffer buf, VkDeviceMemory *mem) const {
        VkMemoryRequirements memReq = {};
        vkGetBufferMemoryRequirements(m_vkDevice, buf, &memReq);
        m_memAllocator->Allocate(memReq, mem);
    }

private:
    const MemoryAllocator *m_memAllocator{nullptr};
};

// VertexBuffer template to wrap the indices and vertices
template<typename T>
struct VertexBuffer : public VertexBufferBase {
    bool Create(uint32_t idxCount, uint32_t vtxCount) {
        VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufInfo.size = sizeof(uint16_t) * idxCount;
        CHECK_VKCMD(vkCreateBuffer(m_vkDevice, &bufInfo, nullptr, &idxBuf));
        AllocateBufferMemory(idxBuf, &idxMem);
        CHECK_VKCMD(vkBindBufferMemory(m_vkDevice, idxBuf, idxMem, 0));

        bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufInfo.size = sizeof(T) * vtxCount;
        CHECK_VKCMD(vkCreateBuffer(m_vkDevice, &bufInfo, nullptr, &vtxBuf));
        AllocateBufferMemory(vtxBuf, &vtxMem);
        CHECK_VKCMD(vkBindBufferMemory(m_vkDevice, vtxBuf, vtxMem, 0));

        bindDesc.binding = 0;
        bindDesc.stride = sizeof(T);
        bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        count = {idxCount, vtxCount};

        return true;
    }

    void UpdateIndices(const uint16_t *data, uint32_t elements, uint32_t offset = 0) {
        uint16_t *map = nullptr;
        CHECK_VKCMD(vkMapMemory(m_vkDevice, idxMem, sizeof(map[0]) * offset,
                                sizeof(map[0]) * elements, 0, (void **) &map));
        for (size_t i = 0; i < elements; ++i) {
            map[i] = data[i];
        }
        vkUnmapMemory(m_vkDevice, idxMem);
    }

    void UpdateVertices(const T *data, uint32_t elements, uint32_t offset = 0) {
        T *map = nullptr;
        CHECK_VKCMD(vkMapMemory(m_vkDevice, vtxMem, sizeof(map[0]) * offset,
                                sizeof(map[0]) * elements, 0, (void **) &map));
        for (size_t i = 0; i < elements; ++i) {
            map[i] = data[i];
        }
        vkUnmapMemory(m_vkDevice, vtxMem);
    }
};

